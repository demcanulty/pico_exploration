

#include "main.h"
#include "hardware/timer.h"
#include "pico/time.h"
#include "gverb.h"




u32 active_out_buffer;
float audio_in_0[BLOCK_SIZE];
float audio_in_1[BLOCK_SIZE];

float audio_out_0[BLOCK_SIZE];
float audio_out_1[BLOCK_SIZE];
s16 audio_out_combined[BLOCK_SIZE * 2];



float scratch_buff[MAX_IN_OUT_CHANNELS][BLOCK_SIZE];
float dummy_buff[MAX_IN_OUT_CHANNELS][BLOCK_SIZE];



/** Tanh signal distortion using piecewise approximation of a hyperbolic tangent */
float tanhDriveSignal(float x) //, float drive) {
{
    //x *= drive;

    if(x < -1.25f) {
        return -1.f;
    }
    else if(x < -0.75f) {
        return 1.f - (x * (-2.5f - x) - 0.5625f) - 1.f;
    }
    else if(x > 1.25f) {
        return 1.f;
    }
    else if(x > 0.75f) {
        return x * (2.5f - x) - 0.5625f;
    }

    return x;
}




ty_gverb        *verb;



float maxroomsize = 41.f;
float roomsize = 40.0f;
float revtime = 7.0f;
float damping = 0.5f; 
float spread  = 0.5f; 
float inputbandwidth  = 0.5f; 
float earlylevel  = 0.5f; 
float taillevel  = 0.5f; 

void init_audio_code(void)
{

    verb = gverb_new(SAMPLE_RATE, maxroomsize , roomsize, revtime, 
                     damping, spread, inputbandwidth , earlylevel, taillevel);


}


// const float * inputs[] = { audio_in_0[0],  audio_in_1[0],  dummy_buff[0],  dummy_buff[1],    dummy_buff[2] };
//       float * outs[] =   { audio_out_0[0], audio_out_1[0], dummy_buff[0], dummy_buff[1], dummy_buff[2], scratch_buff[5], scratch_buff[6], scratch_buff[7] };  


u32 accum_dt, ave_dt, accum_dt_count;
u32 dt;
u32 max_dt;
bool accum_dt_lockout;
u32 t0;
void process_audio(void)
{

    t0 = timer_hw->timerawl; 


    for(int i=0; i<BLOCK_SIZE; i++)
    {
        // audio_out_0[i] = audio_in_0[i];
        // audio_out_1[i] = audio_in_1[i];

        float left, right;
        gverb_do(verb, audio_in_0[i], &left, &right);


         audio_out_0[i] = tanhDriveSignal(left);
         audio_out_1[i] = tanhDriveSignal(right);
    }



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



