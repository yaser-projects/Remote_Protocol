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

#ifndef F_CPU
#define F_CPU 1000000UL
#endif

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

   static void initialize(const RemoteConfig &config);
   static void deinitialize(void);
   
   friend void INT0_vect(void);

private:
   static uint8_t instanceCount; // تعداد اشیائ ساخته شده از کلاس
   static TimingTable timing;
   static RemoteConfig activeConfig;
   static uint16_t timerTickUs;
   static volatile uint32_t bitIndex;
   static volatile uint32_t frameData;
   static uint32_t addressMask;

   static volatile uint16_t highTicks;
   static volatile uint16_t lowTicks;
   static uint16_t minTimeTicks;
   static uint16_t maxTimeTicks;
   //static volatile bool decodePending;

   static void selectTimer1Prescaler(uint16_t desiredTickUs);
   static void buildTimingLimits(const RemoteConfig &config);
   static void calculatePulseTiming(
       PulseTiming &pulse,
       uint8_t highRatio,
       uint8_t lowRatio);

   static bool IsSyncPulse(uint16_t h, uint16_t l);
   static bool IsBitZero(uint16_t h, uint16_t l);
   static bool isBitOne(uint16_t h, uint16_t l);

   static void resetBuffer(void);
   static void Decoder(void);
};
//======================================================================================================
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>  End Class <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//======================================================================================================
#endif /* REMOTE_H_ */