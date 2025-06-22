
#ifndef _ADC_H_
#define _ADC_H_

#ifdef __cplusplus
extern "C" 
{
#endif

//#include "main.h"

#define NUM_ADC_PINS 3

void init_project_adc();
void adc_collect();
void check_adc_vals();
void dma_handler();
extern bool adc_dma_finished;
void flash_dma_handler();
#ifdef __cplusplus
}
#endif
#endif