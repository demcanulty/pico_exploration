

#include <stdio.h>
#include "pico/stdlib.h"
#include "tusb.h"               
#include "bsp/board_api.h"
#include  "adc.h"
#include "hardware/clocks.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "hardware/spi.h"
#include "hardware/vreg.h"
#include <hardware/structs/qmi.h>
#define LED_DELAY_MS 1000

//SPI configurations
#define PIN_SCK   2
#define PIN_MOSI  3 
#define PIN_MISO  4
#define PIN_CS    5
#define SPI_PORT spi0


#define BUF_SIZE 128
#define BUF_SIZE_BYTES   (BUF_SIZE * 4)
float rx_buffer[BUF_SIZE];
#define SPI_SPEED  20000000

int spi_dma_chan;
dma_channel_config spi_dma_cfg;

void setup_spi_slave() 
{
    spi_init(SPI_PORT, SPI_SPEED);  // Clock is controlled by master
    spi_set_slave(SPI_PORT, true);
    // Configure SPI pins
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_CS,   GPIO_FUNC_SPI);

    spi_set_format(SPI_PORT, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);  
}

void setup_dma_rx() 
{
    spi_dma_chan = dma_claim_unused_channel(true);
    spi_dma_cfg = dma_channel_get_default_config(spi_dma_chan);

    channel_config_set_transfer_data_size(&spi_dma_cfg, DMA_SIZE_8); // 8-bit transfers
    channel_config_set_read_increment(&spi_dma_cfg, false);          // Always read from FIFO
    channel_config_set_write_increment(&spi_dma_cfg, true);          // Fill buffer sequentially
    channel_config_set_dreq(&spi_dma_cfg, spi_get_dreq(SPI_PORT, false));
}

void start_dma_receive(uint8_t *dest, size_t num_bytes) 
{
    dma_channel_configure(
        spi_dma_chan,
        &spi_dma_cfg,
        dest,
        &spi_get_hw(SPI_PORT)->dr,
        num_bytes,
        true  // start immediately
    );
}

void print_floats(const float *buf, size_t count) 
{
    for (size_t i = 0; i < count; ++i) {
        printf("[%03d] %.6f\n", i, buf[i]);
        //printf("%d\n", i, buf[i]);
    }
}


//******************************************
//******************************************
//******************************************


void pico_led_init(void) 

{
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

}

void pico_set_led(bool led_on) 
{
    gpio_put(PICO_DEFAULT_LED_PIN, led_on);

}
//#define OVERCLOCK_300MHZ  
#define OVERCLOCK_400MHZ      //See RP2350 datasheet, QMI: M0_TIMING, M1_TIMING Registers, CLKDIV bits


uint32_t this_count;
uint32_t this_time, adc_time;
int main() 
{
    #ifdef OVERCLOCK_300MHZ
    vreg_set_voltage(VREG_VOLTAGE_1_20);    //300Mhz was locking up at 1.10v, bumping to 1.20v
    set_sys_clock_khz(300000, true);
    #endif
    #ifdef OVERCLOCK_400MHZ
    //***  REDUCE FLASH TIMING CLOCK  ***
    qmi_hw->m[0].timing |= 0x4;                     //qmi_hw->m[0].timing now equals 0x60007207  (raise third bit,              qmi clkdiv is now 7)
    qmi_hw->m[0].timing &= ~(0x2);                  //qmi_hw->m[0].timing now equals 0x60007205  (drop the second bit,          qmi clkdiv is now 5) 
    vreg_set_voltage(VREG_VOLTAGE_1_30);            //400 Mhz may be highest achievable clockspeed at 1.30v. 
    set_sys_clock_khz(380000, true);                //400 Mhz needs clockdiv of 7, 380 Mhz works well with clockdiv of 5
    #endif

    bool led_state;
    stdio_init_all();

    while (!tud_cdc_connected()) 
    {
        tight_loop_contents();
    }
    
    printf("hello\n");
    pico_led_init();


    setup_spi_slave();
    setup_dma_rx();

    printf("SPI DMA RX ready to receive %d floats (%d bytes)...\n", 128, 128 * 4);



    this_time = board_millis();



    while (true) 
    {


        

        if(board_millis() - this_time > 999)
        {
            this_time = board_millis();

            led_state = !led_state;
            pico_set_led(led_state);


            printf("Runs through main: %d\n", this_count);
            //printf("Time (in millis) : %d\n\n", this_time);
            // printf("ADC Interrupt: %d\n", adc_interrupt_count);
            // printf("Samples per second: %d\n\n", adc_interrupt_count * NUM_ADC_SAMPLES);
            adc_interrupt_count=0;
            this_count = 0;

           
        }

        start_dma_receive((uint8_t *)rx_buffer, BUF_SIZE_BYTES);
        dma_channel_wait_for_finish_blocking(spi_dma_chan);
        printf("Received float data:\n");
        print_floats(rx_buffer, BUF_SIZE);

        printf("Waiting for next transfer...\n");


        this_count++;



    }
}
