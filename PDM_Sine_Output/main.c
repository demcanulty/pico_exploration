#include "stdlib.h"
#include "pico/stdlib.h"
#include "pico/stdio_usb.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "pdm_output.pio.h"
#include <math.h>
#include "bsp/board_api.h"
#include "pico/multicore.h"
#include "hardware/clocks.h"
#include "hardware/vreg.h"
#include <hardware/structs/qmi.h>


#define PDM_GPIO      16
#define SAMPLE_RATE   48000
#define OVERSAMPLE    64
#define PDM_BITRATE   (SAMPLE_RATE * OVERSAMPLE)  // e.g. 3.072 MHz
#define TABLE_SIZE    256
#define PI            3.14159265f

int32_t accumulator = 0;


void pdm_output_program_init(PIO pio, uint state_machine, uint offset, uint pin) 
{
    // Set the pin as a PIO-controlled output
    pio_sm_set_consecutive_pindirs(pio, state_machine, pin, 1, true);
    pio_gpio_init(pio, pin);

    // Configure the state machine
    pio_sm_config c = pdm_output_program_get_default_config(offset);
    sm_config_set_out_pins(&c, pin, 1);
    sm_config_set_out_shift(&c, false, true, 32);  // SHIFT_LEFT, autopull, 32 bits
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_TX);

    // Initialize the state machine
    pio_sm_init(pio, state_machine, offset, &c);
}

// Fill a sine table with 16-bit unsigned values
uint16_t sine_table[TABLE_SIZE];

void generate_sine_table() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        float phase = 2.0f * PI * i / TABLE_SIZE;
        float amplitude = (sinf(phase) * 0.9f + 1.0f) * 0.5f;  // 0.05–0.95
        sine_table[i] = (uint16_t)(amplitude * 65535.0f);
    }
}

// Sigma-delta PDM generator: 2x 32-bit words per 16-bit sample
void generate_pdm_words(uint16_t sample, uint32_t *out_words) {
    for (int word_index = 0; word_index < 2; ++word_index) {
        uint32_t bits = 0;
        for (int i = 0; i < 32; ++i) {
            int32_t target = (accumulator >= 0) ? 65535 : 0;
            int32_t error = (int32_t)sample - target;
            accumulator += error;
            uint32_t bit = (accumulator >= 0) ? 1 : 0;
            bits = (bits << 1) | bit;
        }
        out_words[word_index] = bits;
    }
}



uint32_t this_count;
uint32_t this_time, blink_time, this_millis;
bool led_state;



int main() 
{
    //***  REDUCE FLASH TIMING CLOCK  ***
    qmi_hw->m[0].timing |= 0x4;             //qmi_hw->m[0].timing now equals 0x60007207  (raise third bit,              qmi clkdiv is now 7)
    //qmi_hw->m[0].timing &= ~(0x3);        //qmi_hw->m[0].timing now equals 0x60007204  (drop first and second bits,   qmi clkdiv is now 4) 
    qmi_hw->m[0].timing &= ~(0x2);        //qmi_hw->m[0].timing now equals 0x60007205  (drop the second bit,          qmi clkdiv is now 5) 
    vreg_set_voltage(VREG_VOLTAGE_1_30);    //400 Mhz may be highest achievable at 1.30v. 
    uint32_t clock_speed = 380000;
    set_sys_clock_khz(clock_speed, true);        //400 Mhz may be too unstable



    stdio_init_all();
    stdio_usb_init();
    while (!stdio_usb_connected()) {
        sleep_ms(100);  // Optional: wait for terminal to connect
    }
    generate_sine_table();

    PIO pio = pio0;
    uint sm = 0;
    uint offset = pio_add_program(pio, &pdm_output_program);
    pdm_output_program_init(pio, sm, offset, PDM_GPIO);

    float clk_div = (float)clock_get_hz(clk_sys) / PDM_BITRATE;
    pio_sm_set_clkdiv(pio, sm, clk_div);
    pio_sm_set_enabled(pio, sm, true);

    uint32_t phase = 0;
    uint32_t phase_step = (TABLE_SIZE << 16) / SAMPLE_RATE * 440;

    uint32_t pdm_words[2];
    int pdm_word_index = 2;  // Start > 1 to trigger initial generation

    while (true) 
    {

        this_count++;
        //**********************************
        //***  PRINT RUNS THROUGH MAIN   ***
        //**********************************
        if(board_millis() - this_time > 999)
        {
            this_time = board_millis();
            printf("\nCore 0 - Runs through main: %d\n", this_count);
            this_count = 0;
        }

        // Check if there is room in the TX FIFO
        if (!pio_sm_is_tx_fifo_full(pio, sm)) {
            // If both words have been sent, generate next sample
            if (pdm_word_index >= 2) {
                uint16_t sample = sine_table[phase >> 16];
                generate_pdm_words(sample, pdm_words);
                phase += phase_step;
                if ((phase >> 16) >= TABLE_SIZE)
                    phase -= (TABLE_SIZE << 16);
                pdm_word_index = 0;
            }

            // Send the next word (non-blocking)
            pio_sm_put(pio, sm, pdm_words[pdm_word_index++]);
        }

        // Optional: sleep or yield if you're doing other work
        // __wfi(); // Wait for interrupt — super low power
        // sleep_us(1); // light delay to avoid spinning too fast
    }
}