//******************************************************************************************************
//
// File Name		: Remote.cpp
// Title		    : Remote C++ Code file
// Created:         : 5/31/2016 11:33:14 AM
// software         : version 2.0
// Author		    : Yaser Rashnabadi
// Target MCU		: Atmel AVR series
//
//******************************************************************************************************
#include "RemoteDecoder.h"

#define REGISTER_BIT_SET(_byte_, _bit_) ((_byte_) |= (1UL << (_bit_)))
#define REGISTER_BIT_CLEAR(_byte_, _bit_) ((_byte_) &= ~(1UL << (_bit_)))

static volatile uint16_t HTime;
static volatile uint16_t LTime;
static RemoteConfig currentConfig;

uint8_t RemoteDecoder::instanceCount = 0;
// volatile uint16_t RemoteDecoder::HTime;
// volatile uint16_t RemoteDecoder::LTime;
// //RemoteConfig RemoteDecoder::currentConfig;
volatile uint32_t RemoteDecoder::bit = 0;
volatile uint32_t RemoteDecoder::value = 0;
RemotePacket RemoteDecoder::data;
//****************************************************************************************************************************
//---------------------------------------------------- Button Function codes -------------------------------------------------
//****************************************************************************************************************************
//--------- Function: Constructor - Initialization of Port, Pin and  Mode interupt
//=============================================================================
RemoteDecoder::RemoteDecoder(volatile uint8_t &Portx, volatile uint8_t &Ddrx, uint8_t Pin, InterruptMode Mode)
{
	// Set port pin as pull-up or pull-down
	REGISTER_BIT_CLEAR(Portx, Pin);
	if (PULL_UP == Mode)
	{
		// Enable internal pull-up
		REGISTER_BIT_SET(Portx, Pin);
	}

	// Set ddr pin as input
	REGISTER_BIT_CLEAR(Ddrx, Pin);

	if (instanceCount < 255)
		instanceCount++;
}
//=============================================================================
//-------------------------------------------------------- Function: Destructor
//=============================================================================
RemoteDecoder::~RemoteDecoder(void)
{
	if (instanceCount > 0)
		instanceCount--;
}
//=============================================================================
//-------------------------------------------------------- Function:
//=============================================================================
void RemoteDecoder::init(const RemoteConfig& config)
{
    uint8_t sreg = SREG;
    cli();

    currentConfig = config;
    Reset_Buffer();

    TCCR1A = 0x00;
    TCCR1B = 0x00;
    TCNT1  = 0;

    TCCR1B = (1 << CS12);
    GICR &= ~(1 << INT0);
	
    MCUCR &= ~((1 << ISC01) | (1 << ISC00));
    MCUCR |=  ((1 << ISC01) | (1 << ISC00));

    GIFR |= (1 << INTF0);
    GICR |= (1 << INT0);

    SREG = sreg;
	sei();
}
//=============================================================================
//-------------------------------------------------------- Function:
//=============================================================================
void RemoteDecoder::deInit(void)
{
    uint8_t sreg = SREG;
    cli();

    GICR &= ~(1 << INT0);

    GIFR |= (1 << INTF0);

    MCUCR &= ~((1 << ISC01) | (1 << ISC00));

    TCCR1A = 0x00;
    TCCR1B = 0x00;

    TCNT1  = 0;

    TIMSK &= ~(1 << TOIE1);

    TIFR |= (1 << TOV1);

    Reset_Buffer();

    SREG = sreg;
}
//=============================================================================
//-------------------------------------------------------- Function:
//=============================================================================
inline bool RemoteDecoder::IsSyncPulse(uint16_t h, uint16_t l)
{
	return (l < h * currentConfig.syncMaxRatio) &&
		   (l > h * currentConfig.syncMinRatio);
}
//=============================================================================
//-------------------------------------------------------- Function:
//=============================================================================
inline bool RemoteDecoder::IsBitZero(uint16_t h, uint16_t l)
{
	return (l < h * currentConfig.bit0MaxRatio) &&
		   (l > h * currentConfig.bit0MinRatio);
}
//=============================================================================
//-------------------------------------------------------- Function:
//=============================================================================
inline bool RemoteDecoder::IsBitOne(uint16_t h, uint16_t l)
{
	return (h < l * currentConfig.bit1MaxRatio) &&
		   (h > l * currentConfig.bit1MinRatio);
}
//=============================================================================
//-------------------------------------------------------- Function:
//=============================================================================
inline void RemoteDecoder::Reset_Buffer(void)
{
	bit = currentConfig.totalBits;
	value = 0;
	data.detected = false;
	data.frame = 0;
	data.address = 0;
	data.key = 0;
}
//=============================================================================
//-------------------------------------------------------- Function:
//=============================================================================
void RemoteDecoder::Decoder(void)
{
	if (IsBitOne(HTime, LTime))
	{
		bit--;
		REGISTER_BIT_SET(value, bit);
	}
	else if (IsBitZero(HTime, LTime))
	{
		bit--;
	}
	else if (IsSyncPulse(HTime, LTime))
	{
		if (bit == 0)
		{
					data.frame = value;
					data.detected = true;
					data.address = value & 0x000FFFFF;
					data.key = (uint8_t)(value >> 20);

					bit = currentConfig.totalBits;
					value = 0;
		}
		else
			Reset_Buffer();
	}
	else
		Reset_Buffer();
}
//=============================================================================
//-------------------------------------------------------- Function:
//=============================================================================
ISR(INT0_vect)
{
	if (MCUCR == 3)
	{
		if (TCNT1 > currentConfig.maxTime || TCNT1 < currentConfig.minTime)
			LTime = 0;
		else
			LTime = TCNT1;
		TCNT1 = 0;
		MCUCR = 2; // Set interrupt on falling edge
		RemoteDecoder::Decoder();
	}
	else
	{
		if (TCNT1 > currentConfig.maxTime || TCNT1 < currentConfig.minTime)
			HTime = 0;
		else
			HTime = TCNT1;
		TCNT1 = 0;
		MCUCR = 3; // Set interrupt on rising edge
	}
}
//======================================================================================================
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<End
//======================================================================================================