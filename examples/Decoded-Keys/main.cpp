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

#include "RemoteDecoder.h"


RemoteDecoder ev1527(PORTD, DDRD, 2, PULL_UP);

int main()
{
  PORTC = 0x00;
  DDRC = 0xFF;

  RemoteDecoder::init(EV1527_CONFIG);

  while (1)
  {

     if (ev1527.data.detected)
     {
       PORTC = ev1527.data.key;
     }
     else
     {
       PORTC = 0;
     }
  }
}
