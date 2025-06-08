


#ifndef _AUDIO_PROCESS_H_
#define _AUDIO_PROCESS_H_ 1

#include "common.h"

#define BLOCK_SIZE     (48 * 1)
#define SAMPLE_RATE     48000
#define CALLBACK_HERTZ  1000
#define MAX_IN_OUT_CHANNELS  8


extern u32 accum_dt, ave_dt, accum_dt_count;
extern u32 dt;
extern u32 max_dt;
extern bool accum_dt_lockout;



void init_gen_device(void);
extern float audio_in_0[BLOCK_SIZE];
extern float audio_in_1[BLOCK_SIZE];

extern float audio_out_0[BLOCK_SIZE];
extern float audio_out_1[BLOCK_SIZE];


extern s16 audio_out_combined[BLOCK_SIZE * 2];

extern u32 active_in_buffer;
extern u32 active_out_buffer;

extern void process_audio(void);

#endif 

