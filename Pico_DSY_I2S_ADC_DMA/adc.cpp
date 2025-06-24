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



u8 adc_buff_index;
u8 adc_buff_index_to_process;
u32 time_to_finish_adc_scan;

uint16_t  __attribute__((aligned(4))) adc_buffer[2][NUM_ADC_SAMPLES];
uint16_t adc_val[3];        
uint16_t last_adc_val[3];
int adc_dma_chan_num;
dma_channel_config dma_cfg;

bool adc_dma_finished;
uint32_t adc_interrupt_count;
u8 adc_pin_to_process;


static void __isr __time_critical_func(dma_handler)(void)
{
    
    static u32 this_time;
    static u8  this_adc_pin;

    //***  START TIMER  ***
    u32 timer_snapshot = timer_hw->timerawl;
    time_to_finish_adc_scan = timer_snapshot - this_time;
    this_time = timer_snapshot;
    adc_interrupt_count++;
    
    //***  STOP ADC  ***
    adc_run(false);
    
    //*** SWAP BUFFERS  ***
    adc_buff_index_to_process = adc_buff_index;
    adc_buff_index = !adc_buff_index;


    //*** START PROCESSING  ***
    adc_pin_to_process = this_adc_pin;
    adc_dma_finished = true;

    //***  INCREMENT PIN AND CHECK BOUNDS  ***
    this_adc_pin++;
    if(this_adc_pin >= NUM_ADC_PINS)
    {
        this_adc_pin = 0;
    }
    adc_select_input(this_adc_pin);
    //***  RESTART DMA **************************
    //***  NEED TO RESET THE WRITE ADDRESS!!  ***
    dma_channel_hw_addr(adc_dma_chan_num)->write_addr = (uintptr_t) &adc_buffer[adc_buff_index][0];
    dma_channel_hw_addr(adc_dma_chan_num)->al3_read_addr_trig = (uintptr_t) &adc_hw->fifo;

    //***  ACK / CLEAR THE DMA ISR  ***
    dma_hw->ints1 |= 1u << adc_dma_chan_num;

    //RESTART THE ADC
    adc_run(true);
}

//*************************************************************
//*************************************************************
//*************************************************************
//*************************************************************

u32 time_to_finish_adc_process;

void process_adc()
{
    
    //***  ONLY RUN AFTER INTERRUPT  ***
    if(adc_dma_finished)
    {
        //*** START TIMER ***
        u32 timer_snapshot = timer_hw->timerawl;

        //***  CLEAR FLAG  *** */
        adc_dma_finished = false;

        u32 this_sum = 0;

        //***  GET POINTER TO INACTIVE BUFFER ***
        uint16_t * ptr = &(adc_buffer[adc_buff_index_to_process][0]);

        for(int i = 0; i < NUM_ADC_SAMPLES; i++ )
        {
            //*** SUM BUFFER  ***
            this_sum += *ptr++;
        }

        //*** DECIMATE AND STORE  ***
        adc_val[adc_pin_to_process] = this_sum / NUM_ADC_SAMPLES;

        time_to_finish_adc_process = timer_hw->timerawl - timer_snapshot;

    }

    
}




//**************************************************************************
//**************************************************************************
//**************************************************************************
//**************************************************************************
//**************************************************************************


void init_project_adc()
{
     adc_init();
    adc_gpio_init(26);
    adc_gpio_init(27);
    adc_gpio_init(28);

    adc_select_input(0);


    //****************************
    //***  Configure ADC FIFO  ***
    //****************************

    adc_fifo_setup(
        true,     // Enable      - Enable ability to write each result to the FIFO
        true,     // DREQ Enable - Enable DMA data request 
        1,        // Drec_Thresh - DREQ (Data Request) asserted when at least 1 sample in FIFO
        false,    // Err_in_fifo - Don't add an error flag on the 15th bit of every sample
        false     // Byte_shift  - No 8-bit packing (we want full 12-bit samples)
    );

    adc_set_clkdiv(0);   //0 is full speed 

    printf("Arming DMA\n");
    // sleep_ms(1000);
    //**************************
    //*** Set up DMA channel ***
    //**************************
    adc_dma_chan_num             = dma_claim_unused_channel(true);
    dma_cfg  = dma_channel_get_default_config(adc_dma_chan_num);
    channel_config_set_transfer_data_size (&dma_cfg, DMA_SIZE_16);      // 16-bit
    channel_config_set_read_increment     (&dma_cfg, false);            // Read from same address (ADC Register)
    channel_config_set_write_increment    (&dma_cfg, true);             // Write to buffer and increment pointer every time
                                                                        // Pace transfers based on availability of ADC samples:
    channel_config_set_dreq(&dma_cfg, DREQ_ADC);                        // DREQ = ADC FIFO Data Request


    dma_channel_configure(
        adc_dma_chan_num,     // DMA Channel number that we got from dma_claim_unused_channel
        &dma_cfg,             // Configuration packet that we just made
        adc_buffer,           // Destination pointer
        &adc_hw->fifo,        // Source pointer (ADC FIFO)
        NUM_ADC_SAMPLES,          // Number of transfers
        false                 // Do not start immediately
    );

    
    printf("about to enable dma\n");
    //***  START DMA  ***
    dma_channel_start(adc_dma_chan_num);

    // //***  START ADC  ***
    adc_run(true);
    


    //*** DMA IRQ Setup ***
    dma_channel_set_irq1_enabled(adc_dma_chan_num, true);   // Enable IRQ on specific DMA channel
    irq_set_exclusive_handler(DMA_IRQ_1, dma_handler);      // Label our interrupt handler function 
    irq_set_priority(DMA_IRQ_1, 0xff);
    irq_set_enabled(DMA_IRQ_1, true);                       // Enables interrupt on the executing core       


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
        if(abs(adc_val[i] - last_adc_val[i]) > 1)   
        {
        
            last_adc_val[i] = adc_val[i];
            adc_val_changed = true;

            control_val_changed(i, adc_val[i]);
        }
    }

    // if(adc_val_changed)
    // {
    //     printf("adc0: %5u  adc1: %5u  adc3: %5u \n", adc_val[0], adc_val[1], adc_val[2]);
    // }

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
