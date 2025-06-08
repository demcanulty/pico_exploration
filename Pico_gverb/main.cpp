#include <stdio.h>
#include "pico/stdlib.h"


#include "tusb.h"             
#include "tusb_config.h"  
#include "bsp/board_api.h"
#include "pico/multicore.h"


#include "main.h"
#include "blink.h"
#include "common.h"
#include "midi.h"




//**************************************************************
//**************************************************************
//**********   SECOND MAIN LOOP FOR SECOND CORE  !!  ***********
//**************************************************************
//**************************************************************

uint32_t core1_this_time, core1_this_count;


float dt_in_us;
float max_dt_in_us;


bool core_1_trigger_process;
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

            printf("\nave_dt: %d    max_dt: %d\n", (int)ave_dt, (int)max_dt);
            printf("Core 1 - Runs through main: %d\n", core1_this_count);
            
            core1_this_count = 0;


            //**************************************
            //***  CALCULATE AUDIO PERFORMANCE  ****
            //**************************************

            accum_dt_lockout = true;

            
            dt_in_us     = (float) accum_dt / accum_dt_count;
            max_dt_in_us = (float)   max_dt;
            ave_dt = accum_dt / accum_dt_count;
            

            //***  RESET VARIABLES  ***
            accum_dt = 0;
            accum_dt_count = 0;
            max_dt = 0;
            accum_dt_lockout = false;

            


        }

        core1_this_count++;


        if(core_1_trigger_process)
        {
            process_audio();
            core_1_trigger_process = false;
        }
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
    board_init();
    pico_led_init();

    init_audio_code();


    //***********************`
    //***  TINY USB INIT  ***
    //***********************
   
    tusb_rhport_init_t dev_init = {     // init device stack on configured roothub port
        .role = TUSB_ROLE_DEVICE,
        .speed = TUSB_SPEED_AUTO
    };
    tusb_init(BOARD_TUD_RHPORT, &dev_init);



    //***  USB UART INIT  ****
    stdio_init_all();

    //*********************************************************************************************
    //*********************************************************************************************
    //******************************  MAIN LOOP  **************************************************
    //*********************************************************************************************
    //*********************************************************************************************
    this_time = board_millis();
    multicore_launch_core1(core1_main);  
    while (true) 
    {
        

        //*****************************
        //***  TinyUSB device task  ***
        //*****************************
        tud_task();                            
        
        //**********************************
        //***  PRINT RUNS THROUGH MAIN   ***
        //**********************************
        if(board_millis() - this_time > 999)
        {
            this_time = board_millis();

            
            printf("Core 0 - Runs through main: %d\n", this_count);
            printf("Time (in millis)          : %d\n\n", this_time);
            send_test_cc();
            this_count = 0;
        }

        //********************
        //***  BLINK LED   ***
        //********************
        if(board_millis() - blink_time > blink_interval_ms)
        {
            blink_time = board_millis();

            led_state = !led_state;
            pico_set_led(led_state);

            //printf("Co");

        }

        //*******************************
        //***  ONCE PER MILLISECOND   ***
        //*******************************
        if(board_millis() != this_millis)
        {
            this_millis = board_millis();
            usb_midi_task();
        }
        
        this_count++;

    }
}
