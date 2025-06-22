
#ifndef _ADC_H_
#define _ADC_H_

#ifdef __cplusplus
extern "C" 
{
#endif

#include "main.h"

#define NUM_ADC_PINS 3
#define NUM_SAMPLES 1024   
void init_project_adc();
void adc_collect();
void check_adc_vals();
extern u8 adc_buff_index_to_process;


extern u32 time_to_finish_adc_scan, adc_interrupt_count;
uint16_t fir_filter_channel(int channel, float new_sample);
#ifdef __cplusplus
}
#endif
#endif