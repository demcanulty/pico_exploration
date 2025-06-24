
#ifndef _SPI_H_
#define _SPI_H_ 



#ifdef __cplusplus
extern "C" 
{
#endif


void init_spi();
void spi_dma_transfer( int16_t * this_buff, u32 buff_size_32_bit);

extern float random_data[128];




#ifdef __cplusplus
}
#endif
#endif