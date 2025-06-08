#include "main.h"
#include "midi.h"

#include <stdio.h>
#include "pico/stdlib.h"
#include "tusb.h"             
#include "tusb_config.h"  
#include "bsp/board_api.h"




//midi settings
uint8_t const cable_num = 0; // MIDI jack associated with USB endpoint
uint8_t const channel   = 0; // 0 for channel 1


void handle_cc(u8 status, u8 cc_num, u8 cc_val);

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

        u8 Status_byte, byte_two, byte_three, cable_num;

        cable_num   = inpacket[MIDI_CABLE_NUM];    //cable_num
        Status_byte = inpacket[MIDI_BYTE_ONE];     //message type
        byte_two    = inpacket[MIDI_BYTE_TWO];     //Data byte1
        byte_three  = inpacket[MIDI_BYTE_THREE];   //Data byte2
        

        if(Status_byte & 0xF0 == MIDI_CC)
        {
            handle_cc(Status_byte, byte_two, byte_three);

            outpacket[0] = cable_num;
            outpacket[1] = Status_byte;
            outpacket[2] = byte_two;
            outpacket[3] = byte_three;
            tud_midi_packet_write(outpacket);
            
        }
    
        // 



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



void handle_cc(u8 status, u8 cc_num, u8 cc_val)
{
    //***  CONVERT TO 0-1 float  
    float map_val = cc_val / 127.f;


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