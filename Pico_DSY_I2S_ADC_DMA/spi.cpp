
#include "main.h"
#include "spi.h"
#include "pico/stdlib.h"
#include "hardware/dma.h"
#include "hardware/spi.h"

//SPI configurations
#define PIN_MISO 2
#define PIN_CS   3
#define PIN_SCK  4
#define PIN_MOSI 5
#define SPI_PORT spi0


float random_data[128] = {25, 212, 147, 197, 24, 55, 15, 90, 172, 149, 12, 222, 244, 50, 75, 79, 71, 241, 58, 153, 209, 24, 8, 34, 204, 46, 161, 66, 141, 50, 67, 222, 28, 93, 151, 190, 77, 234, 198, 23, 18, 225, 166, 126, 82, 76, 183, 103, 115, 126, 126, 187, 200, 224, 136, 23, 116, 146, 161, 77, 242, 198, 183, 33, 67, 56, 103, 180, 29, 222, 175, 24, 226, 134, 71, 244, 20, 213, 102, 182, 105, 35, 78, 189, 249, 152, 125, 74, 192, 125, 73, 211, 80, 250, 181, 196, 253, 193, 28, 210, 118, 127, 109, 43, 109, 234, 120, 178, 81, 184, 144, 22, 175, 174, 195, 194, 49, 238, 55, 66, 120, 210, 65, 6, 211, 34, 108, 193};

int spi_dma_chan;
dma_channel_config spi_dma_cfg;
//#define SPI_SPEED  20000000
#define SPI_SPEED  200000


void init_spi()
{
    // Initialize SPI channel (channel, baud rate set to 20MHz)
    spi_init(SPI_PORT, SPI_SPEED);


    // Format SPI channel (channel, data bits per transfer, polarity, phase, order)
    spi_set_format(SPI_PORT, 32, SPI_CPOL_0, SPI_CPHA_0, SPI_LSB_FIRST);

    // Map SPI signals to GPIO ports, acts like framed SPI with this CS mapping
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_CS,   GPIO_FUNC_SPI) ;
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);


    //**************************
    //*** Set up DMA channel ***
    //**************************
    spi_dma_chan = dma_claim_unused_channel(true);
    spi_dma_cfg  = dma_channel_get_default_config(spi_dma_chan);
    channel_config_set_transfer_data_size (&spi_dma_cfg, DMA_SIZE_32);      // 16-bit
    channel_config_set_read_increment     (&spi_dma_cfg, true);             // Read from buffer
    channel_config_set_write_increment    (&spi_dma_cfg, false);            // Write to SPI 0
                                                                        // Pace transfers based on availability of ADC samples:
    channel_config_set_dreq(&spi_dma_cfg, DREQ_ADC);                        // DREQ = ADC FIFO Data Request

    dma_channel_configure(
        spi_dma_chan,               // DMA Channel number that we got from dma_claim_unused_channel
        &spi_dma_cfg,                   // Configuration packet that we just made
        &spi_get_hw(SPI_PORT)->dr,  // Destination pointer
        &random_data,               // Source pointer (ADC FIFO)
        sizeof(random_data),        // Number of transfers
        false                       // Do not start immediately
    );


    dma_start_channel_mask(1u << spi_dma_chan) ;
}



void spi_dma_transfer( float * this_buff, u32 buff_size_32_bit)
{

    dma_channel_configure(
        spi_dma_chan,               // DMA Channel number that we got from dma_claim_unused_channel
        &spi_dma_cfg,                   // Configuration packet that we just made
        &spi_get_hw(SPI_PORT)->dr,  // Destination pointer
        this_buff,               // Source pointer (ADC FIFO)
        buff_size_32_bit,        // Number of transfers
        true                     // start immediately
    );

}