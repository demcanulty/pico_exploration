#include "main.h"     

const float * dummy_input;



t_sample tap_quad_control_buff_0[NUM_QUAD_CONTROL_MULTIPLIERS][1];   // NOTE YOU MUST DISABLE THE INCREMENT IN GEN CODE  
t_sample tap_quad_control_buff_1[NUM_QUAD_CONTROL_MULTIPLIERS][1];
t_sample tap_quad_control_buff_2[NUM_QUAD_CONTROL_MULTIPLIERS][1];
t_sample tap_quad_control_buff_3[NUM_QUAD_CONTROL_MULTIPLIERS][1];

float  * tap_quad_ctrl_buff_buff_0[] = { tap_quad_control_buff_0[0], tap_quad_control_buff_0[1], tap_quad_control_buff_0[2], tap_quad_control_buff_0[3]}; 
float  * tap_quad_ctrl_buff_buff_1[] = { tap_quad_control_buff_1[0], tap_quad_control_buff_1[1], tap_quad_control_buff_1[2], tap_quad_control_buff_1[3]}; 
float  * tap_quad_ctrl_buff_buff_2[] = { tap_quad_control_buff_2[0], tap_quad_control_buff_2[1], tap_quad_control_buff_2[2], tap_quad_control_buff_2[3]}; 
float  * tap_quad_ctrl_buff_buff_3[] = { tap_quad_control_buff_3[0], tap_quad_control_buff_3[1], tap_quad_control_buff_3[2], tap_quad_control_buff_3[3]}; 



CommonState* tap_band_quad_panner_0;
CommonState* tap_band_quad_panner_1;
CommonState* tap_band_quad_panner_2;
CommonState* tap_band_quad_panner_3;

CommonState* tap_band_quad_pan_ctrl_0;
CommonState* tap_band_quad_pan_ctrl_1;
CommonState* tap_band_quad_pan_ctrl_2;
CommonState* tap_band_quad_pan_ctrl_3;

CommonState*  tap_band_delay;


void init_tap_band_delay(void)
{

    //*********************************
    //******  TAP BAND PANNERS  
    //*********************************
    tap_band_quad_panner_0 = (CommonState *) QuadPan_dsp    ::  create(SAMPLE_RATE, BLOCK_SIZE);
    tap_band_quad_panner_1 = (CommonState *) QuadPan_dsp    ::  create(SAMPLE_RATE, BLOCK_SIZE);
    tap_band_quad_panner_2 = (CommonState *) QuadPan_dsp    ::  create(SAMPLE_RATE, BLOCK_SIZE);
    tap_band_quad_panner_3 = (CommonState *) QuadPan_dsp    ::  create(SAMPLE_RATE, BLOCK_SIZE);

    //*********************************
    //******  TAP BAND PAN CONTROLS  
    //*********************************

    tap_band_quad_pan_ctrl_0  = (CommonState *) QuadPan_ctl_min ::  create(SAMPLE_RATE,          1);  //trying a block size of 1
    tap_band_quad_pan_ctrl_1  = (CommonState *) QuadPan_ctl_min ::  create(SAMPLE_RATE,          1);
    tap_band_quad_pan_ctrl_2  = (CommonState *) QuadPan_ctl_min ::  create(SAMPLE_RATE,          1);
    tap_band_quad_pan_ctrl_3  = (CommonState *) QuadPan_ctl_min ::  create(SAMPLE_RATE,          1);

    
    #define DEFAULT_ATTENUATION  1.0f
    //***  SET DEFAULTS  ***
    QuadPan_ctl_min::setparameter(tap_band_quad_pan_ctrl_0, QUAD_PAN_ATTENUATE, DEFAULT_ATTENUATION, nullptr);
    QuadPan_ctl_min::setparameter(tap_band_quad_pan_ctrl_1, QUAD_PAN_ATTENUATE, DEFAULT_ATTENUATION, nullptr);
    QuadPan_ctl_min::setparameter(tap_band_quad_pan_ctrl_2, QUAD_PAN_ATTENUATE, DEFAULT_ATTENUATION, nullptr);
    QuadPan_ctl_min::setparameter(tap_band_quad_pan_ctrl_3, QUAD_PAN_ATTENUATE, DEFAULT_ATTENUATION, nullptr);


    QuadPan_ctl_min::setparameter(tap_band_quad_pan_ctrl_0, QUAD_PAN_X_LOC, 0.7f, nullptr);
    QuadPan_ctl_min::setparameter(tap_band_quad_pan_ctrl_1, QUAD_PAN_X_LOC, 0.7f, nullptr);
    QuadPan_ctl_min::setparameter(tap_band_quad_pan_ctrl_2, QUAD_PAN_X_LOC, -0.7f, nullptr);
    QuadPan_ctl_min::setparameter(tap_band_quad_pan_ctrl_3, QUAD_PAN_X_LOC, -0.7f, nullptr);


    QuadPan_ctl_min::setparameter(tap_band_quad_pan_ctrl_0, QUAD_PAN_Y_LOC, 0.7f, nullptr);
    QuadPan_ctl_min::setparameter(tap_band_quad_pan_ctrl_1, QUAD_PAN_Y_LOC, -0.7f, nullptr);
    QuadPan_ctl_min::setparameter(tap_band_quad_pan_ctrl_2, QUAD_PAN_Y_LOC, 0.7f, nullptr);
    QuadPan_ctl_min::setparameter(tap_band_quad_pan_ctrl_3, QUAD_PAN_Y_LOC, -0.7f, nullptr);


    //***  CALCULATE FIRST VALUES FOR GLOBAL PANNERS  *****
    QuadPan_ctl_min :: perform(tap_band_quad_pan_ctrl_0, (t_sample **) dummy_input,1,tap_quad_ctrl_buff_buff_0,4,1);      
    QuadPan_ctl_min :: perform(tap_band_quad_pan_ctrl_1, (t_sample **) dummy_input,1,tap_quad_ctrl_buff_buff_1,4,1);     
    QuadPan_ctl_min :: perform(tap_band_quad_pan_ctrl_2, (t_sample **) dummy_input,1,tap_quad_ctrl_buff_buff_2,4,1);     
    QuadPan_ctl_min :: perform(tap_band_quad_pan_ctrl_3, (t_sample **) dummy_input,1,tap_quad_ctrl_buff_buff_3,4,1);     



    //*********************************
    //******  TAP BAND DELAY
    //*********************************

    tap_band_delay = (CommonState *) tapBandDelays    ::  create(SAMPLE_RATE, BLOCK_SIZE);



    tapBandDelays::setparameter(tap_band_delay, TAP_BAND_DTIME_MS,  1000, nullptr);
    tapBandDelays::setparameter(tap_band_delay, TAP_BAND_FEEDBACK,  0.5f, nullptr);
    tapBandDelays::setparameter(tap_band_delay, TAP_BAND_FREQ,      20.f, nullptr);
    tapBandDelays::setparameter(tap_band_delay, TAP_BAND_Q,         0.5, nullptr);
}




void update_tap_band_params(void)
{
    QuadPan_ctl_min :: perform(tap_band_quad_pan_ctrl_0, (t_sample **) dummy_input,1,tap_quad_ctrl_buff_buff_0,4,1);      
    QuadPan_ctl_min :: perform(tap_band_quad_pan_ctrl_1, (t_sample **) dummy_input,1,tap_quad_ctrl_buff_buff_1,4,1);     
    QuadPan_ctl_min :: perform(tap_band_quad_pan_ctrl_2, (t_sample **) dummy_input,1,tap_quad_ctrl_buff_buff_2,4,1);     
    QuadPan_ctl_min :: perform(tap_band_quad_pan_ctrl_3, (t_sample **) dummy_input,1,tap_quad_ctrl_buff_buff_3,4,1);      
}

