

#include "main.h"
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "pico/time.h"
#include "math.h"
#include "sound_i2s.h"
#include <stdalign.h> // For alignas and alignof in C11
#include "Tonic.h"
using namespace Tonic;

u32 active_out_buffer;
float audio_in_0[BLOCK_SIZE];
float audio_in_1[BLOCK_SIZE];

float audio_out_0[BLOCK_SIZE];
float audio_out_1[BLOCK_SIZE];
s16 audio_out_combined[BLOCK_SIZE * 2];

alignas(32) int16_t buffer_0[I2S_BLOCK_SIZE * 2];
alignas(32) int16_t buffer_1[I2S_BLOCK_SIZE * 2];

// float scratch_buff[MAX_IN_OUT_CHANNELS][BLOCK_SIZE];
// float dummy_buff[MAX_IN_OUT_CHANNELS][BLOCK_SIZE];





TonicFloat sineBuff[BLOCK_SIZE];
TonicFloat noiseBuff[BLOCK_SIZE];
TonicFloat sawBuff[BLOCK_SIZE];
TonicFloat rectBuffBL[BLOCK_SIZE];
TonicFloat rectBuffNoBL[BLOCK_SIZE];

uint32_t outputBuff1[BLOCK_SIZE];
uint32_t outputBuff2[BLOCK_SIZE];


Synth           *sineSynth;
Synth           *sawSynth;
SineWave        *sineWave;
SawtoothWaveBL  *sawWave;

#define START_FREQ 80.f



u32 audio_interrupt_count;


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

u8 sin_count;

int16_t out_count;
void process_audio(void)
{

    t0 = timer_hw->timerawl; 

    audio_interrupt_count++;

    sineSynth->fillBufferOfFloats((float*)sineBuff,  BLOCK_SIZE, 1);
    sawSynth ->fillBufferOfFloats((float*)sawBuff,   BLOCK_SIZE, 1);


    int16_t * buff = sound_i2s_get_next_buffer();
    for(int i=0; i<BLOCK_SIZE; i++)
    {

        // float left = (sin(2.0f * ((float) M_PI) * sin_count++ / 256.0f));
        // float right;// = 1 - left;


        //*****  CONVERT FLOAT TO INT16  *****
        *buff++ = (int16_t)(sineBuff[i] * 32767);      //
        *buff++ = (int16_t)(sawBuff[i]  * 32767);
        
        //*buff++ = (int16_t)(left * 32767);
        // *buff++ = (int16_t)(right * -32767);   
        //*buff++ = 0;
        //*buff++ = 0x7fff ;
    }
    out_count++;


        //***  MICROSECOND COUNTER  ***

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



