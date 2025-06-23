
#ifndef _ADC_H_
#define _ADC_H_

#ifdef __cplusplus
extern "C" 
{
#endif

#include "stdint.h"

#define NUM_ADC_SAMPLES 1024

typedef uint8_t   u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t  s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;




#define NUM_ADC_PINS 3

void init_project_adc();
void adc_collect();
void check_adc_vals();
void process_adc();
// void dma_handler();
extern bool adc_dma_finished;
void flash_dma_handler();

extern u32 adc_interrupt_count;
#ifdef __cplusplus
}
#endif
#endif