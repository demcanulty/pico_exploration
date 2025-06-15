#include <stdio.h>
#include "pico/stdlib.h"


#include "tusb.h"             
#include "tusb_config.h"  
#include "bsp/board_api.h"
#include "pico/multicore.h"
#include "hardware/clocks.h"
#include "hardware/vreg.h"
#include "main.h"
#include "midi/midi.h"
#include "sound_i2s/sound_i2s.h"
#include "Tonic.h"

#include <hardware/structs/qmi.h>


//#define OVERCLOCK_300MHZ  
#define OVERCLOCK_400MHZ      //See RP2350 datasheet, QMI: M0_TIMING, M1_TIMING Registers, CLKDIV bits


uint32_t blink_interval_ms = BLINK_NOT_MOUNTED;

//*************************************
//**********   I2S STUFF  *************
//*************************************
#define I2S_DATA_PIN             28 // -> I2S DIN
#define I2S_CLOCK_PIN_BASE       26 // -> I2S BCK
// The third required connection is GPIO 27 -> I2S LRCK (BCK+1)

static const struct sound_i2s_config sound_config = 
{
    .pio_num         = 0, // 0 for pio0, 1 for pio1
    .pin_scl         = I2S_CLOCK_PIN_BASE,
    .pin_sda         = I2S_DATA_PIN,
    .pin_ws          = I2S_CLOCK_PIN_BASE + 1,
    .sample_rate     = SAMPLE_RATE,
    .bits_per_sample = 16,
};





//**************************************************************
//**************************************************************
//**********   SECOND MAIN LOOP FOR SECOND CORE  ***************
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
        //***  Do nothing every second   ***
        //**********************************
        if(board_millis() - core1_this_time > 999)
        {
            core1_this_time = board_millis();

        }

        core1_this_count++;

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
    //********************
    //***  OVERCLOCK  ***
    //********************
    #ifdef OVERCLOCK_300MHZ
    vreg_set_voltage(VREG_VOLTAGE_1_20);    //300Mhz was locking up at 1.10v, bumping to 1.20v
    set_sys_clock_khz(300000, true);
    #endif

    #ifdef OVERCLOCK_400MHZ
    //***  REDUCE FLASH TIMING CLOCK  ***
    qmi_hw->m[0].timing |= 0x4;             //qmi_hw->m[0].timing now equals 0x60007207  (raise third bit, qmi clkdiv is now 3)
    qmi_hw->m[0].timing &= ~(0x3);          //qmi_hw->m[0].timing now equals 0x60007204  (drop first and second bits, qmi clkdiv is now 4) 
    //qmi_hw->m[0].timing &= ~(0x2);          //qmi_hw->m[0].timing now equals 0x60007205  (drop the second bit, qmi clkdiv is now 5) 
    vreg_set_voltage(VREG_VOLTAGE_1_30);    //400 Mhz may be highest achievable at 1.30v. 
    set_sys_clock_khz(380000, true);        //400 Mhz may be too unstable, trying something lower 
    #endif

    board_init();
    init_pins();

    init_audio_code();


    //***********************`
    //***  TINY USB INIT  ***
    //***********************
   
    tusb_rhport_init_t dev_init = {     // init device stack on configured roothub port
        .role = TUSB_ROLE_DEVICE,
        .speed = TUSB_SPEED_AUTO
    };
    tusb_init(BOARD_TUD_RHPORT, &dev_init);


    //************************
    //***  USB UART INIT  ****
    //************************

    stdio_init_all();

    //******************************************
    //****  I2S AUDIO OUT  *********************
    //******************************************
    sound_i2s_init(&sound_config);
    sound_i2s_playback_start();
    


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

            
            printf("\nCore 0 - Runs through main: %d\n", this_count);
            printf("Core 1 - Runs through main: %d\n", core1_this_count);
            //printf("Time (in millis)          : %d\n\n", this_time);
            printf("Audio Interrupts per sec: %d\n", audio_interrupt_count);
            printf("ave_dt: %d    max_dt: %d\n", (int)ave_dt, (int)max_dt);
            printf("qmi_hw->m[0].timing: %x\n", qmi_hw->m[0].timing);
            this_count = 0;
            audio_interrupt_count = 0;
            core1_this_count = 0;

            //**************************************
            //***  CALCULATE AUDIO PERFORMANCE  ****
            //**************************************

            accum_dt_lockout = true;

            dt_in_us     = (float) accum_dt / accum_dt_count;
            max_dt_in_us = (float)   max_dt;
            ave_dt = accum_dt / accum_dt_count;

            //*************************
            //***  RESET VARIABLES  ***
            //*************************
            accum_dt = 0;
            accum_dt_count = 0;
            max_dt = 0;
            accum_dt_lockout = false;

            
            
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
        
        this_count++;

    }
}







//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************


// Perform initialisation
void init_pins(void) 

{

    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_init(DEBUG_A);
    gpio_init(DEBUG_B);
    gpio_init(DEBUG_C);

    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    gpio_set_dir(DEBUG_A, GPIO_OUT);
    gpio_set_dir(DEBUG_B, GPIO_OUT);
    gpio_set_dir(DEBUG_C, GPIO_OUT);




}





// Turn the led on or off
void pico_set_led(bool led_on) 
{
    // Just set the GPIO on or off
    gpio_put(PICO_DEFAULT_LED_PIN, led_on);

}