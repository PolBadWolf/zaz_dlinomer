/* 
* LcdKey_Timer.cpp
*
* Created: 04.02.2017 11:34:08
* Author: User
*/


#include "path.h"
#include "LcdKey_Timer.h"
#include <avr/interrupt.h>

// timer0 - предделители 1, 8, 32, 64, 128, 256, 1024


namespace ns_LcdKeyTimer
{
	// ========================
	void Init()
	{
		volatile uint16_t prDiv;
		TCCR0 = 0;
		// CTC mode
		TCCR0_WGM01 = 1;
		TCCR0_WGM00 = 0;
		TCCR0_COM01 = 0;
		TCCR0_COM00 = 0;
		// pre Div
		prDiv = (uint16_t)( ((uint32_t)C_Fosc) / (  ((uint32_t)FT_LcdKeyTimer) * ((uint32_t)256) * ((uint32_t)1) )  );
		if (prDiv <= 1)
		{
			prDiv = 1;
			TCCR0_CS02 = 0;
			TCCR0_CS01 = 0;
			TCCR0_CS00 = 1;
		}
		else if (prDiv <= 8)
		{
			prDiv = 8;
			TCCR0_CS02 = 0;
			TCCR0_CS01 = 1;
			TCCR0_CS00 = 0;
		}
		else if (prDiv <= 32)
		{
			prDiv = 32;
			TCCR0_CS02 = 0;
			TCCR0_CS01 = 1;
			TCCR0_CS00 = 1;
		}
		else if (prDiv <= 64)
		{
			prDiv = 64;
			TCCR0_CS02 = 1;
			TCCR0_CS01 = 0;
			TCCR0_CS00 = 0;
		}
		else if (prDiv <= 128)
		{
			prDiv = 128;
			TCCR0_CS02 = 1;
			TCCR0_CS01 = 0;
			TCCR0_CS00 = 1;
		}
		else if (prDiv <= 256)
		{
			prDiv = 256;
			TCCR0_CS02 = 1;
			TCCR0_CS01 = 1;
			TCCR0_CS00 = 0;
		}
		else
		{
			prDiv = 1024;
			TCCR0_CS02 = 1;
			TCCR0_CS01 = 1;
			TCCR0_CS00 = 1;
		}

		// настройка частоты
		TCCR0_FOC0 = 1;
		TCNT0 = 0;
		//uint8_t div = (((unsigned long)C_Fosc) / prDiv / FT_LcdKeyTimer) - 1;
		uint8_t div = (uint8_t)( ((uint32_t)C_Fosc) / ( ((uint32_t)prDiv) * ((uint32_t)FT_LcdKeyTimer) * ((uint32_t)1) ) ) - 1;
		OCR0 = div;
		TIFR_OCF0 = 0;
		TIFR_TOV0 = 0;
		TIMSK_OCIE0 = 1;
	}

	ISR (TIMER0_COMP_vect)
	{
		TIMSK_OCIE0 = 0;
		sei();
		// lcd
		ns_lcd::Interrupt();
		// key
		key4->Interrupt();
		// clock rt
		ns_clock::Interrupt();
		// interrupt module
		ns_module::InterruptLcdTimer();
		TIMSK_OCIE0 = 1;
	}

}

