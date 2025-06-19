#include "stdlib.h"
#include "pico/stdlib.h"
#include "pico/stdio_usb.h"

#include "pdm_output.pio.h"
#include <math.h>
#include "bsp/board_api.h"
#include "pico/multicore.h"
#include "hardware/clocks.h"
#include "hardware/vreg.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
#include <hardware/structs/qmi.h>


#define CHANNELS         1
#define SAMPLE_RATE      48000
#define OVERSAMPLE       64
#define PDM_RATE         (SAMPLE_RATE * OVERSAMPLE)
#define PDM_BITS_PER_SAMPLE OVERSAMPLE
#define PDM_WORDS_PER_SAMPLE (PDM_BITS_PER_SAMPLE / 32)

#define TABLE_SIZE       256     // Sine lookup table
#define BASE_GPIO        16       // GPIO 2–9
#define BUFFER_WORDS     512     // Size of circular DMA buffers (in 32-bit words)

#define PI 3.14159265f

typedef struct {
    PIO pio;
    uint sm;
    uint gpio;
    uint dma_channel;

    volatile uint32_t *buffer;
    volatile uint32_t write_index;

    int32_t accumulator;
    uint32_t phase;
    uint32_t phase_step;
} pdm_channel_t;


// Circular buffers: 8 channels × 512 words (32-bit)
static uint32_t pdm_buffers[CHANNELS][BUFFER_WORDS];

// Channel state
static pdm_channel_t channels[CHANNELS];

// Shared sine table
static uint16_t sine_table[TABLE_SIZE];



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



void setup_pio_and_dma() 
{
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
        ch->dma_channel = dma_claim_unused_channel(true);

        ch->buffer = pdm_buffers[i];
        ch->write_index = 0;

        ch->accumulator = 0;
        ch->phase = 0;

        int freq = 220 + i * 20;
        ch->phase_step = ((TABLE_SIZE << 16) / SAMPLE_RATE) * freq;

        uint offset = (ch->pio == pio0) ? offset0 : offset1;

        // Init PIO
        pdm_output_program_init(ch->pio, ch->sm, offset, ch->gpio);
        float div = (float)clock_get_hz(clk_sys) / PDM_RATE;
        pio_sm_set_clkdiv(ch->pio, ch->sm, div);
        pio_sm_set_enabled(ch->pio, ch->sm, true);

        // DMA config
        dma_channel_config c = dma_channel_get_default_config(ch->dma_channel);
        channel_config_set_transfer_data_size(&c, DMA_SIZE_32);
        channel_config_set_read_increment(&c, true);
        channel_config_set_write_increment(&c, false);
        channel_config_set_ring(&c, true, 9);  // 2^9 = 512 words = 2KB
        channel_config_set_dreq(&c, pio_get_dreq(ch->pio, ch->sm, true));

        dma_channel_configure(
            ch->dma_channel,
            &c,
            &ch->pio->txf[ch->sm],  // Destination: PIO TX FIFO
            ch->buffer,             // Source: circular buffer
            BUFFER_WORDS,           // Initial transfer count
            false                    // Do not start immediately
        );
    }
}

// Read the current DMA read pointer as an index into the buffer
static inline uint dma_get_read_index(pdm_channel_t *ch) 
{
    uintptr_t read_addr = dma_channel_hw_addr(ch->dma_channel)->read_addr;
    return (read_addr - (uintptr_t)(ch->buffer)) / sizeof(uint32_t);
}

// Fill sine wave table (0–65535)
void generate_sine_table() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        float phase = 2 * PI * i / TABLE_SIZE;
        float val = (sinf(phase) * 0.9f + 1.0f) * 0.5f;
        sine_table[i] = (uint16_t)(val * 65535.0f);
    }
}



void refill_channel_buffer(pdm_channel_t *ch) 
{
    uint read_index = dma_get_read_index(ch);
    uint available = (read_index + BUFFER_WORDS - ch->write_index - 1) % BUFFER_WORDS;
    printf("available: %d\n", available);
    printf(" wr=%u rd=%u avail=%u\n", ch->write_index, read_index, available);
    printf("CH%d: read_addr = 0x%08x\n", 0,  (uint32_t)dma_channel_hw_addr(ch->dma_channel)->read_addr);

    while (available >= PDM_WORDS_PER_SAMPLE) 
    {
        uint16_t sample = sine_table[ch->phase >> 16];

        // Generate PDM words for 1 sample
        for (int w = 0; w < PDM_WORDS_PER_SAMPLE; ++w) 
        {
            uint32_t bits = 0;
            for (int i = 0; i < 32; ++i) 
            {
                int32_t target = (ch->accumulator >= 0) ? 65535 : 0;
                ch->accumulator += ((int32_t)sample - target);
                bits = (bits << 1) | (ch->accumulator >= 0);
            }

            ch->buffer[ch->write_index] = bits;
            ch->write_index = (ch->write_index + 1) % BUFFER_WORDS;
        }

        ch->phase += ch->phase_step;
        if ((ch->phase >> 16) >= TABLE_SIZE)
            ch->phase -= (TABLE_SIZE << 16);

        available -= PDM_WORDS_PER_SAMPLE;
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


    // vreg_set_voltage(VREG_VOLTAGE_1_20);    //400 Mhz may be highest achievable at 1.30v. 
    // uint32_t clock_speed = 300000;
    // set_sys_clock_khz(clock_speed, true);        //400 Mhz may be too unstable

    stdio_init_all();

    stdio_usb_init();
    while (!stdio_usb_connected()) {
        sleep_ms(100);  // Optional: wait for terminal to connect
    }
    generate_sine_table();
    setup_pio_and_dma();

    for (int i = 0; i < CHANNELS; ++i) 
    {
        refill_channel_buffer(&channels[i]);
    }
    for (int i = 0; i < CHANNELS; ++i) 
    {
        dma_channel_start(channels[i].dma_channel);
    }
    refill_channel_buffer(&channels[0]);
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
            
            for (int i = 0; i < CHANNELS; ++i) {
            refill_channel_buffer(&channels[i]);
        }
        }


        
    }

    return 0;
}
