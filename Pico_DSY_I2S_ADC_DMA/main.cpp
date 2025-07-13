
#include "main.h"

#include "adc.h"
#include "bsp/board_api.h"
#include "midi/midi.h"
#include "hardware/clocks.h"
#include "hardware/adc.h"
#include <hardware/structs/qmi.h>
#include "hardware/vreg.h"
#include "pico/multicore.h"
#include "pico/stdlib.h"
#include "sound_i2s/sound_i2s.h"
#include "spi.h"
#include <stdio.h>
#include "tusb.h"             
#include "tusb_config.h"  
#include "utils/utils.h"


/*
Note: text labels from https://patorjk.com/software/taag/#p=display&f=Bright&t=CORE-1
*/


//#define OVERCLOCK_300MHZ  
#define OVERCLOCK_400MHZ      //See RP2350 datasheet, QMI: M0_TIMING, M1_TIMING Registers, CLKDIV bits


uint32_t blink_interval_ms = BLINK_NOT_MOUNTED;


tusb_rhport_init_t dev_init = // init device stack on configured roothub port
{     
    .role  = TUSB_ROLE_DEVICE,
    .speed = TUSB_SPEED_AUTO
};

//****  INIT PINS  ****
void init_pins(void) 
{
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    // gpio_init(DEBUG_A);
    // gpio_init(DEBUG_B);
    // gpio_init(DEBUG_C);
    // gpio_set_dir(DEBUG_A, GPIO_OUT);
    // gpio_set_dir(DEBUG_B, GPIO_OUT);
    // gpio_set_dir(DEBUG_C, GPIO_OUT);
}

// Turn the led on or off
void pico_set_led(bool led_on) 
{
    // Just set the GPIO on or off
    gpio_put(PICO_DEFAULT_LED_PIN, led_on);
}





//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************



//**************************************************************
//**************************************************************
//**************************************************************
//**************************************************************
//**************************************************************
//***** ..####....####...#####...######...........####..........
//***** .##..##..##..##..##..##..##..............##..##.........
//***** .##......##..##..#####...####....######..######.........
//***** .##..##..##..##..##..##..##..............##..##.........
//***** ..####....####...##..##..######...........####..........
//***** ........................................................

uint32_t core0_main_count;
uint32_t this_time, blink_time, this_millis;
uint32_t us_usb_time;
bool led_state;



int main()
{
    u32 clock_speed;
    //********************
    //***  OVERCLOCK  ***
    //********************
    #ifdef OVERCLOCK_300MHZ
    vreg_set_voltage(VREG_VOLTAGE_1_20);    //300Mhz was locking up at 1.10v, bumping to 1.20v
    clock_speed = 300000;
    set_sys_clock_khz(clock_speed, true);
    #endif

    #ifdef OVERCLOCK_400MHZ
    clock_speed = 380000;
    //***  REDUCE FLASH TIMING CLOCK  ***
    qmi_hw->m[0].timing |= 0x4;                     //qmi_hw->m[0].timing now equals 0x60007207  (raise third bit,              qmi clkdiv is now 7)
    qmi_hw->m[0].timing &= ~(0x2);                  //qmi_hw->m[0].timing now equals 0x60007205  (drop the second bit,          qmi clkdiv is now 5) 
    vreg_set_voltage(VREG_VOLTAGE_1_30);            //400 Mhz may be highest achievable clockspeed at 1.30v. 
    set_sys_clock_khz(clock_speed, true);           //400 Mhz needs clockdiv of 7, 380 Mhz works well with clockdiv of 5
    #endif

    board_init();
    init_pins();

    init_audio_processing();


    //***********************
    //***  TINY USB INIT  ***
    //***********************
   
    
    tusb_init(BOARD_TUD_RHPORT, &dev_init);


    //************************
    //***  USB UART INIT  ****
    //************************

    stdio_init_all();

    //***  wait for connection
    while(!tud_cdc_connected())
    {
        tud_task();    
    }
    startup_splash();

    //******************************************
    //****  I2S AUDIO OUT  *********************
    //******************************************
    sound_i2s_init(&sound_config);
    sound_i2s_playback_start();
    
    //*******************
    //***  ADC INIT  ****
    //*******************
    //***  ADC HANDLED IN CORE 1  ***

    //*******************
    //***  SPI INIT  ****
    //*******************
    init_spi();

    //*********************** .##...##...####...######..##..##. ***************
    //*********************** .###.###..##..##....##....###.##. ***************
    //*********************** .##.#.##..######....##....##.###. ***************
    //*********************** .##...##..##..##....##....##..##. ***************
    //*********************** .##...##..##..##..######..##..##. ***************
    //*********************** ................................. ***************

    this_time = board_millis();
    multicore_launch_core1(core1_main);  
    
    while (true) 
    {
        

        
        //*****************************
        //***  TinyUSB device task  ***
        //*****************************
        if(timer_hw->timerawl - us_usb_time > 200)
        {
            us_usb_time = timer_hw->timerawl;
            tud_task();  
        }
                                  
        
        //**********************************
        //***  PRINT RUNS THROUGH MAIN   ***
        //**********************************
        if(board_millis() - this_time > 999)
        {
            this_time = board_millis();

            print_cpu_performance_information(0);
            
            //spi_dma_transfer(random_data, sizeof(random_data));
            //spi_dma_transfer(buffer_0, sizeof(buffer_0));
            //gpio_put(DEBUG_C, !gpio_get(DEBUG_C));
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
        
        core0_main_count++;

    }
}



//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************






//**************************************************************
//**************************************************************
//**************************************************************
//**************************************************************
//**************************************************************
//***** ..####....####...#####...######............##...........
//***** .##..##..##..##..##..##..##...............###...........
//***** .##......##..##..#####...####....######....##...........
//***** .##..##..##..##..##..##..##................##...........
//***** ..####....####...##..##..######..........######.........
//***** ........................................................

uint32_t core1_this_time, core1_main_count;

u32 adc_millis;
uint32_t dt_in_us;
uint32_t max_dt_in_us;


bool core_1_trigger_process;
void core1_main()
{



    //*******************
    //***  ADC INIT  ****
    //*******************
    printf("about to start adc init\n");
    init_project_adc();

    //*********************** .##...##...####...######..##..##. ***************
    //*********************** .###.###..##..##....##....###.##. ***************
    //*********************** .##.#.##..######....##....##.###. ***************
    //*********************** .##...##..##..##....##....##..##. ***************
    //*********************** .##...##..##..##..######..##..##. ***************
    //*********************** ................................. ***************
    while(true)
    {

        //***********************************
        //***  ADC EVERY 2 milliseconds   ***
        //***********************************
        if(board_millis() - adc_millis > 1) 
        {
            adc_millis = board_millis();
            process_adc();
            check_adc_vals();           
        }

        //*************************
        //***  Once Per Second  ***
        //*************************
        if(board_millis() - core1_this_time > 999)
        {
            core1_this_time = board_millis();

        }

        core1_main_count++;

        //*************************
        //***  PROCESS AUDIO
        //*************************
        if(core_1_trigger_process)
        {
            process_audio();
            core_1_trigger_process = false;
        }
    }
}





//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************






//***************************************************************
//***************************************************************
//***************************************************************
//***************************************************************
//***** .##...##..######..######..#####...######...####....####..
//***** .###.###..##........##....##..##....##....##..##..##.....
//***** .##.#.##..####......##....#####.....##....##.......####..
//***** .##...##..##........##....##..##....##....##..##......##.
//***** .##...##..######....##....##..##..######...####....####..

                                                         

void print_cpu_performance_information(u8 skipnum)
{

    static uint8_t this_count;

    this_count++;
    if(this_count < skipnum)
    {
        //just clear all the variables and return
        accum_dt = 0;
        accum_dt_count = 0;
        max_dt = 0;
        accum_dt_lockout = false;
        core0_main_count = 0;
        audio_interrupt_count = 0;
        core1_main_count = 0;
        return;
    }

    this_count = 0;
    //**************************************
    //***  CALCULATE AUDIO PERFORMANCE  ****
    //**************************************

    accum_dt_lockout = true;

    dt_in_us     = accum_dt / accum_dt_count;
    max_dt_in_us = max_dt;
    ave_dt_in_us = accum_dt / accum_dt_count;

    //*************************
    //***  RESET VARIABLES  ***
    //*************************
    accum_dt = 0;
    accum_dt_count = 0;
    max_dt = 0;
    accum_dt_lockout = false;


    // ***  NOW WITH COMMA FORMATTING  !
    char formatted_num_string[20];

    //****************
    //***  CORE 0  ***
    //****************
    format_with_commas(core0_main_count, formatted_num_string);
    printf("\nCore 0 - Runs through main: %s\n", formatted_num_string);

    //****************
    //***  CORE 1  ***
    //****************
    format_with_commas(core1_main_count, formatted_num_string);
    printf("Core 1 - Runs through main: %s\n", formatted_num_string);

    //**********************
    //***  Running Time  ***
    //**********************
    
    // printf("Time (in millis)          : %d\n", this_time);

    //******************************
    //***  AUDIO INTERRUPT INFO  ***
    //******************************

    float percentage_full =   (((float)max_dt_in_us) / MAX_TIME_FOR_AUDIO_INTERRUPT)  * 100;

    //printf("Audio Interrupts per sec: %d\n", audio_interrupt_count);
    printf("ave_dt_in_us: %d    max_dt: %d   CPU%%: %d\n", (int)ave_dt_in_us, (int)max_dt_in_us, (int)percentage_full);


    //*******************
    //***  CPU CLOCK  ***
    //*******************

    // printf("qmi_hw->m[0].timing: %x\n", qmi_hw->m[0].timing);
    // printf("clock_speed:  %d\n", clock_speed/1000);


    //*********************
    //***  ADC METRICS  ***
    //*********************
    printf("ADC Scan time: %d  uS   Process time: %d  uS\n", time_to_finish_adc_scan, time_to_finish_adc_process);
    printf("Num ADC Interrupts:  %d,  samples per second: %d\n", adc_interrupt_count, adc_interrupt_count * NUM_ADC_SAMPLES);
    //*****  RESET RUNNING VARIABLES  ***
    core0_main_count = 0;
    audio_interrupt_count = 0;
    adc_interrupt_count = 0;
    core1_main_count = 0;




}


