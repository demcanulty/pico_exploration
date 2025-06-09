

#include "main.h"
#include "hardware/timer.h"
#include "pico/time.h"
#include "Dattorro.hpp"




u32 active_out_buffer;
float audio_in_0[2][BLOCK_SIZE];
float audio_in_1[2][BLOCK_SIZE];

float audio_out_0[2][BLOCK_SIZE];
float audio_out_1[2][BLOCK_SIZE];
s16 audio_out_combined[BLOCK_SIZE * 2];


CommonState* chorus_flange_device;

t_sample scratch_buff[MAX_IN_OUT_CHANNELS][BLOCK_SIZE];
t_sample dummy_buff[MAX_IN_OUT_CHANNELS][BLOCK_SIZE];

//*********************************
const float sizeMax = 4.0f;
float size = 1.f;
float preDelay = 0.f;
float inputDampLow = 0.f;
float inputDampHigh = 10.f;
float reverbDampLow = 0.f;
float reverbDampHigh = 10.f;
float modSpeed = 0.1f;
float modShape = 0.5f;
float modDepth = 0.f;
float diffusion = 1.f;
float decay = 0.f;
int diffuseInput = 1;

static constexpr float minus20dBGain = 0.1f;
static constexpr float minus18dBGain = 0.12589254f;
static constexpr float zerodBGain = 1.f;
float inputSensitivity = minus18dBGain;

//
Dattorro reverb;

void init_gen_device(void)
{

    //chorus_flange_device = (CommonState *) chorus_flange    ::  create(SAMPLE_RATE, BLOCK_SIZE);
    
    
    //chorus_flange::setparameter(chorus_flange_device, 1, map_val, nullptr);

    
    // reverb.setSampleRate(SAMPLE_RATE);

    // reverb.setTimeScale(size);
    // reverb.setPreDelay(clamp(preDelay, 0.f, 1.f));
    // reverb.setInputFilterLowCutoffPitch(inputDampLow);
    // reverb.setInputFilterHighCutoffPitch(inputDampHigh);
    // reverb.enableInputDiffusion(diffuseInput > 0.5f);
    // reverb.setDecay(decay);
    // reverb.setTankDiffusion(diffusion);
    // reverb.setTankFilterLowCutFrequency(reverbDampLow);
    // reverb.setTankFilterHighCutFrequency(reverbDampHigh);
    // reverb.setTankModSpeed(modSpeed);
    // reverb.setTankModDepth(modDepth);
    // reverb.setTankModShape(modShape);
    //Dattorro1997Tank * reverb = new Dattorro1997Tank(48000.0, 16.0, sizeMax);

    //Dattorro reverb.Dattorro(48000.0, 16.0, sizeMax);
}


const float * inputs[] = { audio_in_0[0],  audio_in_1[0],  dummy_buff[0],  dummy_buff[1],    dummy_buff[2] };
      float * outs[] =   { audio_out_0[0], audio_out_1[0], dummy_buff[0], dummy_buff[1], dummy_buff[2], scratch_buff[5], scratch_buff[6], scratch_buff[7] };  


u32 accum_dt, ave_dt, accum_dt_count;
u32 dt;
u32 max_dt;
bool accum_dt_lockout;
u64 t0;
void process_audio(void)
{

    //t0 = timer_hw->timerawl; 
    t0 = time_us_64();

    // for(int i=0; i<BLOCK_SIZE; i++)
    // {
    //     audio_out_0[0][i] = audio_in_0[0][i];
    //     audio_out_1[0][i] = audio_in_0[0][i] / 2.f;  
    // }

    //chorus_flange::perform(chorus_flange_device,(t_sample **)inputs,5,outs,8,BLOCK_SIZE);      
    /*

    for(int i=0; i<BLOCK_SIZE; i++)
    {
        audio_out_0[0][i] = audio_in_0[0][i];
        audio_out_1[0][i] = audio_in_1[0][i];  

        reverb.process(audio_in_0[0][i] * minus20dBGain,  
                       audio_in_1[0][i] * minus20dBGain );
 
        audio_out_0[0][i] = reverb.getLeftOutput();
        audio_out_1[0][i] = reverb.getRightOutput();
    
    }

    */
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



