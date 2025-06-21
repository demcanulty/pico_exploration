

#include "main.h"
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "pico/time.h"
#include "math.h"
#include "sound_i2s.h"
#include <stdalign.h> // For alignas and alignof in C11
#include "Tonic.h"
#include "pico/sync.h"

using namespace Tonic;

alignas(32) int16_t buffer_0[I2S_BLOCK_SIZE * 2];
alignas(32) int16_t buffer_1[I2S_BLOCK_SIZE * 2];


bool in_audio_interrupt;


TonicFloat sineBuff[BLOCK_SIZE];
TonicFloat sawBuff[BLOCK_SIZE];
TonicFloat rectBuffBL[BLOCK_SIZE];


Synth           *sineSynth;
Synth           *sawSynth;
Synth           *rectSynthBL;

SineWave        *sineWave;
SawtoothWaveBL  *sawWave;
RectWaveBL      *rectWaveBL; 


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
    Tonic::setSampleRate(SAMPLE_RATE);
    sineSynth = new Synth;	
	sawSynth  = new Synth;
    rectSynthBL = new Synth;

    sawWave  = new 	SawtoothWaveBL();
    sineWave = new 	SineWave();
    rectWaveBL = new RectWaveBL();
	
    sawWave->freq(FixedValue(START_FREQ));
    sineWave->freq(FixedValue(START_FREQ));
    rectWaveBL->freq(FixedValue(START_FREQ)); 
    rectWaveBL->pwm(FixedValue(0.5F));


    sineSynth   ->setOutputGen( *sineWave);
    sawSynth    ->setOutputGen( *sawWave);
    rectSynthBL ->setOutputGen( *rectWaveBL);
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
    in_audio_interrupt = true;
    sineSynth->fillBufferOfFloats(   (float*)sineBuff,     BLOCK_SIZE, 1);
    // sawSynth ->fillBufferOfFloats(   (float*)sawBuff,      BLOCK_SIZE, 1);
    rectSynthBL ->fillBufferOfFloats((float*)rectBuffBL,   BLOCK_SIZE, 1);
    
    in_audio_interrupt = false;

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
        
        //*****  CONVERT FLOAT TO INT16  AND INTERLEAVE  *****
        
        *buff++ = (int16_t)(  sineBuff[i] * -1 * 32767);      //RIGHT OUTPUT BUFFER LOCATION
        //*buff++ = (int16_t)(sawBuff[i]  * 32767);      //LEFT OUTPUT BUFFER LOCATION 
        *buff++ = (int16_t)(rectBuffBL[i]  * 32767);      //LEFT OUTPUT BUFFER LOCATION 
        
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
    uint32_t interrupt_status = save_and_disable_interrupts();
    new_sine_freq = this_freq;
    new_saw_freq  = this_freq;
    new_rect_freq = this_freq;
    params_changed = true;
    restore_interrupts_from_disabled(interrupt_status);
    //printf("\n\nnew freq:  %.3f\n\n", this_freq);
}


void set_pwm(float dutycycle)
{
    static u32 this_time;
    

    //***  REJECT IF HAPPENS TOO SOON  ***
    if(board_millis() - this_time < 20)
    {
        return;
    }

    if((dutycycle > 0.2) && (dutycycle < 0.9))
    {
        u32 diff = board_millis() - this_time;
        this_time = board_millis();
        printf("set_pwm %f   %d\n", dutycycle, diff);
        uint32_t interrupt_status = save_and_disable_interrupts();
        
        new_rect_pwm = dutycycle;
        params_changed = true;
        restore_interrupts_from_disabled(interrupt_status);
    }

    
}



void check_for_param_changes()
{

    //***  CHECK PARAMS CHANGED FLAG  ***
    // if(!params_changed)
    // {
    //     return;     //NOTHING CHANGED, LEAVE FUNCTION 
    // }

    
    if (fabsf(sine_freq - new_sine_freq) >= EPSILON) 
    {
        sine_freq = new_sine_freq;
        sineWave  ->freq(FixedValue(new_sine_freq));
   
    }

    if (fabsf(saw_freq - new_saw_freq) >= EPSILON) 
    {
        saw_freq = new_saw_freq;
        sawWave   ->freq(FixedValue(new_saw_freq));
    
    }

    if (fabsf(rect_freq - new_rect_freq) >= EPSILON) 
    {
        rect_freq = new_rect_freq;
        rectWaveBL->freq(FixedValue(new_rect_freq));
    }

    if (fabsf(rect_pwm - new_rect_pwm) >= EPSILON) 
    {
        rect_pwm = new_rect_pwm;
        rectWaveBL->pwm(FixedValue(new_rect_pwm));
    }

    //***  CLEAR PARAMS CHANGED FLAG  ***
    params_changed = false;

}




//*************************************************************
//*************************************************************
//*************************************************************
//*************************************************************




