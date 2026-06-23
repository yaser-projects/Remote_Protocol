//****************************************************************************************************************************
//
// File Name      : Remote.h
// Title          : Remote interface header file
// software       : version 3.0
// Target MCU     : Atmel AVR Series
// Created        : 23/06/2026
// Author         : Yaser Rashnabadi
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

/**
 * @class RemoteDecoder
 * @brief A robust infrared remote control signal decoder for AVR microcontrollers.
 *
 * This class provides a complete solution for decoding common IR remote protocols
 * (such as NEC, Sony, RC5, etc.) using external interrupt INT0 and Timer1.
 *
 * It supports configurable timing parameters, tolerance, and different pin modes.
 * The decoder works in the background via interrupts and stores the received packet
 * in the static member `data`.
 *
 * @note Only one active decoder instance is typically needed. Multiple instances
 *       are supported but share the same interrupt and timer resources.
 */
class RemoteDecoder
{
public:
   /**
    * @brief Constructs a RemoteDecoder object and configures the input pin.
    *
    * @param Portx Reference to the PORT register of the pin (e.g. PORTD)
    * @param Ddrx  Reference to the DDR register of the pin (e.g. DDRD)
    * @param Pin   Pin number (0-7)
    * @param Mode  InterruptMode::PULL_UP or InterruptMode::PULL_DOWN
    */
   RemoteDecoder(volatile uint8_t &Portx,
                 volatile uint8_t &Ddrx,
                 uint8_t Pin,
                 InterruptMode Mode);

   /**
    * @brief Destructor. Decrements instance counter.
    */
   ~RemoteDecoder();

   /**
    * @brief Static member that holds the latest decoded remote packet.
    *
    * After a successful decode, check `data.detected` and read `data.address`,
    * `data.key`, etc.
    */
   static RemotePacket data;

   /**
    * @brief Initializes the remote decoder with protocol configuration.
    *
    * This function sets up Timer1, configures the interrupt, builds timing
    * limits based on the provided configuration, and enables the decoder.
    *
    * @param config Reference to a RemoteConfig structure containing protocol timing parameters.
    */
   static void initialize(const RemoteConfig &config);

   /**
    * @brief Deinitializes the decoder, disables interrupt and timer.
    *
    * Call this when you no longer need the remote decoder to free resources.
    */
   static void deinitialize(void);

   /**
    * @brief Friend declaration to allow ISR access to private static members.
    */
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

   static void selectTimer1Prescaler(uint16_t desiredTickUs);
   static void buildTimingLimits(const RemoteConfig &config);
   static void calculatePulseTiming(
       PulseTiming &pulse,
       uint8_t highRatio,
       uint8_t lowRatio);

   static bool isSyncPulse(uint16_t h, uint16_t l);
   static bool isBitZero(uint16_t h, uint16_t l);
   static bool isBitOne(uint16_t h, uint16_t l);

   static void resetBuffer(void);
   static void Decoder(void);
};
//======================================================================================================
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>  End Class <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//======================================================================================================
#endif /* REMOTE_H_ */