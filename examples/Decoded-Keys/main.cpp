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
#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

#include "Remote.h"

#define PORT_Decoder PORTD
#define DDR__Decoder DDRD
#define RXD 2 // Pin 2

REMOTE ev1527(PORT_Decoder, DDR__Decoder, RXD, PULL_UP);

int main()
{
  PORTC = 0x00;
  DDRC = 0xFF;

  REMOTE::init();

  while (1)
  {

    if (ev1527.data.Bits.Detect)
    {
      PORTC = ev1527.data.Bits.Keys;
    }
    else
    {
      PORTC = 0;
    }
  }
}
