#ifndef _MAIN_H_
#define _MAIN_H_

#include <stdio.h>
#include <stdint.h>



#include "audio_process.h"
#ifdef __cplusplus
#include <cstdint>
#endif


#include "gverb.h"



typedef uint8_t   u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;


typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;



typedef int8_t                  S8 ;  //!< 8-bit signed integer.
typedef uint8_t                 U8 ;  //!< 8-bit unsigned integer.
typedef int16_t                 S16;  //!< 16-bit signed integer.
typedef uint16_t                U16;  //!< 16-bit unsigned integer.
typedef uint16_t                le16_t;
typedef uint16_t                be16_t;
typedef int32_t                 S32;  //!< 32-bit signed integer.
typedef uint32_t                U32;  //!< 32-bit unsigned integer.
typedef uint32_t                le32_t;
typedef uint32_t                be32_t;
typedef int64_t                 S64;  //!< 64-bit signed integer.
typedef uint64_t                U64;  //!< 64-bit unsigned integer.
typedef float                   F32;  //!< 32-bit floating-point number.
typedef double                  F64;  //!< 64-bit floating-point number.
typedef uint32_t                iram_size_t;



//C Project measurements
//Blink Pico  RP2040 at 125Mhz - 1,373,298 runs through main
//Blink Pico  RP2040 at 200Mhz - 2,197,497 runs through main
//Blink Pico2 RP2350 at 150Mhz - 2,542,031 runs through main

//C++ Project measurements, just blinking 
//Blink Pico  RP2040 at 125Mhz - 2,118,127 runs through main
//Blink Pico  RP2040 at 200Mhz - 3,389,315 runs through main
//Blink Pico2 RP2350 at 150Mhz - 4,544,875 runs through main



/*
Note pico SDK 2.1.1 - 
replaced the following function in dcd_rp2040.c in the tinyusb library.
It's not perfect, but it crashes less often when switching to 24 bit.
See:  https://github.com/hathach/tinyusb/pull/2937

// New API: Configure and enable an ISO endpoint according to descriptor
bool dcd_edpt_iso_activate(uint8_t rhport, tusb_desc_endpoint_t const * ep_desc) {
  (void) rhport;
  const uint8_t ep_addr = ep_desc->bEndpointAddress;

  printf("ep_desc->wMaxPacketSize: %d\r\n",ep_desc->wMaxPacketSize);

  // init w/o allocate (removed quantize size to 64)
  hw_endpoint_init(ep_addr, ep_desc->wMaxPacketSize, TUSB_XFER_ISOCHRONOUS);

  // Fill in endpoint control register with buffer offset
  struct hw_endpoint* ep = hw_endpoint_get_by_addr(ep_addr);
  TU_ASSERT(ep->hw_data_buf != NULL); // must be inited and buffer allocated
  ep->wMaxPacketSize = ep_desc->wMaxPacketSize;

  hw_endpoint_enable(ep);
  return true;
}

*/

//maybe investigate this project for 24khz ideas:  https://github.com/thisiseth/rp2040-dac-amp




// extern "C" int pico_led_init(void);
// extern "C" void pico_set_led(bool led_on);

extern uint32_t blink_interval_ms;

#endif