/* 
* timer0.cpp
*
* Created: 19.10.2016 8:06:54
* Author: GumerovMN
*/


#include "timer2.h"
#include "path.h"
#include <avr/interrupt.h>

namespace ns_timerMain
{
	// предделители 1, 8, 64, 256, 1024
	void Init()
	{
		uint16_t prDiv;
		TCCR2 = 0;
		// CTC mode
		TCCR2_WGM21 = 1;
		TCCR2_WGM20 = 0;
		TCCR2_COM21 = 0;
		TCCR2_COM20 = 0;
		// pre Div
		prDiv = (uint16_t)( ((uint32_t)C_Fosc) / (  ((uint32_t)FtTimerMain) * ((uint32_t)256) * ((uint32_t)1) )  );
		if (prDiv <= 1)
		{
			prDiv = 1;
			TCCR2_CS22 = 0;
			TCCR2_CS21 = 0;
			TCCR2_CS20 = 1;
		}
		else if (prDiv <= 8)
		{
			prDiv = 8;
			TCCR2_CS22 = 0;
			TCCR2_CS21 = 1;
			TCCR2_CS20 = 0;
		}
		else if (prDiv <= 64)
		{
			prDiv = 64;
			TCCR2_CS22 = 0;
			TCCR2_CS21 = 1;
			TCCR2_CS20 = 1;
		}
		else if (prDiv <= 256)
		{
			prDiv = 256;
			TCCR2_CS22 = 1;
			TCCR2_CS21 = 0;
			TCCR2_CS20 = 0;
		}
		else
		{
			prDiv = 1024;
			TCCR2_CS22 = 1;
			TCCR2_CS21 = 0;
			TCCR2_CS20 = 1;
		}
		// настройка частоты
		TCCR2_FOC2 = 0;
		TCNT2 = 0;
		uint8_t div = (uint8_t)( ((uint32_t)C_Fosc) / ( ((uint32_t)prDiv) * ((uint32_t)FtTimerMain) * ((uint32_t)1) ) ) - 1;
		OCR2 = div;
		TIFR_OCF2 = 0;
		TIFR_TOV2 = 0;
		TIMSK_OCIE2 = 1;
	}
	// =============
	ISR(TIMER2_COMP_vect)
	{
		ns_module::InterruptMainTimer();
	}
}