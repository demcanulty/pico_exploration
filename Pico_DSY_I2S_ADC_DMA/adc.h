
#ifndef _ADC_H_
#define _ADC_H_

#ifdef __cplusplus
extern "C" 
{
#endif

#include "main.h"

#define NUM_ADC_PINS 3
#define NUM_ADC_SAMPLES (1024  * 2)
void init_project_adc();
void adc_collect();
void check_adc_vals();
extern u8 adc_buff_index_to_process;
extern u8 adc_pin_to_process; 
extern uint16_t  __attribute__((aligned(4))) adc_buffer[2][NUM_ADC_SAMPLES];

extern u32 time_to_finish_adc_scan, adc_interrupt_count, time_to_finish_adc_process;
uint16_t fir_filter_channel(int channel, float new_sample);


void process_adc();


#ifdef __cplusplus
}
#endif
#endif