#include "adc.h"
//#include "bsp/board_api.h"
#include "hardware/adc.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "pico/multicore.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include "math.h"



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

u32 adc_interrupt;

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



u32 time_to_finish_adc_process;

void process_adc()
{
    u32 timer_snapshot = timer_hw->timerawl;

    if(adc_dma_finished)
    {
        adc_dma_finished = false;

        u32 this_sum = 0;
        uint16_t * ptr = &(adc_buffer[adc_buff_index_to_process][0]);

        for(int i = 0; i < NUM_ADC_SAMPLES; i++ )  //bump up i to skip some of the first values right after the mux change 
        {
            this_sum += *ptr;
            //*ptr = 0;
            ptr++;
        }

        adc_val[adc_pin_to_process] = this_sum / NUM_ADC_SAMPLES;

    }

    time_to_finish_adc_process = timer_hw->timerawl - timer_snapshot;
    adc_run(true);
}



//**************************************************************************
//**************************************************************************
//**************************************************************************
//**************************************************************************
//**************************************************************************
//**************************************************************************
//**************************************************************************
//**************************************************************************
//**************************************************************************
//**************************************************************************
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




void flash_dma_handler()
{
    // Stop the ADC
    adc_run(false);
    adc_fifo_drain();
    // Example action: print the last few samples
    // printf("ISR DMA complete. Last 5 samples:\n");
    // for (int i = NUM_SAMPLES - 5; i < NUM_SAMPLES; i++) {
    //     printf("%d\n", adc_buffer[i]);
    // }

    // Optionally restart DMA here if you want continuous streaming
    adc_run(true);
    dma_channel_set_read_addr(adc_dma_chan_num, &adc_hw->fifo, true);
    
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

        }
    }

    if(adc_val_changed)
    {
        printf("adc0: %5u  adc1: %5u  adc3: %5u \n", adc_val[0], adc_val[1], adc_val[2]);
    }

}



/*
void dma_handler() 
{
    // Check and clear the interrupt for our channel
    if (dma_hw->ints1 & (1u << adc_dma_chan_num)) 
    {
        dma_hw->ints1 = 1u << adc_dma_chan_num;
    }
        
    // Stop the ADC
    adc_run(false);
    adc_fifo_drain();
    // Example action: print the last few samples
    printf("ISR DMA complete. Last 5 samples:\n");
    for (int i = NUM_SAMPLES - 5; i < NUM_SAMPLES; i++) {
        printf("%d\n", adc_buffer[i]);
    }

    // Optionally restart DMA here if you want continuous streaming
    dma_channel_set_read_addr(adc_dma_chan_num, &adc_hw->fifo, true);
    adc_run(true);

    // Clear the interrupt request
    // dma_hw->ints0 = (1u << adc_dma_chan_num);

     // Check and clear the interrupt for our channel
    if (dma_hw->ints1 & (1u << adc_dma_chan_num)) 
    {
        dma_hw->ints1 = 1u << adc_dma_chan_num;
    }
        
}
    */



/*
void adc_collect()
{


    switch(adc_accumulate_count)
    {
        case 0 ... 50000:
        {
            adc_accumulate_count++;
            adc0_accumulate += adc_read();
        }break;
        case 50001:
        {
            adc_val[0] = adc0_accumulate / adc_accumulate_count;
            
            adc0_accumulate = 0;
            adc_accumulate_count = 0;
        }
    }

}



void check_adc_vals()
{
    bool adc_val_changed = false;
    for(int i=0; i<NUM_ADC_PINS; i++)
    {
        if(adc_val[i] != last_adc_val[i])
        {
            last_adc_val[i] = adc_val[i];
            adc_val_changed = true;

        }
    }

    if(adc_val_changed)
    {
        printf("adc0: %5u  adc1: %5u  adc3: %5u \n", adc_val[0], adc_val[1], adc_val[2]);
    }

}

*/
