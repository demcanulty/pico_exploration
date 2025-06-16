

#include "main.h"
#include "hardware/timer.h"
#include "pico/time.h"
#include "math.h"
#include "sound_i2s.h"

#include "Tonic.h"
using namespace Tonic;


bool in_audio_interrupt;

u32 active_out_buffer;
float audio_in_0[BLOCK_SIZE];
float audio_in_1[BLOCK_SIZE];

float audio_out_0[BLOCK_SIZE];
float audio_out_1[BLOCK_SIZE];
s16 audio_out_combined[BLOCK_SIZE * 2];



// float scratch_buff[MAX_IN_OUT_CHANNELS][BLOCK_SIZE];
// float dummy_buff[MAX_IN_OUT_CHANNELS][BLOCK_SIZE];





TonicFloat waveBuff[BLOCK_SIZE];
TonicFloat noiseBuff[BLOCK_SIZE];
TonicFloat sawBuff[BLOCK_SIZE];
TonicFloat rectBuffBL[BLOCK_SIZE];

uint32_t outputBuff1[BLOCK_SIZE];
uint32_t outputBuff2[BLOCK_SIZE];


Synth           *sineSynth;
Synth           *sawSynth;
SineWave        *sineWave;
SawtoothWaveBL  *sawWave;
Synth           *rectSynthBL;
RectWaveBL      *rectWaveBL; 

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

u8 sin_count;


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

    sineSynth->fillBufferOfFloats(   (float*)audio_out_0,     BLOCK_SIZE, 1);
    // sawSynth ->fillBufferOfFloats(   (float*)audio_out_1,      BLOCK_SIZE, 1);
    rectSynthBL ->fillBufferOfFloats((float*)audio_out_1,   BLOCK_SIZE, 1);
    
    in_audio_interrupt = false;

      //***  TICK BASED MICROSECOND COUNTER  


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

        
}




//*************************************************************
//*************************************************************
//*************************************************************
//*************************************************************


void set_oscillator_frequency(float this_freq)
{
    while(in_audio_interrupt)
    {
        tight_loop_contents();
    }
    sineWave  ->freq(FixedValue(this_freq));
    sawWave   ->freq(FixedValue(this_freq));
    rectWaveBL->freq(FixedValue(this_freq));
    //printf("\n\nnew freq:  %.3f\n\n", this_freq);
}


//*************************************************************
//*************************************************************
//*************************************************************
//*************************************************************



