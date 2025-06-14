


#include "pico/stdlib.h"
#include "tusb.h"             
#include "tusb_config.h"  
#include "bsp/board_api.h"
#include "pico/multicore.h"

//*** MINE  
#include "main.h"
#include "blink.h"
#include "common.h"
#include "midi.h"

// & then pot stuff:
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "pico/stdio.h"


//****  SEE MAIN.H FOR TINYUSB LIBRARY FIX  ***

uint32_t blink_interval_ms = BLINK_NOT_MOUNTED;

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
uint32_t this_time, blink_time, this_millis, adc_time;
bool led_state;

//***********************************************
//***********************************************
//*********  CORE 0 MAIN  ***********************
//***********************************************
//***********************************************

int main()
{
    board_init();
    


    //***********************
    //***  TINY USB INIT  ***
    //***********************
   
    tusb_rhport_init_t dev_init = {     // init device stack on configured roothub port
        .role = TUSB_ROLE_DEVICE,
        .speed = TUSB_SPEED_AUTO
    };
    tusb_init(BOARD_TUD_RHPORT, &dev_init);

    //********************
    //***  OTHER INIT  ***
    //********************

    
    stdio_init_all();       //***  USB UART INIT  ****
    pico_led_init();
    adc_init();
    adc_gpio_init(26);
    adc_select_input(0);

    //*********************************************************************************************
    //*********************************************************************************************
    //******************************  MAIN LOOP  **************************************************
    //*********************************************************************************************
    //*********************************************************************************************
   
    this_time = board_millis();
    //multicore_launch_core1(core1_main);  
    while (true) 
    {
        //*****************************
        //***  TinyUSB device task  ***
        //*****************************
        tud_task();                            
        
        //*******************
        //***  ADC PRINT  ***
        //*******************

        if(board_millis() - adc_time > 20)  //every 20 milliseconds
        {
            adc_time = board_millis();
            printf("ADC value: %d\n", adc_read()); // Print ADC value for pot
        }
        

        //********************
        //***  BLINK LED   ***
        //********************
        if(board_millis() - blink_time > blink_interval_ms)
        {
            blink_time = board_millis();

            led_state = !led_state;
            pico_set_led(led_state);
            send_test_cc();

        }
        
        //*******************************
        //***  ONCE PER MILLISECOND   ***
        //*******************************
        if(board_millis() != this_millis)
        {
            this_millis = board_millis();
            usb_midi_task();
        }
        /*
        //**********************************
        //***  PRINT RUNS THROUGH MAIN   ***
        //**********************************
        if(board_millis() - this_time > 999)
        {
            this_time = board_millis();

            
            printf("Core 0 - Runs through main: %d\n", this_count);
            printf("Time (in millis)          : %d\n\n", this_time);
            
            this_count = 0;
        }
        this_count++;
        */
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




