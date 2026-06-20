
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
//======================================================================================================
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Interface Header Library use Firmware <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//======================================================================================================
#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

#include "Remote.h"
//======================================================================================================
// --------------------------------------- Global Setting ----------------------------------------------
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

static uint32_t Code;
static uint32_t MirrorCode;
static uint8_t OutPut;
static uint8_t id;
//======================================================================================================
// ----------------------------------------- Macro Setting ---------------------------------------------
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

#define SAVE1 6
#define SAVE2 7
#define SAVE3 0
#define SAVE4 1
//----------------------------------------------------
#define DELETE1 4
#define DELETE2 3
#define DELETE3 1
#define DELETE4 0
//----------------------------------------------------
#define JUMPER1 5
#define JUMPER2 4
#define JUMPER3 3
#define JUMPER4 2
//----------------------------------------------------
#define RELEY1 3
#define RELEY2 2
#define RELEY3 1
#define RELEY4 0
//=============================================================================
//----------------------------Select PORT & PIN -------------------------------

#define PORT_Decoder PORTD
#define DDR__Decoder DDRD
#define RXD 2 // Pin 2

#define RESTART_STATE 0x89ABCDEF
//==================================================================================================
// ----------------------------------------------------------------------- Auxiliary functions Macro
//==================================================================================================

#define Reley__ON(_byte_, _bit_) ((_byte_) |= (1UL << (_bit_)))
#define Reley_OFF(_byte_, _bit_) ((_byte_) &= ~(1UL << (_bit_)))

Database database;
RemoteTable table;
REMOTE ev1527(PORT_Decoder, DDR__Decoder, RXD, PULL_UP);
//==================================================================================================
// ----------------------------------------------------------------------------- Auxiliary functions
//==================================================================================================
//----------------- Save functions - Turn ON relays for Save operation -----------------------------

bool task_save1(BUTTON &button)
{
  PORTC++;
  // uint8_t save = 0;
  // Reley__ON(save, RELEY1);
  // table.Field.ID = id++;
  // table.Field.Address = ev1527.data.Frame;
  // table.Field.Switch = (save |= 0xF0);
  // database.InsertRecord(table);
  // PORTC = OutPut | save;
  return true; //<-- Return true for execute handler only once after button state changed
}

bool task_save2(BUTTON &button)
{
  uint8_t save = 0;
  Reley__ON(save, RELEY2);
  table.Field.ID = id++;
  table.Field.Address = ev1527.data.Frame;
  table.Field.Switch = (save |= 0xF0);
  database.InsertRecord(table);
  PORTC = OutPut | save;
  return true;
}

bool task_save3(BUTTON &button)
{
  uint8_t save = 0;
  Reley__ON(save, RELEY3);
  table.Field.ID = id++;
  table.Field.Address = ev1527.data.Frame;
  table.Field.Switch = (save |= 0xF0);
  database.InsertRecord(table);
  PORTC = OutPut | save;
  return true;
}

bool task_save4(BUTTON &button)
{
  uint8_t save = 0;
  Reley__ON(save, RELEY4);
  table.Field.ID = id++;
  table.Field.Address = ev1527.data.Frame;
  table.Field.Switch = (save |= 0xF0);
  database.InsertRecord(table);
  PORTC = OutPut | save;
  return true;
}
//==================================================================================================
//----------------- Delete functions - Turn OFF relays for Delete operation ------------------------

bool task_delete1(BUTTON &button)
{
  uint8_t _delete = 0xFF;

  Reley_OFF(_delete, RELEY1);
  database.Delete(_delete);
  Reley_OFF(OutPut, RELEY1);
  PORTC = OutPut;
  return true;
}

bool task_delete2(BUTTON &button)
{
  uint8_t _delete = 0xFF;

  Reley_OFF(_delete, RELEY2);
  database.Delete(_delete);
  Reley_OFF(OutPut, RELEY2);
  PORTC = OutPut;
  return true;
}

bool task_delete3(BUTTON &button)
{
  uint8_t _delete = 0xFF;

  Reley_OFF(_delete, RELEY3);
  database.Delete(_delete);
  Reley_OFF(OutPut, RELEY3);
  PORTC = OutPut;
  return true;
}

bool task_delete4(BUTTON &button)
{
  uint8_t _delete = 0xFF;

  Reley_OFF(_delete, RELEY4);
  database.Delete(_delete);
  Reley_OFF(OutPut, RELEY4);
  PORTC = OutPut;
  return true;
}
//==================================================================================================
//------------ Jumper functions - Override relay states based on jumper position -------------------

bool task_Jumper1(BUTTON &button)
{
  // PORTC++;
  // Reley_OFF(OutPut, RELEY1);
  // PORTC = OutPut;
  return true;
}

bool task_Jumper2(BUTTON &button)
{
  Reley_OFF(OutPut, RELEY2);
  PORTC = OutPut;
  return true;
}

bool task_Jumper3(BUTTON &button)
{
  Reley_OFF(OutPut, RELEY3);
  PORTC = OutPut;
  return true;
}

bool task_Jumper4(BUTTON &button)
{
  Reley_OFF(OutPut, RELEY4);
  PORTC = OutPut;
  return true;
}
//==================================================================================================
// -------------------------------------------- Setup ---------------------------------------------
//==================================================================================================

int main()
{
  PORTB = 0xFF;
  DDRB = 0x00;

  PORTC = 0xFF;
  DDRC = 0x00;

  PORTD = 0xFF;
  DDRD = 0x00;

  ACSR = 0x80;
  SFIOR = 0x00;

  REMOTE::init();

  // wdt_enable(WDTO_2S);
  //==================================================================================================
  //--------------------------------------------- Loop -----------------------------------------------
  //==================================================================================================
  while (1)
  {

    if (ev1527.data.Bits.Detect)
    {
      if (MirrorCode != Code)
      {
        Code = ev1527.data.Frame;
        OutPut ^= database.Serach(Code);
        OutPut |= 0xF0;
        PORTC = OutPut;

        MirrorCode = Code;
      }
    }
    else
    {

      MirrorCode = 0;
    }
  }
}
