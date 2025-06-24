

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
#define PIN_MISO 2
#define PIN_CS   3
#define PIN_SCK  4
#define PIN_MOSI 5
#define SPI_PORT spi0










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



        this_count++;



    }
}
