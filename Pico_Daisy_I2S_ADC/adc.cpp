#include "adc.h"
#include "bsp/board_api.h"
#include "hardware/adc.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "main.h"
#include "pico/multicore.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include "math.h"
#include "audio_process.h"

#define NUM_SAMPLES 700   // 700 samples takes about 9ms here


u32 time_to_finish_adc_scan;





void init_project_adc()
{
    adc_init();
    adc_gpio_init(26);
    adc_gpio_init(27);
    adc_gpio_init(28);

    adc_select_input(0);


    //*** it would be a good idea to initialize the fir filter buffers here


}

//*****************************************
//*****************************************
//******  COLLECT ADC DATA
//*****************************************
//*****************************************


uint16_t adc_val[NUM_ADC_CHANNELS];        
uint16_t last_adc_val[NUM_ADC_CHANNELS];
u32 adc_accum;
u32 adc_accum_count;


void adc_collect()
{
    static u8 this_adc;   //initialized to 0 by compiler
    static u32 this_time;

    //***  TAKE SAMPLE  
    adc_accum+= adc_read();

    //***  INCREMENT COUNTER  
    adc_accum_count++;

    if(adc_accum_count >= NUM_SAMPLES)
    {
        //***  DECIMATE ACCUMULATOR  
        u16 val = adc_accum / adc_accum_count;

        //***  ADDITIONAL FILTERING  ***
        adc_val[this_adc]  = fir_filter_channel(this_adc, val);

        //***  OR NO FILTERING  
        //adc_val[this_adc]  = val;

        //***  CLEAR ACCUMULATOR VARIABLES 
        adc_accum = 0;
        adc_accum_count = 0;

        //***  INCREMENT ADC 
        this_adc++;
        if(this_adc >= NUM_ADC_CHANNELS)
        {
            this_adc = 0;

            time_to_finish_adc_scan = board_millis() - this_time;
            this_time = board_millis();
        }

        //***  MUX TO CORRECT PIN  
        adc_select_input(this_adc);

    }
}


//*****************************************
//*****************************************
//******  LOOK FOR CHANGES
//*****************************************
//*****************************************

void check_adc_vals()
{
    bool adc_val_changed = false;
    for(int i=0; i<NUM_ADC_PINS; i++)
    {
        if(abs(adc_val[i] - last_adc_val[i]) > 0)   
        {
        
            last_adc_val[i] = adc_val[i];
            adc_val_changed = true;

            control_val_changed(i, adc_val[i]);
        }
    }

    if(adc_val_changed)
    {
        printf("adc0: %5u  adc1: %5u  adc3: %5u \n", adc_val[0], adc_val[1], adc_val[2]);
    }

}

//*****************************************
//*****************************************
//******  FILTERING   *********************
//*****************************************
//*****************************************

#define FIR_TAP_NUM 11 
// float fir_coeffs[FIR_TAP_NUM] = 
// {
//     0.045, 0.075, 0.105, 0.125, 0.140,
//     0.150, 0.140, 0.125, 0.105, 0.075, 0.045
// };

float fir_coeffs[FIR_TAP_NUM] = {
    0.045 / 1.13,
    0.075 / 1.13,
    0.105 / 1.13,
    0.125 / 1.13,
    0.140 / 1.13,   //normalized
    0.150 / 1.13,
    0.140 / 1.13,
    0.125 / 1.13,
    0.105 / 1.13,
    0.075 / 1.13,
    0.045 / 1.13
};

// #define FIR_TAP_NUM 7 
// float fir_coeffs[7] = {
//     //0.2, 0.2, 0.2, 0.2, 0.2
//      0.1, 0.2, 0.2, 0.2, 0.1, 0.1, 0.1
// }; 

// Per-channel filter buffers
float fir_buffers[NUM_ADC_CHANNELS][FIR_TAP_NUM];

// Apply FIR filter to a single channel input
uint16_t fir_filter_channel(int channel, float new_sample) 
{
    //***  check channel bounds
    if(channel >= NUM_ADC_CHANNELS)
    {
        return 0;
    }

    // Shift buffer
    for (int i = FIR_TAP_NUM - 1; i > 0; i--) 
    {
        fir_buffers[channel][i] = fir_buffers[channel][i - 1];
    }
    fir_buffers[channel][0] = new_sample;

    // Apply filter
    float result = 0.0f;
    for (int i = 0; i < FIR_TAP_NUM; i++) {
        result += fir_coeffs[i] * fir_buffers[channel][i];
    }
    return (uint16_t) result;
}
