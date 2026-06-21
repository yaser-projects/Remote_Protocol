//*****************************************************************************
//
// File Name		: Remote.h
// Title		      : Remote interface header file
// software       : version 2.0
// Target MCU		: Atmel AVR Series
// Created:       : 07/06/2026
// Author:        : Yaser Rashnabadi
//
//****************************************************************************************************************************
#ifndef REMOTE_DECODER_H_
#define REMOTE_DECODER_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "RemoteConfig.h"
//=============================================================================
// Interrupt Mode
//=============================================================================
enum InterruptMode
{
   PULL_DOWN, // حالت Pull-Down
   PULL_UP    // حالت Pull-Up (با مقاومت داخلی)
};
//=========================== ISR Declarations (Global) ===========================
#ifdef __cplusplus
extern "C"
{
#endif

   ISR(INT0_vect);

#ifdef __cplusplus
}
#endif
//=======================================================================================================
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Interface Function use Class <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//=======================================================================================================

class RemoteDecoder
{
public:
   RemoteDecoder(volatile uint8_t &Portx, volatile uint8_t &Ddrx, uint8_t Pin, InterruptMode Mode);

   ~RemoteDecoder();

  static RemotePacket data;

   static void init(const RemoteConfig &config);
   static void deInit(void);
   static void Decoder(void);

private:
   static uint8_t instanceCount; // تعداد اشیائ ساخته شده از کلاس

   static volatile uint32_t bit;
   static volatile uint32_t value;

   static bool IsSyncPulse(uint16_t h, uint16_t l);
   static bool IsBitZero(uint16_t h, uint16_t l);
   static bool IsBitOne(uint16_t h, uint16_t l);

   static void Reset_Buffer(void);
};
//======================================================================================================
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>  End Class <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//======================================================================================================
#endif /* REMOTE_H_ */