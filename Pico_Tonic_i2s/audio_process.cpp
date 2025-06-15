

#include "main.h"
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "pico/time.h"
#include "math.h"
#include "sound_i2s.h"
#include <stdalign.h> // For alignas and alignof in C11
#include "Tonic.h"
using namespace Tonic;


alignas(32) int16_t buffer_0[I2S_BLOCK_SIZE * 2];
alignas(32) int16_t buffer_1[I2S_BLOCK_SIZE * 2];





TonicFloat sineBuff[BLOCK_SIZE];
TonicFloat sawBuff[BLOCK_SIZE];



Synth           *sineSynth;
Synth           *sawSynth;
SineWave        *sineWave;
SawtoothWaveBL  *sawWave;

#define START_FREQ 80.f



u32 audio_interrupt_count;

//********************************************
//****  INIT AUDIO DEVICES
//********************************************

void init_audio_code(void)
{
    Tonic::setSampleRate(SAMPLE_RATE);
    sineSynth = new Synth;	
	sawSynth  = new Synth;

    sawWave  = new 	SawtoothWaveBL();
    sineWave = new 	SineWave();

    sawWave->freq(FixedValue(START_FREQ));
	sineWave->freq(FixedValue(START_FREQ));


    sineSynth->setOutputGen(*sineWave);
	sawSynth ->setOutputGen( *sawWave);

}




u32 accum_dt, ave_dt, accum_dt_count;
u32 dt;
u32 max_dt;
bool accum_dt_lockout;
u32 t0;




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
    sineSynth->fillBufferOfFloats((float*)sineBuff,  BLOCK_SIZE, 1);
    sawSynth ->fillBufferOfFloats((float*)sawBuff,   BLOCK_SIZE, 1);


    //*****************************************************
    //***  CONVERT TO INT16 AND TRANSFER TO I2S BUFFER  ***
    //*****************************************************
    int16_t * buff = sound_i2s_get_next_buffer();
    for(int i=0; i<BLOCK_SIZE; i++)
    {

        //*****  CONVERT FLOAT TO INT16  AND INTERLEAVE  *****
        *buff++ = (int16_t)(sineBuff[i] * 32767);      //RIGHT OUTPUT BUFFER LOCATION
        *buff++ = (int16_t)(sawBuff[i]  * 32767);      //LEFT OUTPUT BUFFER LOCATION 
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

    //gpio_put(DEBUG_A, 0);
}



