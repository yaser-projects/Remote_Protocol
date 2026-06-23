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

#define registerBitSet(_byte_, _bit_) ((_byte_) |= (1UL << (_bit_)))
#define registerBitClear(_byte_, _bit_) ((_byte_) &= ~(1UL << (_bit_)))

uint8_t RemoteDecoder::instanceCount = 0;
uint16_t RemoteDecoder::timerTickUs = 0;
TimingTable RemoteDecoder::timing;
RemoteConfig RemoteDecoder::activeConfig;
volatile uint32_t RemoteDecoder::bitIndex = 0;
volatile uint32_t RemoteDecoder::frameData = 0;
uint32_t RemoteDecoder::addressMask = 0;
RemotePacket RemoteDecoder::data;

volatile uint16_t RemoteDecoder::highTicks;
volatile uint16_t RemoteDecoder::lowTicks;
uint16_t RemoteDecoder::minTimeTicks;
uint16_t RemoteDecoder::maxTimeTicks;
//volatile bool RemoteDecoder::decodePending = false;
//****************************************************************************************************************************
//---------------------------------------------------- Button Function codes -------------------------------------------------
//****************************************************************************************************************************
//------ Function: Constructor - Initialization of Port, Pin and  Mode interupt
//=============================================================================
RemoteDecoder::RemoteDecoder(volatile uint8_t &Portx, volatile uint8_t &Ddrx, uint8_t Pin, InterruptMode Mode)
{
	// Set port pin as pull-up or pull-down
	registerBitClear(Portx, Pin);
	if (PULL_UP == Mode)
	{
		// Enable internal pull-up
		registerBitSet(Portx, Pin);
	}

	// Set ddr pin as input
	registerBitClear(Ddrx, Pin);

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
//- Function: Configure Timer1 for the closest tick resolution to desiredTickUs
//=============================================================================
void RemoteDecoder::selectTimer1Prescaler(uint16_t desiredTickUs)
{
	struct
	{
		uint16_t prescaler;
		uint8_t csBits;
	} options[] =
		{
			{1, (1 << CS10)},
			{8, (1 << CS11)},
			{64, (1 << CS11) | (1 << CS10)},
			{256, (1 << CS12)},
			{1024, (1 << CS12) | (1 << CS10)}};

	uint32_t bestError = 0xFFFFFFFFUL;
	uint8_t bestIndex = 0;

	for (uint8_t i = 0; i < 5; i++)
	{
		uint32_t tickUs =
			((uint32_t)options[i].prescaler * 1000000UL) / F_CPU;

		uint32_t error =
			(tickUs > desiredTickUs) ? (tickUs - desiredTickUs) : (desiredTickUs - tickUs);

		if (error < bestError)
		{
			bestError = error;
			bestIndex = i;
			timerTickUs = tickUs;
		}
	}

	TCCR1A = 0;
	TCCR1B = options[bestIndex].csBits;
}
//=============================================================================
//------------- Calculate minimum and maximum valid pulse widths in timer ticks
//=============================================================================
void RemoteDecoder::buildTimingLimits(const RemoteConfig &config)
{
	// بزرگ‌ترین پالس ممکن در پروتکل
	uint32_t maxRatio =
		(config.syncHighRatio > config.syncLowRatio) ? config.syncHighRatio : config.syncLowRatio;

	uint32_t maxPulseUs =
		(uint32_t)config.pulseWidthUs * maxRatio;

	// تبدیل به ticks
	uint32_t maxTicks =
		(maxPulseUs + (timerTickUs / 2)) / timerTickUs;

	uint32_t toleranceTicks =
		(maxTicks * config.tolerancePercent) / 100;

	maxTimeTicks = (uint16_t)(maxTicks + toleranceTicks);
	minTimeTicks = 1;
}
//=============================================================================
//-------------------------------- Calculate pulse timing limits in timer ticks
//=============================================================================
void RemoteDecoder::calculatePulseTiming(PulseTiming &pulse, uint8_t highRatio, uint8_t lowRatio)
{
	uint32_t expectedHigh = (uint32_t)activeConfig.pulseWidthUs * highRatio;
	uint32_t expectedLow = (uint32_t)activeConfig.pulseWidthUs * lowRatio;
	uint32_t toleranceHigh = (expectedHigh * activeConfig.tolerancePercent) / 100;
	uint32_t toleranceLow = (expectedLow * activeConfig.tolerancePercent) / 100;

	pulse.highTime.min = (uint16_t)((expectedHigh - toleranceHigh) / timerTickUs);
	pulse.highTime.max = (uint16_t)((expectedHigh + toleranceHigh) / timerTickUs);

	pulse.lowTime.min = (uint16_t)((expectedLow - toleranceLow) / timerTickUs);
	pulse.lowTime.max = (uint16_t)((expectedLow + toleranceLow) / timerTickUs);
}
//=============================================================================
//------------- Configure and enable Timer1 and INT0 for remote signal decoding
//=============================================================================
void RemoteDecoder::initialize(const RemoteConfig &config)
{
	cli();

	activeConfig = config;

	if (activeConfig.addressBits >= 32)
		addressMask = 0xFFFFFFFFUL;
	else
		addressMask = (1UL << activeConfig.addressBits) - 1UL;

	selectTimer1Prescaler(10);
	buildTimingLimits(config);

	calculatePulseTiming(
		timing.sync,
		activeConfig.syncHighRatio,
		activeConfig.syncLowRatio);

	calculatePulseTiming(
		timing.bit0,
		activeConfig.bit0HighRatio,
		activeConfig.bit0LowRatio);

	calculatePulseTiming(
		timing.bit1,
		activeConfig.bit1HighRatio,
		activeConfig.bit1LowRatio);

	resetBuffer();

	TCNT1 = 0;
	GICR &= ~(1 << INT0);
	MCUCR |= ((1 << ISC01) | (1 << ISC00));
	GIFR |= (1 << INTF0);
	GICR |= (1 << INT0);

	sei();
}
//=============================================================================
//--------------------------- Disable INT0, stop Timer1 and reset decoder state
//=============================================================================
void RemoteDecoder::deinitialize(void)
{
	uint8_t sreg = SREG;
	cli();

	GICR &= ~(1 << INT0);
	GIFR |= (1 << INTF0);

	MCUCR &= ~((1 << ISC01) | (1 << ISC00));

	TCCR1A = 0x00;
	TCCR1B = 0x00;

	TCNT1 = 0;

	TIMSK &= ~(1 << TOIE1);

	TIFR |= (1 << TOV1);

	resetBuffer();

	SREG = sreg;
}
//=============================================================================
//-------------------------- Check whether pulse timings match the sync pattern
//=============================================================================
inline bool RemoteDecoder::isSyncPulse(uint16_t h, uint16_t l)
{
	return (h >= timing.sync.highTime.min) &&
		   (h <= timing.sync.highTime.max) &&
		   (l >= timing.sync.lowTime.min) &&
		   (l <= timing.sync.lowTime.max);
}
//=============================================================================
//-------------------------- Check whether pulse timings represent a logic '0'.
//=============================================================================
inline bool RemoteDecoder::isBitZero(uint16_t h, uint16_t l)
{
	return (h >= timing.bit0.highTime.min) &&
		   (h <= timing.bit0.highTime.max) &&
		   (l >= timing.bit0.lowTime.min) &&
		   (l <= timing.bit0.lowTime.max);
}
//=============================================================================
//-------------------------- Check whether pulse timings represent a logic '1'.
//=============================================================================
inline bool RemoteDecoder::isBitOne(uint16_t h, uint16_t l)
{
	return (h >= timing.bit1.highTime.min) &&
		   (h <= timing.bit1.highTime.max) &&
		   (l >= timing.bit1.lowTime.min) &&
		   (l <= timing.bit1.lowTime.max);
}
//=============================================================================
//-------------- Reset bit counter, frame buffer and decoded packet information
//=============================================================================
inline void RemoteDecoder::resetBuffer(void)
{
	bitIndex = activeConfig.totalBits;
	frameData = 0;
	data.detected = false;
	data.frame = 0;
	data.address = 0;
	data.key = 0;
}
//=============================================================================
//---------- Decode received pulse timings and reconstruct remote control frame
//=============================================================================
void RemoteDecoder::Decoder(void)
{
	// if (!decodeFlag)
	// 	return;

	if (isBitOne(highTicks, lowTicks))
	{
		bitIndex--;
		registerBitSet(frameData, bitIndex);
	}
	else if (isBitZero(highTicks, lowTicks))
	{
		bitIndex--;
	}
	else if (isSyncPulse(highTicks, lowTicks))
	{
		if (bitIndex == 0)
		{
			data.detected = true;
			data.frame = frameData;
			data.address = frameData & addressMask;
			data.key = (uint8_t)(frameData >> activeConfig.addressBits);

			bitIndex = activeConfig.totalBits;
			frameData = 0;
		}
		else
			resetBuffer();
	}
	else
		resetBuffer();

	//decodePending = false;
}
//=============================================================================
// Measure pulse width on INT0 edges and switch trigger between rising/falling for bit decoding
//=============================================================================
ISR(INT0_vect)
{
	uint16_t elapsedTicks = TCNT1;
	TCNT1 = 0;

	if ((MCUCR & ((1 << ISC01) | (1 << ISC00))) == ((1 << ISC01) | (1 << ISC00)))
	{
		RemoteDecoder::lowTicks = (elapsedTicks >= RemoteDecoder::minTimeTicks && elapsedTicks <= RemoteDecoder::maxTimeTicks) ? elapsedTicks : 0;
		MCUCR = (1 << ISC01); // Set interrupt on falling edge
							  // RemoteDecoder::decodeFlag = true;
		RemoteDecoder::Decoder();
	}
	else
	{
		RemoteDecoder::highTicks = (elapsedTicks >= RemoteDecoder::minTimeTicks && elapsedTicks <= RemoteDecoder::maxTimeTicks) ? elapsedTicks : 0;
		MCUCR = (1 << ISC01) | (1 << ISC00); // Set interrupt on rising edge
	}
}
//======================================================================================================
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<End
//======================================================================================================