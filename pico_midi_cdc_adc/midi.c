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
        
    
        tud_midi_packet_write(outpacket);       //This loops back any received messages to the host right now
    }
    
    

}


void send_test_cc(void)
{
    static uint8_t countup;
     // Send CC 
     uint8_t cc_msg[3] = { 0xB0 | channel, 60, countup & 0x7F };
     tud_midi_stream_write(cable_num, cc_msg, 3);

     countup++;
}