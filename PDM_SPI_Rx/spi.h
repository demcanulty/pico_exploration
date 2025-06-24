
#ifndef _SPI_H_
#define _SPI_H_ 




#include <stdint.h>
#include <stdbool.h>
#include "pico/stdlib.h"
#include "bsp/board_api.h"


#ifdef __cplusplus
extern "C" 
{
#endif

typedef uint8_t   u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;




#define EPSILON 1e-6f // For floats


extern float rx_buffer[128];
void core1_main();
void setup_spi_slave();
void setup_dma_rx() ;
void start_dma_receive(uint8_t *dest, size_t num_bytes);
void print_floats(const float *buf, size_t count);




void init_spi();
void spi_dma_transfer( float * this_buff, u32 buff_size_32_bit);

extern float random_data[128];
extern int spi_dma_chan;


#define I2S_BLOCK_SIZE       240                //number of samples per interrupt
#define BLOCK_SIZE         I2S_BLOCK_SIZE
#define TRANSFER_SIZE       (I2S_BLOCK_SIZE * 2 * 2)



extern int16_t buffer_1[TRANSFER_SIZE];
extern int16_t buffer_2[TRANSFER_SIZE];




#ifdef __cplusplus
}
#endif
#endif