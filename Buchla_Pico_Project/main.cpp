#include <stdio.h>
#include "pico/stdlib.h"
// #include "tusb.h"             
// #include "tusb_config.h"  
#include "bsp/board_api.h"


//*** MINE  
#include "main.h"
#include "blink.h"
#include "common.h"

//***  INCLUDE C HEADERS HERE  *****
extern "C" {
  
  
}


uint32_t blink_interval_ms = 1000;//BLINK_NOT_MOUNTED;



//C Project measurements
//Blink Pico  RP2040 at 125Mhz - 1,373,298 runs through main
//Blink Pico  RP2040 at 200Mhz - 2,197,497 runs through main
//Blink Pico2 RP2350 at 150Mhz - 2,542,031 runs through main

//C++ Project measurements, just blinking 
//Blink Pico  RP2040 at 125Mhz - 2,118,127 runs through main
//Blink Pico  RP2040 at 200Mhz - 3,389,315 runs through main
//Blink Pico2 RP2350 at 150Mhz - 4,544,875 runs through main



uint32_t this_count;
uint32_t this_time, blink_time;
bool led_state;


int main()
{
    pico_led_init();


  


    //***  USB UART INIT  ****
    stdio_init_all();


    //*********************************************************************************************
    //*********************************************************************************************
    //******************************  MAIN LOOP  **************************************************
    //*********************************************************************************************
    //*********************************************************************************************
   
    this_time = board_millis();

    while (true) 
    {

        //**********************************
        //***  PRINT RUNS THROUGH MAIN   ***
        //**********************************
        if(board_millis() - this_time > 999)
        {
            this_time = board_millis();

            led_state = !led_state;
            pico_set_led(led_state);
            
            printf("Runs through main: %d\n", this_count);
            printf("Time (in millis) : %d\n\n", this_time);

            this_count = 0;

        }

        // //********************
        // //***  BLINK LED   ***
        // //********************
        // if(board_millis() - blink_time > blink_interval_ms)
        // {
        //     blink_time = board_millis();

            

        // }
        

        this_count++;

        

    }
}




