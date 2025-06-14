

#include "main.h"
#include "hardware/timer.h"
#include "pico/time.h"
#include "math.h"
#include "sound_i2s.h"
#include "Tonic.h"
using namespace Tonic;


u32 active_out_buffer;
float audio_in_0[BLOCK_SIZE];
float audio_in_1[BLOCK_SIZE];

float audio_out_0[BLOCK_SIZE];
float audio_out_1[BLOCK_SIZE];
s16 audio_out_combined[BLOCK_SIZE * 2];


#define AUDIO_BUFFER_SIZE  BLOCK_SIZE
#define START_FREQ           10.f

int16_t buffer_0[I2S_BLOCK_SIZE * 2];
int16_t buffer_1[I2S_BLOCK_SIZE * 2];


TonicFloat waveBuff[I2S_BLOCK_SIZE];
TonicFloat noiseBuff[I2S_BLOCK_SIZE];
TonicFloat sawBuff[I2S_BLOCK_SIZE];
TonicFloat rectBuffBL[I2S_BLOCK_SIZE];
TonicFloat rectBuffNoBL[I2S_BLOCK_SIZE];

uint32_t outputBuff1[I2S_BLOCK_SIZE];
uint32_t outputBuff2[I2S_BLOCK_SIZE];


Synth           *sineSynth;
Synth           *sawSynth;
SineWave        *sineWave;
SawtoothWaveBL  *sawWave;




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


u32 audio_interrupt_count;
u32 accum_dt, ave_dt, accum_dt_count;
u32 dt;
u32 max_dt;
bool accum_dt_lockout;
u32 t0;

u8 sin_count;
void process_audio(void)
{

    t0 = timer_hw->timerawl; 

    audio_interrupt_count++;

    //int16_t * buff =  sound_i2s_get_next_buffer();
    
    // sineSynth->fillBufferOfFloats((float*)waveBuff, BLOCK_SIZE, 1);
    // sawSynth ->fillBufferOfFloats((float*)sawBuff,  BLOCK_SIZE, 1);

    for(int i=0; i<BLOCK_SIZE; i++)
    {
        audio_out_0[i] = audio_in_0[i];
        audio_out_1[i] = audio_in_1[i];

        


        //  audio_out_0[i] = waveBuff[i];
        //  audio_out_1[i] = sawBuff[i];
    }


    /*
    for(int i=0; i<I2S_BLOCK_SIZE; i++)
    {

        float left = (sin(2.0f * ((float) M_PI) * sin_count++ / 256.0f) * 32767);
        //float right = 1 - left;

        //float left = (sawBuff [i] * 32768);

        // *buff++ = (int16_t)(waveBuff[i] * 32768);
        // *buff++ = (int16_t)(sawBuff [i] * 32768);;
        //*********************************************
        //***  RE-INTERLEAVE AND CONVERT TO int16_t ***
        //*********************************************
        // *buff++ = left;
        // *buff++ = left;

        *buff++ = (int16_t) left;
        *buff++ = (int16_t) left;
    }*/



        //***  TICK BASED MICROSECOND COUNTER  

    if(!accum_dt_lockout)
    {
        //***  accumulate tick deltas
        dt = (timer_hw->timerawl) - t0;
        accum_dt += dt;
        accum_dt_count++;

        //***  save greatest tick delta
        if(max_dt < dt)
        {
            max_dt = dt;
        }
    }
        
}



