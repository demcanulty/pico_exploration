
#ifndef _ADC_H_
#define _ADC_H_

#ifdef __cplusplus
extern "C" 
{
#endif

#include "main.h"

#define NUM_ADC_PINS 3

void init_project_adc();
void adc_collect();
void check_adc_vals();



extern u32 time_to_finish_adc_scan;
uint16_t fir_filter_channel(int channel, float new_sample);
#ifdef __cplusplus
}
#endif
#endif