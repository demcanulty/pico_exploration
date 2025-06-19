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


#define SAMPLE_RATE    48000
#define OVERSAMPLE     128
#define PDM_RATE       (SAMPLE_RATE * OVERSAMPLE)
#define TABLE_SIZE     256
#define CHANNELS       8
#define BASE_GPIO      15  // GPIO 2–7

#define PDM_WORDS_PER_SAMPLE (OVERSAMPLE / 32)


#define PI 3.14159265f

typedef struct {
    PIO pio;
    uint sm;
    uint gpio;
    int32_t accumulator;
    uint32_t phase;
    uint32_t phase_step;
    uint32_t pdm_words[PDM_WORDS_PER_SAMPLE];
    int word_index;
} pdm_channel_t;

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
pdm_channel_t channels[CHANNELS];
uint16_t sine_table[TABLE_SIZE];

// Fill sine wave table (0–65535)
void generate_sine_table() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        float phase = 2 * PI * i / TABLE_SIZE;
        float val = (sinf(phase) * 0.9f + 1.0f) * 0.5f;
        sine_table[i] = (uint16_t)(val * 65535.0f);
    }
}


// Sigma-delta PDM generator: 2x 32-bit words per 16-bit sample

// void generate_pdm_words(pdm_channel_t *ch, uint16_t sample) 
// {
//     for (int word_index = 0; word_index < 2; word_index++) 
//     {
//         uint32_t bits = 0;
//         for (int i = 0; i < 32; i++) {
//             int32_t target = (ch->accumulator >= 0) ? 65535 : 0;
//             ch->accumulator += ((int32_t)sample - target);
//             bits = (bits << 1) | (ch->accumulator >= 0);
//         }
//         ch->pdm_words[word_index] = bits;
//     }
//     ch->word_index = 0;
// }

 //Sigma-delta PDM generator: 4x 32-bit words per 16-bit sample
void generate_pdm_words(pdm_channel_t *ch, uint16_t sample) {
    for (int w = 0; w < 4; w++) {  // Now 4 words
        uint32_t bits = 0;
        for (int i = 0; i < 32; i++) {
            int32_t target = (ch->accumulator >= 0) ? 65535 : 0;
            ch->accumulator += ((int32_t)sample - target);
            bits = (bits << 1) | (ch->accumulator >= 0);
        }
        ch->pdm_words[w] = bits;
    }
    ch->word_index = 0;
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


    // vreg_set_voltage(VREG_VOLTAGE_1_20);    //400 Mhz may be highest achievable at 1.30v. 
    // uint32_t clock_speed = 300000;
    // set_sys_clock_khz(clock_speed, true);        //400 Mhz may be too unstable

    stdio_init_all();

    stdio_usb_init();
    while (!stdio_usb_connected()) {
        sleep_ms(100);  // Optional: wait for terminal to connect
    }
    generate_sine_table();


    uint offset0 = pio_add_program(pio0, &pdm_output_program);
    uint offset1 = pio_add_program(pio1, &pdm_output_program);

    for (int i = 0; i < CHANNELS; i++) 
    {
        pdm_channel_t *ch = &channels[i];

        switch(i)
        {
            case 0 ... 3:
                ch->pio = pio0;
                ch->sm = i;
            break;
            case 4 ... 7:
                ch->pio = pio1;
                ch->sm = i - 4;
            break;
        }
        
        
        ch->gpio = BASE_GPIO + i;
        ch->phase = 0;
        ch->accumulator = 0;

        int freq = 220 + i * 20;
        ch->phase_step = ((TABLE_SIZE << 16) / SAMPLE_RATE) * freq;

        // Init this channel's PIO pin and SM
        uint offset = (ch->pio == pio0) ? offset0 : offset1;
        pdm_output_program_init(ch->pio, ch->sm, offset, ch->gpio);
        float div = (float)clock_get_hz(clk_sys) / PDM_RATE;
        pio_sm_set_clkdiv(ch->pio, ch->sm, div);
        pio_sm_set_enabled(ch->pio, ch->sm, true);
    }

    while (1) 
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


        for (int i = 0; i < CHANNELS; i++) {
            pdm_channel_t *ch = &channels[i];

            if (!pio_sm_is_tx_fifo_full(ch->pio, ch->sm)) {
                if (ch->word_index >= PDM_WORDS_PER_SAMPLE) {
                    uint16_t sample = sine_table[ch->phase >> 16];
                    generate_pdm_words(ch, sample);
                    ch->phase += ch->phase_step;
                    if ((ch->phase >> 16) >= TABLE_SIZE)
                        ch->phase -= (TABLE_SIZE << 16);
                }

                pio_sm_put(ch->pio, ch->sm, ch->pdm_words[ch->word_index++]);
            }
        }
    }

    return 0;
}
