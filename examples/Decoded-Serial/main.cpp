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
#include <avr/interrupt.h>
#include <stdio.h>

#include "RemoteDecoder.h"

static uint32_t Code;
static uint32_t MirrorCode;
char buffer[64];

RemoteDecoder ev1527(PORTD, DDRD, 2, PULL_UP);
//==================================================================================================
// ----------------------------------------------------------------------------- Auxiliary functions
//==================================================================================================
void UART_Init(void)
{
  // تنظیم Baud Rate
  // UBRRH = (unsigned char)(UBRR_VALUE >> 8);
  // UBRRL = (unsigned char)UBRR_VALUE;

  // Baud Rate = 9600 @ 8MHz
  UBRRH = 0;
  UBRRL = 51;

  // TX Enable
  UCSRB = (1 << TXEN);

  // 8 Data Bits, 1 Stop Bit, No Parity
  UCSRC = (1 << URSEL) |
          (1 << UCSZ1) |
          (1 << UCSZ0);
}
//==================================================================================================
//------------ Jumper functions - Override relay states based on jumper position ----------------
void UART_Send_U8(uint8_t data)
{
  while (!(UCSRA & (1 << UDRE)))
    ;
  UDR = data;
}
//==================================================================================================
//------------ Jumper functions - Override relay states based on jumper position ----------------
void UART_Send_U32(uint32_t data)
{
  while (!(UCSRA & (1 << UDRE)))
    ;
  for (uint8_t i = 0; i < 4; i++)
    UDR = (uint8_t)data >> (i * 8);
}
//==================================================================================================
//------------ Jumper functions - Override relay states based on jumper position ----------------
void UART_SendChar(char data)
{
  // انتظار تا بافر ارسال خالی شود
  while (!(UCSRA & (1 << UDRE)))
    ;

  // ارسال داده
  UDR = data;
}
//==================================================================================================
//------------ Jumper functions - Override relay states based on jumper position ----------------
void UART_SendString(const char *str)
{
  while (*str)
  {
    UART_SendChar(*str++);
  }
}
//==================================================================================================
int main()
{
  PORTD = 0xFF;
  DDRD = 0x00;

  UART_Init();
  UART_SendString("\r\n[SYSTEM] EV1527 Receiver Started\r\n");

  RemoteDecoder::init(EV1527_CONFIG);

  while (1)
  {
    if (ev1527.data.detected)
    {
      
      if (MirrorCode != Code)
      {
        UART_SendString("\r\n [EV1527]");

        sprintf(buffer, "\r\n | Frame: 0x%06lX", ev1527.data.frame);
        UART_SendString(buffer);

        sprintf(buffer, "\r\n | Addr:  0x%05lX", ev1527.data.address);
        UART_SendString(buffer);

        sprintf(buffer, "\r\n | Key:   0x%X\r\n", ev1527.data.key);
        UART_SendString(buffer);

        UART_SendString("\r\n"); // یک بار در آخر

        MirrorCode = Code;
      }
      else
      {
        Code = ev1527.data.frame;
      }
    }
    else
    {
      MirrorCode = 0;
    }
  }
}
