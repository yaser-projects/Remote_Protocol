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

//=============================================================================
// RemoteDecoder Class
//-----------------------------------------------------------------------------
// RF remote control decoder for AVR microcontrollers.
//
// Supported protocols:
//
//   - EV1527
//   - HS1527
//   - PT2262
//   - SC2262
//   - LX2262
//   - HT12E
//   - HT6P20B
//
// The decoder uses INT0 and Timer1 to measure pulse widths and reconstruct
// the received frame according to the selected protocol configuration.
//
// Example:
//
//      #include "RemoteDecoder.h"
//
//      RemoteDecoder Remote(
//          PORTD,
//          DDRD,
//          PD2,
//          PULL_UP);
//
//      int main(void)
//      {
//          RemoteDecoder::initialize(EV1527_CONFIG);
//
//          while (1)
//          {
//              if (RemoteDecoder::data.detected)
//              {
//                  uint32_t address =
//                      RemoteDecoder::data.address;
//
//                  uint8_t key =
//                      RemoteDecoder::data.key;
//
//                  RemoteDecoder::data.detected = false;
//              }
//          }
//      }
//
// Decoded data:
//
//      RemoteDecoder::data.frame
//      RemoteDecoder::data.address
//      RemoteDecoder::data.key
//      RemoteDecoder::data.detected
//
//=============================================================================
class RemoteDecoder
{
public:
   public:

   //-------------------------------------------------------------------------
   // سازنده کلاس
   //-------------------------------------------------------------------------
   // پین ورودی مورد استفاده برای دریافت سیگنال ریموت را پیکربندی می‌کند.
   // در صورت انتخاب PULL_UP مقاومت Pull-Up داخلی نیز فعال خواهد شد.
   //
   // Portx : رجیستر PORT مربوط به پایه ورودی
   // Ddrx  : رجیستر DDR مربوط به پایه ورودی
   // Pin   : شماره پایه ورودی
   // Mode  : نوع ورودی (Pull-Up یا Pull-Down)
   //-------------------------------------------------------------------------
   RemoteDecoder(volatile uint8_t &Portx,
                 volatile uint8_t &Ddrx,
                 uint8_t Pin,
                 InterruptMode Mode);

   //-------------------------------------------------------------------------
   // مخرب کلاس
   //-------------------------------------------------------------------------
   // تعداد نمونه‌های فعال کلاس را مدیریت می‌کند.
   //-------------------------------------------------------------------------
   ~RemoteDecoder();

   //-------------------------------------------------------------------------
   // آخرین داده دیکود شده
   //-------------------------------------------------------------------------
   // پس از دریافت موفق یک فریم معتبر، اطلاعات زیر در این ساختار قرار می‌گیرد:
   //
   // data.frame     : فریم کامل دریافتی
   // data.address   : آدرس استخراج شده
   // data.key       : کد کلید استخراج شده
   // data.detected  : نشان‌دهنده دریافت موفق فریم
   //-------------------------------------------------------------------------
   static RemotePacket data;

   //-------------------------------------------------------------------------
   // راه‌اندازی دیکودر
   //-------------------------------------------------------------------------
   // دیکودر را با تنظیمات پروتکل مشخص شده فعال می‌کند.
   // این تابع Timer1 و INT0 را مقداردهی و فعال می‌کند.
   //
   // مثال:
   //      RemoteDecoder::initialize(EV1527_CONFIG);
   //-------------------------------------------------------------------------
   static void initialize(const RemoteConfig &config);

   //-------------------------------------------------------------------------
   // توقف دیکودر
   //-------------------------------------------------------------------------
   // وقفه INT0 و Timer1 را غیرفعال کرده و وضعیت داخلی دیکودر را پاک می‌کند.
   //-------------------------------------------------------------------------
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