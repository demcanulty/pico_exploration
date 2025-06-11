#ifndef _MIDI_H_
#define _MIDI_H_

#include "main.h"

#ifdef __cplusplus
extern "C" 
{
#endif



void usb_midi_task(void);
void send_test_cc(void);



// MIDI stuff

#define MIDI_CHANNEL_ONE		   0
#define MIDI_CHANNEL_TWO		   1
#define MIDI_CHANNEL_THREE		   2
#define MIDI_CHANNEL_FOUR		   3
#define MIDI_CHANNEL_FIVE		   4
#define MIDI_CHANNEL_SIX		   5
#define MIDI_CHANNEL_SEVEN		   6
#define MIDI_CHANNEL_EIGHT		   7
#define MIDI_CHANNEL_NINE		   8
#define MIDI_CHANNEL_TEN 		   9
#define MIDI_CHANNEL_ELEVEN		   10
#define MIDI_CHANNEL_TWELVE		   11
#define MIDI_CHANNEL_THIRTEEN	   12
#define MIDI_CHANNEL_FOURTEEN	   13
#define MIDI_CHANNEL_FIFTEEN	   14
#define MIDI_CHANNEL_SIXTEEN	   15


#define MIDI_NOTEOFF		0x80
#define MIDI_NOTEON			0x90
#define MIDI_POLY_ATOUCH	0xA0
#define MIDI_CC 			0xB0
#define MIDI_PGM			0xC0
#define MIDI_CHAN_ATOUCH	0xD0
#define MIDI_XBEND			0xE0
#define MIDI_SYSEX_START	0xF0
#define MIDI_SYSEX_END		0xF7
#define MIDI_BEATCLOCK		0xF8
#define MIDI_START			0xFA
#define MIDI_CONTINUE		0xFB
#define MIDI_STOP			0xFC

#define MIDI_RESET_CONTROLLERS  121
#define MIDI_ALL_NOTES_OFF      123 



// See section 4 of the USB MIDI Device Class Definition
#define	MIDI_CIN0	0x00	// Misc function codes, reserved
#define	MIDI_CIN1	0x01	// Cable events, reserved
#define	MIDI_CIN2	0x02	// Two byte system common message
#define	MIDI_CIN3	0x03	// Three byte system common message
#define	MIDI_CIN4	0x04	// Sysex starts or continues, three bytes
#define	MIDI_CIN5	0x05	// Single byte system common, or sysex ends with following single byte
#define	MIDI_CIN6	0x06    // Sysex ends with following two bytes
#define	MIDI_CIN7	0x07	// Sysex ends with following three bytes
#define	MIDI_CIN8	0x08	// Note Off, three bytes
#define	MIDI_CIN9	0x09    // Note On, three bytes
#define	MIDI_CIN10	0x0A	// Poly KeyPress, three bytes
#define	MIDI_CIN11	0x0B	// Control change, three bytes
#define	MIDI_CIN12	0x0C	// Program change, two bytes
#define	MIDI_CIN13	0x0D	// Channel pressure, two bytes
#define	MIDI_CIN14	0x0E	// Pitch bend, three bytes
#define	MIDI_CIN15	0x0F	// Single Byte

// CIN (Code Index Number) definitions for USB MIDI packets.
#define USB_MIDI_CIN_TWO_BYTE_SYSTEM_COMMON				0x2
#define USB_MIDI_CIN_THREE_BYTE_SYSTEM_COMMON			0x3
#define USB_MIDI_CIN_SYSEX_START_OR_CONTINUE			0x4
#define USB_MIDI_CIN_ONE_BYTE_SYSTEM_COMMON_OR_SYSEX	0x5
#define USB_MIDI_CIN_SYSEX_END_PLUS_TWO_BYTES			0x6
#define USB_MIDI_CIN_SYSEX_END_PLUS_THREE_BYTES			0x7
#define USB_MIDI_CIN_NOTE_OFF							0x8
#define USB_MIDI_CIN_NOTE_ON							0x9
#define USB_MIDI_CIN_POLY_KEY_PRESSURE					0xA
#define USB_MIDI_CIN_CONTROLLER_CHANGE					0xB
#define USB_MIDI_CIN_PROGRAM_CHANGE						0xC
#define USB_MIDI_CIN_CHANNEL_PRESSURE					0xD
#define USB_MIDI_CIN_PITCHBEND_CHANGE					0xE
#define USB_MIDI_CIN_SINGLE_BYTE						0xF

#define MIDI_CABLE_NUM          0
#define MIDI_BYTE_ONE			1
#define MIDI_BYTE_TWO			2
#define MIDI_BYTE_THREE			3


#define NUM_MIDI_CHANNELS		16
#define NUM_MIDI_CONTROLLERS	127
#define NUM_MIDI_NOTES          127



void USB_MIDI_NoteOn( U8 noteNum, U8 velocity, U8 chan );
void USB_MIDI_NoteOff( U8 noteNum, U8 velocity, U8 chan );
void USB_MIDI_ControlOut( U8 num, U8 val, U8 chan );
void USB_MIDI_ControlOut_debug( U8 num, U8 val, U8 chan );
void USB_MIDI_AftertouchOut( U8 key, U8 val, U8 chan );
void USB_MIDI_ChannelPressureOut( U8 val, U8 chan );
void USB_MIDI_PitchBendOut( U8 msb, U8 lsb, U8 chan );
void USB_MIDI_ProgramChangeOut( U8 program, U8 chan );
void USB_MIDI_ResetControlOut( U8 chan );
void USB_MIDI_AllNotesOffOut( U8 chan );
void USB_MIDI_MonoPolyOut( U8 chan, U8 fMonoEnabled );
void USB_MIDI_ResetOut( void );
void USB_MIDI_ClockOut( void );
void USB_MIDI_StartOut( void );
void USB_MIDI_StopOut( void );
void USB_MIDI_ContinueOut( void );
void USB_MIDI_SongSelectOut( U8 songNum );


#ifdef __cplusplus
}
#endif



#endif