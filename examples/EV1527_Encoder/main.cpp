//******************************************************************************************************
//
// File Name    : Firmware AVR
// Title        : Firmware interface header file
// software     : version 2.0
// Target MCU   : AVR Series
// Created:     : 07/06/2026
// Author:      : Yaser Rashnabadi
//
//******************************************************************************************************
#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// #include "Remote.h"

uint32_t Code;
uint32_t C;
uint8_t i;

//=============================================================================
//----------------------------Select PORT & PIN -------------------------------

#define PORT_Decoder PORTD
#define DDR__Decoder DDRD
#define RXD 2 // Pin 2

// REMOTE ev1527(PORT_Decoder, DDR__Decoder, RXD, PULL_UP);
//==================================================================================================
//  ----------------------------------------------------------------------------- Auxiliary functions
//==================================================================================================
#define Pin (1 << 1)
#define Line_in PINC &= Pin
#define out1 PORTB |= (1 << 0)
#define out0 PORTB &= ~(1 << 0)
#define Time 335 // 335us //750us
#define Noise 100
#define r rand();

//==================================================================================================
//------------ Jumper functions - Override relay states based on jumper position ----------------

//==================================================================================================
//------------ Jumper functions - Override relay states based on jumper position ----------------

//==================================================================================================
int main()
{
  PORTA = 0xFF;
  DDRA = 0x00;

  PORTB = 0xF0;
  DDRB = 0x01;

  PORTC = 0xFF;
  DDRC = 0x00;

  PORTD = 0xFF;
  DDRD = 0x00;

  // REMOTE::init();

  while (1)
  {
    uint32_t key = (~PINA & 0x0F);

    uint8_t a = ~PINC;
    uint8_t b = ~PIND;
    uint8_t c = (~PINB & 0xF0);

    Code = 0;
    Code = (uint32_t)(a << 16) |
           //(uint32_t)(b << 8) |
           //(uint32_t)(c << 4) |
           (uint32_t)(key << 0);

    if (key)
    {
      C = Code;
      out1;
      _delay_us(Time);
      out0;
      _delay_us(Time * 31);
      for (i = 0; i < 24; i++)
      {
        if (C & 1)
        {
          out1;
          _delay_us(Time * 3);
          out0;
          _delay_us(Time);
        }
        else
        {
          out1;
          _delay_us(Time);
          out0;
          _delay_us(Time * 3);
        }
        C >>= 1;
      }
    }
    else
    {
      out1;
      _delay_us(Noise);
      out0;
      _delay_us(Noise * 2);
      out1;
      _delay_us(Noise * 3);
      out0;
      _delay_us(Noise * 4);
      out1;
      _delay_us(Noise * 5);
      out0;
      _delay_us(Noise * 20);
      out1;
      _delay_us(Noise * 4);
      out0;
      _delay_us(Noise * 3);
    }
  }
}
