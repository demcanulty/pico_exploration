/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "tusb.h"               //
#include "bsp/board_api.h"

#ifndef LED_DELAY_MS
#define LED_DELAY_MS 1000
#endif


//#define PICO_USE_FASTEST_SUPPORTED_CLOCK 1
//#define SYS_CLK_MHZ  200

//Blink Pico  RP2040 at 133Mhz - 1,373,298 runs through main
//Blink Pico  RP2040 at 200Mhz - 2,197,497 runs through main
//Blink Pico2 RP2350 at 150Mhz - 2,542,031 runs through main


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


uint32_t this_count;
uint32_t this_time;
int main() 
{
    bool led_state;
    stdio_init_all();
    // while (!tud_cdc_connected()) 
    // {
    //     tight_loop_contents();
    // }
    
     printf("hello\n");
    int rc = pico_led_init();
    hard_assert(rc == PICO_OK);



    this_time = board_millis();

    while (true) 
    {

        if(board_millis() - this_time > 999)
        {
            this_time = board_millis();

            led_state = !led_state;
            pico_set_led(led_state);


            printf("Runs through main: %d\n", this_count);
            printf("Time (in millis) : %d\n\n", this_time);

            this_count = 0;
        }

        

        this_count++;



    }
}
