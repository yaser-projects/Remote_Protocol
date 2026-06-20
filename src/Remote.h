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
#ifndef REMOTE_H_
#define REMOTE_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
//=============================================================================
//----------------------------bit use for Bite --------------------------------
#define Bit_use 24
//=============================================================================
//-----------------------------Min & Max Time ---------------------------------

#define Min_Time_Fail 18   // 400 us   800 us
#define Max_Time_Fail 1500 //
//=============================================================================
//=============================================================================

enum InterruptMode
{
   PULL_DOWN, // حالت Pull-Down
   PULL_UP         // حالت Pull-Up (با مقاومت داخلی)
};

typedef union
{
   uint32_t Frame; /**< Direct 32-bit access to entire decoded value */

   struct
   {
      uint32_t Address : 20; /**< 20-bit unique transmitter address (0 to 1,048,575) */
      uint8_t Keys : 4;      /**< 4-bit key/button code (0 to 15) - identifies which button pressed */
      uint8_t Detect : 1;    /**< Detection flag: 1=valid code received, 0=no detection */
      uint32_t Reserve : 8;  /**< Reserved bits for future use or alignment */
   } Bits;                   /**< Bit-field structure for easy field access */
} ev1527_T;

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

/*
 *کاربرد کلاس :
 *به منظور استفاده از کلید در پروژه های میکرو کنترولر از این کلاس استفاده می شود
 *نحوه ای استفاده :
 *
 * BUTTON Name(GPIO_Name,Pin_number,NOT_GATE Or BUFFER_GATE,noize);
 *
 * BUTTON Name;
 * Name = BUTTON(GPIO_Name,Pin_number,NOT_GATE Or BUFFER_GATE,noize);
 *
 * BUTTON Name = BUTTON(GPIO_Name,Pin_number,NOT_GATE Or BUFFER_GATE,noize);
 *
 *توجه :
 *این کلاس مشتق شده از هیچ کلاسی نمی باشد
 *
 */
class REMOTE
{
public:
   REMOTE(volatile uint8_t &Portx, volatile uint8_t &Ddrx, uint8_t Pin, InterruptMode Mode);
   ~REMOTE();

   ev1527_T static data;

   /* -------------------------------------------------------
    * @brief Disable EV1527 decoder and release hardware resources
    * @retval None
    * @note Deinitialization sequence:
    *       1. Disable INT0 external interrupt
    *       2. Stop Timer1 (set prescaler to 0 = no clock source)
    *       3. Set Timer1 to normal mode (clear all WGM bits)
    * @note Use this to save power when RF reception not needed
    *       or after successful code reception to prevent re-triggering
    * ------------------------------------------------------- */
   static void init(void);

   /* -------------------------------------------------------
    * @brief Disable EV1527 decoder and release hardware resources
    * @retval None
    * @note Deinitialization sequence:
    *       1. Disable INT0 external interrupt
    *       2. Stop Timer1 (set prescaler to 0 = no clock source)
    *       3. Set Timer1 to normal mode (clear all WGM bits)
    * @note Use this to save power when RF reception not needed
    *       or after successful code reception to prevent re-triggering
    * ------------------------------------------------------- */
   static void deInit(void);

   static void Reset_Buffer(void);
   static void Decoder(void);

private:
   // static const uint8_t MAX_BUTTONS = 20; // حداکثر دکمه‌ای که می‌خوای پشتیبانی کنی
   //  static BUTTON *instances[MAX_BUTTONS]; // آرایه از اشاره‌گرها
   static uint8_t object; // تعداد اشیائ ساخته شده از کلاس

   // ButtonEventHandler_t _onPressHandler;
   // ButtonEventHandler_t _onReleaseHandler;

   static volatile uint32_t bit;
   static volatile uint32_t value;
   // static volatile uint32_t buffer;
};
//======================================================================================================
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>  End Class <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//======================================================================================================
#endif /* REMOTE_H_ */