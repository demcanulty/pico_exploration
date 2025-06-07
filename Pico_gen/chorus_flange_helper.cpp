

#include "main.h"



u32 active_out_buffer;
float audio_in_0[2][BLOCK_SIZE];
float audio_in_1[2][BLOCK_SIZE];

float audio_out_0[2][BLOCK_SIZE];
float audio_out_1[2][BLOCK_SIZE];
s16 audio_out_combined[BLOCK_SIZE * 2];


CommonState* chorus_flange_device;



void init_gen_device(void)
{

    chorus_flange_device = (CommonState *) chorus_flange    ::  create(SAMPLE_RATE, BLOCK_SIZE);

}


void process_audio(void)
{
    for(int i=0; i<BLOCK_SIZE; i++)
    {

        audio_out_0[0][i] = audio_in_0[0][i];
        audio_out_1[0][i] = audio_in_0[0][i] / 2.f;  

    }
        
}



