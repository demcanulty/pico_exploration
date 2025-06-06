#ifndef _TAPBANDS_HELP_H_
#define _TAPBANDS_HELP_H_

#include "main.h"

void init_tap_band_delay(void);
void update_tap_band_params(void);
extern CommonState* tap_band_quad_pan_ctrl_0;
extern CommonState* tap_band_quad_pan_ctrl_1;
extern CommonState* tap_band_quad_pan_ctrl_2;
extern CommonState* tap_band_quad_pan_ctrl_3;
extern CommonState*  tap_band_delay;


#define NUM_QUAD_CONTROL_MULTIPLIERS  4


extern CommonState* tap_band_quad_panner_0;
extern CommonState* tap_band_quad_panner_1;
extern CommonState* tap_band_quad_panner_2;
extern CommonState* tap_band_quad_panner_3;

extern t_sample tap_quad_control_buff_0[NUM_QUAD_CONTROL_MULTIPLIERS][1];   // NOTE YOU MUST DISABLE THE INCREMENT IN GEN CODE  
extern t_sample tap_quad_control_buff_1[NUM_QUAD_CONTROL_MULTIPLIERS][1];
extern t_sample tap_quad_control_buff_2[NUM_QUAD_CONTROL_MULTIPLIERS][1];
extern t_sample tap_quad_control_buff_3[NUM_QUAD_CONTROL_MULTIPLIERS][1];



#endif 
