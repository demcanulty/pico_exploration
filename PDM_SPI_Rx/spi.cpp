

#include "spi.h"
#include "pico/stdlib.h"
#include "hardware/dma.h"
#include "hardware/spi.h"


//SPI configurations
#define PIN_SCK   2
#define PIN_MOSI  3 
#define PIN_MISO  4
#define PIN_CS    5
#define SPI_PORT spi0



alignas(32) int16_t buffer_1[TRANSFER_SIZE];
alignas(32) int16_t buffer_2[TRANSFER_SIZE];



int spi_dma_chan;
dma_channel_config spi_dma_cfg;

#define SPI_SPEED  20000000   


void setup_spi_slave() 
{
    spi_init(SPI_PORT, SPI_SPEED);  // Clock is controlled by master
    spi_set_slave(SPI_PORT, true);
    // Configure SPI pins
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_CS,   GPIO_FUNC_SPI);

    spi_set_format(SPI_PORT, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);  
}




void setup_dma_rx() 
{
    spi_dma_chan = dma_claim_unused_channel(true);
    spi_dma_cfg = dma_channel_get_default_config(spi_dma_chan);

    channel_config_set_transfer_data_size(&spi_dma_cfg, DMA_SIZE_8); // 8-bit transfers
    channel_config_set_read_increment(&spi_dma_cfg, false);          // Always read from FIFO
    channel_config_set_write_increment(&spi_dma_cfg, true);          // Fill buffer sequentially
    channel_config_set_dreq(&spi_dma_cfg, spi_get_dreq(SPI_PORT, false));
}





void start_dma_receive(uint8_t *dest, size_t num_bytes) 
{
    dma_channel_configure(
        spi_dma_chan,
        &spi_dma_cfg,
        dest,
        &spi_get_hw(SPI_PORT)->dr,
        num_bytes,
        true  // start immediately
    );
}



void print_floats(const float *buf, size_t count) 
{
    for (size_t i = 0; i < count; ++i) {
        printf("[%03d] %.6f\n", i, buf[i]);
        //printf("%d\n", i, buf[i]);
    }
}
