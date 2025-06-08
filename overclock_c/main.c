#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "bsp/board_api.h"
#include "hardware/clocks.h"
#include "hardware/adc.h"
void pico_led_init(void);
void pico_set_led(bool led_on);
//**************************************************************
//**************************************************************
//**********   SECOND MAIN LOOP FOR SECOND CORE  !!  ***********
//**************************************************************
//**************************************************************

uint32_t core1_this_time, core1_this_count;
void core1_main()
{

    while(true)
    {
        //**********************************
        //***  PRINT RUNS THROUGH MAIN   ***
        //**********************************
        if(board_millis() - core1_this_time > 999)
        {
            core1_this_time = board_millis();

            
            printf("\nCore 1 - Runs through main: %d\n", core1_this_count);
            
            core1_this_count = 0;
        }

        core1_this_count++;
    }

    
}

//**************************************************************
//**************************************************************
//**************************************************************
//**************************************************************
//**************************************************************

uint32_t this_count;
uint32_t this_time, blink_time, this_millis;
bool led_state;



int main()
{
    set_sys_clock_khz(300000, true);

    //***  USB UART INIT  ****
    stdio_init_all();

    adc_init();
    adc_set_temp_sensor_enabled(true);
    adc_select_input(4);   //on RP2350A 4 is temp sensor, on RP2350B 8 is temp sensor


    this_time = board_millis();
    multicore_launch_core1(core1_main);  
    pico_led_init();
    while (true) 
    {

        //**********************************
        //***  PRINT RUNS THROUGH MAIN   ***
        //**********************************
        if(board_millis() - this_time > 999)
        {
            this_time = board_millis();

            float volt = (3.3/4096)*adc_read();
            float temp_c = 27-(volt-0.706)/0.001721;
            float temp_f = (temp_c * (9/5)) + 32;
            printf("Core 0 - Runs through main: %d\n", this_count);
            printf("Time (in millis)          : %d\n", this_time);
            printf("Core Temp:  %.1fF   %.1fC\n\n", temp_f, temp_c);

            this_count = 0;
        }

        //********************
        //***  BLINK LED   ***
        //********************
        if(board_millis() - blink_time > 999)
        {
            blink_time = board_millis();

            led_state = !led_state;
            pico_set_led(led_state);

        }


        
        this_count++;

    }
}




// Perform initialisation
void pico_led_init(void) 

{
   
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
   
}





// Turn the led on or off
void pico_set_led(bool led_on) 
{
    // Just set the GPIO on or off
    gpio_put(PICO_DEFAULT_LED_PIN, led_on);

}