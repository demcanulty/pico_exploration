#include <stdio.h>
#include "pico/stdlib.h"
#include "tusb.h"             
#include "tusb_config.h"  
#include "bsp/board_api.h"


//*** MINE  
#include "main.h"
#include "blink.h"
#include "common.h"
#include "midi.h"

//***  INCLUDE C HEADERS HERE  *****
extern "C" {
  
  
}


extern uint32_t blink_interval_ms;// = BLINK_NOT_MOUNTED;



//C Project measurements
//Blink Pico  RP2040 at 125Mhz - 1,373,298 runs through main
//Blink Pico  RP2040 at 200Mhz - 2,197,497 runs through main
//Blink Pico2 RP2350 at 150Mhz - 2,542,031 runs through main

//C++ Project measurements, just blinking 
//Blink Pico  RP2040 at 125Mhz - 2,118,127 runs through main
//Blink Pico  RP2040 at 200Mhz - 3,389,315 runs through main
//Blink Pico2 RP2350 at 150Mhz - 4,544,875 runs through main



uint32_t this_count;
uint32_t this_time, blink_time, this_millis;
bool led_state;


int main()
{
    board_init();
    pico_led_init();


    //***********************
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

            
            printf("Runs through main: %d\n", this_count);
            printf("Time (in millis) : %d\n\n", this_time);
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





//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void)
{
  blink_interval_ms = BLINK_MOUNTED;
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
  blink_interval_ms = BLINK_NOT_MOUNTED;
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
  (void)remote_wakeup_en;
  blink_interval_ms = BLINK_SUSPENDED;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
  blink_interval_ms = BLINK_MOUNTED;
}




