


#ifndef _CHORUS_FLANGE_HELPER_H_
#define _CHORUS_FLANGE_HELPER_H_ 1

#include "common.h"

#define BLOCK_SIZE     (48 * 1)
#define SAMPLE_RATE     48000
#define CALLBACK_HERTZ  1000


void init_gen_device(void);
extern float audio_in_0[2][BLOCK_SIZE];
extern float audio_in_1[2][BLOCK_SIZE];

extern float audio_out_0[2][BLOCK_SIZE];
extern float audio_out_1[2][BLOCK_SIZE];


extern s16 audio_out_combined[BLOCK_SIZE * 2];

extern u32 active_in_buffer;
extern u32 active_out_buffer;

extern void process_audio(void);

#endif 

