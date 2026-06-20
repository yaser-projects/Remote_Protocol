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
#include "Remote.h"

#define Is_Sync_Pulse(H, L) (L < H * 37 && L > H * 25)
#define Bit_Is_Zero(H, L) (L < H * 4 && L > H * 2)
#define Bit_Is_One(H, L) (H < L * 4 && H > L * 2)

static volatile uint16_t HTime, LTime;

#define REGISTER_BIT_SET(_byte_, _bit_) ((_byte_) |= (1UL << (_bit_)))
#define REGISTER_BIT_CLEAR(_byte_, _bit_) ((_byte_) &= ~(1UL << (_bit_)))

uint8_t REMOTE::object = 0;
volatile uint32_t REMOTE::bit = 0;
volatile uint32_t REMOTE::value = 0;
ev1527_T REMOTE::data;

// BUTTON *BUTTON::instances[BUTTON::MAX_BUTTONS] = {nullptr};
//****************************************************************************************************************************
//---------------------------------------------------- Button Function codes -------------------------------------------------
//****************************************************************************************************************************
//--------- Function: Constructor - Initialization of Port, Pin and  Mode interupt
//=============================================================================
REMOTE::REMOTE(volatile uint8_t &Portx, volatile uint8_t &Ddrx, uint8_t Pin, InterruptMode Mode)
{

	//_onPressHandler = nullptr;
	//_onReleaseHandler = nullptr;

	// Set port pin as pull-up or pull-down
	REGISTER_BIT_CLEAR(Portx, Pin);
	if (PULL_UP == Mode)
	{
		// Enable internal pull-up
		REGISTER_BIT_SET(Portx, Pin);
	}

	// Set ddr pin as input
	REGISTER_BIT_CLEAR(Ddrx, Pin);

	// if (object < MAX_BUTTONS)
	//{
	// instances[object] = this;
	object++;
	//}
};
//=============================================================================
//-------------------------------------------------------- Function: Destructor
//=============================================================================
REMOTE::~REMOTE(void)
{
	if (object > 0)
		object--;
};
//=============================================================================
//-------------------------------------------------------- Function:
//=============================================================================
void REMOTE::init(void)
{
	cli();
	TCCR1A = 0x00;
	TCCR1B = 0x04;
	TCNT1H = 0x00;
	TCNT1L = 0x00;
	ICR1H = 0x00;
	ICR1L = 0x00;
	OCR1AH = 0x00;
	OCR1AL = 0x00;
	OCR1BH = 0x00;
	OCR1BL = 0x00;

	// External Interrupt(s) initialization
	// INT0: On
	// INT0 Mode: Rising Edge
	// INT1: Off
	// INT2: Off
	GICR |= (1 << INT0);				  // Enable INT0 Interrupt
	MCUCR |= (1 << ISC01) | (1 << ISC00); // Trigger INT0 on rising edge
	GIFR |= (1 << INTF0);				  // flag INT0 Interrupt

	TIMSK |= 0x04;
	sei(); // Enable global interrupts
};
//=============================================================================
//-------------------------------------------------------- Function:
//=============================================================================
void REMOTE::deInit(void) {
	/* ===== Disable INT0 External Interrupt ===== */
	/* Clear INT0 edge detection configuration */
	// bitClear(EICRA, ISC00);                                  /**< ISC00=0: Disable edge detection (part 1) */
	// bitClear(EICRA, ISC01);                                  /**< ISC01=0: Disable edge detection (part 2) */

	/* Disable INT0 interrupt */
	// bitClear(EIMSK, INT0);                                   /**< Disable INT0 in External Interrupt Mask Register */

	/* ===== Disable Timer1 ===== */
	/* Clear Timer1 mode configuration (set to Normal mode - all WGM bits = 0) */
	//   bitClear(TCCR1A, WGM10);                                 /**< WGM10=0: Clear mode bit */
	//   bitClear(TCCR1A, WGM11);                                 /**< WGM11=0: Clear mode bit */
	//   bitClear(TCCR1B, WGM12);                                 /**< WGM12=0: Clear mode bit */

	//   /* Stop Timer1 by clearing prescaler (CS12:CS10 = 000 = No clock source) */
	//   bitClear(TCCR1B, CS10);                                  /**< CS10=0: Stop timer (part 1) */
	//   bitClear(TCCR1B, CS11);                                  /**< CS11=0: Stop timer (part 2) */
	//   bitClear(TCCR1B, CS12);                                  /**< CS12=0: Stop timer (part 3) - redundant but ensures complete stop */
};
//=============================================================================
//-------------------------------------------------------- Function:
//=============================================================================
inline void REMOTE::Reset_Buffer(void)
{
	bit = Bit_use;
	value = 0;
	data.Bits.Detect = false;
	data.Frame = 0;
	data.Bits.Address = 0;
	data.Bits.Keys = 0;
};
//=============================================================================
//-------------------------------------------------------- Function:
//=============================================================================
void REMOTE::Decoder(void)
{
	if (Bit_Is_One(HTime, LTime))
	{
		bit--;
		REGISTER_BIT_SET(value, bit);
	}
	else if (Bit_Is_Zero(HTime, LTime))
	{
		bit--;
	}
	else if (Is_Sync_Pulse(HTime, LTime))
	{
		if (bit == 0)
		{
			data.Frame = value;
			data.Bits.Detect = true;
			data.Bits.Address = value & 0x000FFFFF;
			data.Bits.Keys = (uint8_t)(value >> 20);

			bit = Bit_use;
			value = 0;
		}
		else
			Reset_Buffer();
	}
	else
		Reset_Buffer();
};
//=============================================================================
//-------------------------------------------------------- Function:
//=============================================================================
ISR(INT0_vect)
{
	if (MCUCR == 3)
	{
		if (TCNT1 > Max_Time_Fail || TCNT1 < Min_Time_Fail)
			LTime = 0;
		else
			LTime = TCNT1;
		TCNT1 = 0;
		MCUCR = 2; // Set interrupt on falling edge
		REMOTE::Decoder();
	}
	else
	{
		if (TCNT1 > Max_Time_Fail || TCNT1 < Min_Time_Fail)
			HTime = 0;
		else
			HTime = TCNT1;
		TCNT1 = 0;
		MCUCR = 3; // Set interrupt on rising edge
	}
};
//======================================================================================================
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<End
//======================================================================================================