/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "tusb.h"               //
#include "bsp/board_api.h"
#include  "adc.h"
#include "hardware/clocks.h"
#include "hardware/vreg.h"
#include <hardware/structs/qmi.h>
#ifndef LED_DELAY_MS
#define LED_DELAY_MS 1000
#endif


//#define PICO_USE_FASTEST_SUPPORTED_CLOCK 1
//#define SYS_CLK_MHZ  200

//Blink Pico  RP2040 at 133Mhz - 1,373,298 runs through main
//Blink Pico  RP2040 at 200Mhz - 2,197,497 runs through main
//Blink Pico2 RP2350 at 150Mhz - 2,542,031 runs through main
extern uint32_t adc_interrupt;

// Perform initialisation
int pico_led_init(void) 

{
    // A device like Pico that uses a GPIO for the LED will define PICO_DEFAULT_LED_PIN
    // so we can use normal GPIO functionality to turn the led on and off
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    return PICO_OK;

}

// Turn the led on or off
void pico_set_led(bool led_on) 
{
    // Just set the GPIO on or off
    gpio_put(PICO_DEFAULT_LED_PIN, led_on);

}
//#define OVERCLOCK_300MHZ  
//#define OVERCLOCK_400MHZ      //See RP2350 datasheet, QMI: M0_TIMING, M1_TIMING Registers, CLKDIV bits


uint32_t this_count;
uint32_t this_time, adc_time;
int main() 
{
    float clock_speed;
    #ifdef OVERCLOCK_300MHZ
    vreg_set_voltage(VREG_VOLTAGE_1_20);    //300Mhz was locking up at 1.10v, bumping to 1.20v
    set_sys_clock_khz(300000, true);
    #endif
    #ifdef OVERCLOCK_400MHZ
    //***  REDUCE FLASH TIMING CLOCK  ***
    qmi_hw->m[0].timing |= 0x4;                     //qmi_hw->m[0].timing now equals 0x60007207  (raise third bit,              qmi clkdiv is now 7)
    //qmi_hw->m[0].timing &= ~(0x3);                //qmi_hw->m[0].timing now equals 0x60007204  (drop first and second bits,   qmi clkdiv is now 4) 
    qmi_hw->m[0].timing &= ~(0x2);                  //qmi_hw->m[0].timing now equals 0x60007205  (drop the second bit,          qmi clkdiv is now 5) 
    vreg_set_voltage(VREG_VOLTAGE_1_30);            //400 Mhz may be highest achievable clockspeed at 1.30v. 
    clock_speed = 380000;
    set_sys_clock_khz(clock_speed, true);           //400 Mhz needs clockdiv of 7, 380 Mhz works well with clockdiv of 5
    #endif

    bool led_state;
    stdio_init_all();
    while (!tud_cdc_connected()) 
    {
        tight_loop_contents();
    }
    
    printf("hello\n");
    int rc = pico_led_init();
    hard_assert(rc == PICO_OK);



    this_time = board_millis();

    init_project_adc();

    while (true) 
    {


        

        if(board_millis() - this_time > 999)
        //if(this_count > 4000000)
        {
            this_time = board_millis();

            led_state = !led_state;
            pico_set_led(led_state);


            printf("Runs through main: %d\n", this_count);
            //printf("Time (in millis) : %d\n\n", this_time);
            printf("ADC Interrupt: %d\n", adc_interrupt_count);
            printf("Samples per second: %d\n\n", adc_interrupt_count * NUM_ADC_SAMPLES);
            adc_interrupt_count=0;
            this_count = 0;

           
        }


        
        if(board_millis() - adc_time > 9)
        {
            adc_time = 0;
            check_adc_vals();
            process_adc();
        }        
        
        this_count++;



    }
}
