#include "main.h"
#include "midi.h"

#include <stdio.h>
#include "pico/stdlib.h"
#include "tusb.h"             
#include "tusb_config.h"  
#include "bsp/board_api.h"




void handle_cc(u8 status, u8 cc_num, u8 cc_val)
{
    //***  CONVERT TO 0-1 float  
    float map_val = cc_val / 127.f;

    if(status & 0xF0 != MIDI_CC)
    {
        return;
    }

    switch(cc_num)
    {
        //***  For Launch Control XL
        //*********************************************
        //***********  KNOBS ROW 1  *******************
        //*********************************************
        case 13:        //set_atten
        {
            map_val = (map_val * 41) ; 
            gverb_set_roomsize(verb, map_val);
            
        }break;

        case 14:      
        {
            map_val = (map_val * 30); 
            gverb_set_revtime(verb, map_val);
            
        }break;
        
        case 15:     
        {
            map_val = (map_val * 5);  
            gverb_set_damping(verb, map_val);
            
        }break;
        
        case 16:     
        {
            map_val = (map_val * 5);  
            gverb_set_inputbandwidth(verb, map_val);
            
        }break;
        
        case 17:       
        {
            map_val = (map_val * 5);  
            gverb_set_earlylevel(verb, map_val);
            
        }break;
        
        case 18:      
        {
            map_val = (map_val * 5);  
            gverb_set_taillevel(verb, map_val);
        }break;
        
        case 19:       
        {
        }break;

        case 20:        
        {
        }break;
        
    }

    
}

//midi settings
// uint8_t const cable_num = 0; // MIDI jack associated with USB endpoint
// uint8_t const channel   = 0; // 0 for channel 1



u8 Status_byte, byte_two, byte_three, cable_num;
void usb_midi_task(void)
{
      static uint32_t start_ms = 0;

    
    // The MIDI interface always creates input and output port/jack descriptors
    // regardless of these being used or not. Therefore incoming traffic should be read
    // (possibly just discarded) to avoid the sender blocking in IO
    uint8_t inpacket[4];
    uint8_t outpacket[4];
    
    while ( tud_midi_available() )
    {
        
        tud_midi_packet_read(inpacket);
        outpacket[0] = inpacket[0];//cable_num?
        outpacket[1] = inpacket[1];//message type
        outpacket[2] = inpacket[2];//Data byte1
        outpacket[3] = inpacket[3];//Data byte2
        
    
        tud_midi_packet_write(outpacket);



        cable_num   = inpacket[MIDI_CABLE_NUM];    //cable_num
        Status_byte = inpacket[MIDI_BYTE_ONE];     //message type
        byte_two    = inpacket[MIDI_BYTE_TWO];     //Data byte1
        byte_three  = inpacket[MIDI_BYTE_THREE];   //Data byte2
        
        handle_cc(Status_byte, byte_two, byte_three);
        if(inpacket[MIDI_BYTE_ONE] & 0xF0 == MIDI_CC)
        {
            handle_cc(Status_byte, byte_two, byte_three);
            printf("foo\n");
            
        }

    }
    
    
    
    


     
 

}

uint8_t countup;
uint8_t cc_msg[3] = { 0xB0, 60, 0 };

void send_test_cc(void)
{
    
     // Send CC 
     cc_msg[2] = countup & 0x7F;
     tud_midi_stream_write(cable_num, cc_msg, 3);

     countup++;
}

