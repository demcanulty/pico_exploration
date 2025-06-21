

#include "main.h"
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "pico/time.h"
#include "math.h"
#include "sound_i2s.h"
#include <stdalign.h> // For alignas and alignof in C11
#include "pico/sync.h"
#include "daisysp.h"
using namespace daisysp;

alignas(32) int16_t buffer_0[I2S_BLOCK_SIZE * 2];
alignas(32) int16_t buffer_1[I2S_BLOCK_SIZE * 2];


bool in_audio_interrupt;

Oscillator sine_osc;
Oscillator rect_osc;

#define START_FREQ 80.f

float new_sine_freq, new_saw_freq, new_rect_freq, new_rect_pwm;
float sine_freq, saw_freq, rect_freq, rect_pwm;
bool params_changed;


u32 audio_interrupt_count;

//********************************************
//****  INIT AUDIO DEVICES
//********************************************

void init_audio_code(void)
{
   sine_osc.Init(SAMPLE_RATE);

   // Set parameters for oscillator
    sine_osc.SetWaveform(sine_osc.WAVE_SIN);
    sine_osc.SetFreq(440);
    sine_osc.SetAmp(0.5);


    rect_osc.Init(SAMPLE_RATE);

   // Set parameters for oscillator
    rect_osc.SetWaveform(sine_osc.WAVE_SQUARE);
    rect_osc.SetFreq(440);
    rect_osc.SetAmp(0.5);
}




u32 accum_dt, ave_dt, accum_dt_count;
u32 dt;
u32 max_dt;
bool accum_dt_lockout;
u32 t0;


u8 sine_count;

//********************************************
//****  AUDIO PROCESSING INTERRUPT
//********************************************

void process_audio(void)
{
    //**********************************
    //***  JUST SOME TIMING METRICS  ***
    //**********************************
    t0 = timer_hw->timerawl; 
    audio_interrupt_count++;

    //***************************************************
    //***  CALCULATE SAMPLES FOR TONIC SYNTH BUFFERS  ***
    //***************************************************
    //in_audio_interrupt = true;
    
    
    
    //in_audio_interrupt = false;

    //*****************************************
    //***  CHECK FOR ANY PARAMETER CHANGES  ***
    //*****************************************
    check_for_param_changes();

    //*****************************************************
    //***  CONVERT TO INT16 AND TRANSFER TO I2S BUFFER  ***
    //*****************************************************
    int16_t * buff = sound_i2s_get_next_buffer();
    for(int i=0; i<BLOCK_SIZE; i++)
    {
        float sine_sig = sine_osc.Process();
        float rect_sig = rect_osc.Process();
        //*****  CONVERT FLOAT TO INT16  AND INTERLEAVE  *****
        
        *buff++ = (int16_t)(sine_sig * 32767);      //RIGHT OUTPUT BUFFER LOCATION
        *buff++ = (int16_t)(rect_sig * 32767);      //LEFT OUTPUT BUFFER LOCATION 
        
    }
   
   
    //**********************************
    //***  TIMING METRICS CONTINUED  ***
    //**********************************
    if(!accum_dt_lockout)
    {
        //***  accumulate uS deltas
        dt = (timer_hw->timerawl) - t0;
        accum_dt += dt;
        accum_dt_count++;

        //***  save greatest uS delta
        if(max_dt < dt)
        {
            max_dt = dt;
        }
    }

    gpio_put(DEBUG_A, 0);
}

//*************************************************************
//*************************************************************
//*************************************************************
//*************************************************************


void set_oscillator_frequency(float this_freq)
{
    sine_osc.SetFreq(this_freq);
    rect_osc.SetFreq(this_freq);
}


void set_pwm(float dutycycle)
{
    if((dutycycle > 0.01) && (dutycycle < 0.99))
    {
        rect_osc.SetPw(dutycycle);
    }
    

}



void check_for_param_changes()
{

    //***  CHECK PARAMS CHANGED FLAG  ***
    if(!params_changed)
    {
        return;     //NOTHING CHANGED, LEAVE FUNCTION 
    }

    
    if (fabsf(sine_freq - new_sine_freq) >= EPSILON) 
    {
        sine_freq = new_sine_freq;

    }

    if (fabsf(saw_freq - new_saw_freq) >= EPSILON) 
    {
        saw_freq = new_saw_freq;
    
    }

    if (fabsf(rect_freq - new_rect_freq) >= EPSILON) 
    {
        rect_freq = new_rect_freq;
    }

    if (fabsf(rect_pwm - new_rect_pwm) >= EPSILON) 
    {
        rect_pwm = new_rect_pwm;
    }

    //***  CLEAR PARAMS CHANGED FLAG  ***
    params_changed = false;

}




//*************************************************************
//*************************************************************
//*************************************************************
//*************************************************************




