/*
 * izmLen-mega128.cpp
 *
 * Created: 04.02.2017 11:25:02
 * Author : User
 */ 

#include "system/path.h"
#include <avr/interrupt.h>

	//uint16_t bb[2] = {795, 10433};

int main(void)
{
	/*uint16_t* bbX;
	bbX = &bb[0];
	(*bbX)++;
	(*bbX)++;
	(*bbX)++;
	(*bbX)++;
	bbX = &bb[1];
	(*bbX)++;
	(*bbX)++;
	(*bbX)++;
	(*bbX)++;
	(*bbX)++;
	(*bbX)++;
	(*bbX)++;
	(*bbX)++;*/
	/*uint16_t dlinaDe;
	int8_t sqBasDe;
	ns_line::times[0][0] = 1;
	ns_line::times[1][0] = 31561;
	ns_line::times[2][0] = 325652;
	ns_line::times[3][0] = 373646;
	ns_line::times[4][0] = 420974;

	ns_line::times[0][1] = 398067;
	ns_line::times[1][1] = 424943;
	ns_line::times[2][1] = 0;
	ns_line::times[3][1] = 0;
	ns_line::times[4][1] = 0;
	ns_line::Render(&dlinaDe, &sqBasDe);
*/
	ns_device::Init();
	ns_module::Init();
	// ===========
	sei();
    /* Replace with your application code */
	uint16_t dlina;
	int8_t sqBas;
	scr::Clear();
	scr::String(scr::SetPosition(0, 0), "Doctor_4");
	ns_rs232::String_P(PSTR("Restart system\r\n\r\n") );
	__delay_ms(1000);
	ns_menu::Init();
	ns_watchdog::enabled();
    while (1) 
    {
		ns_watchdog::reset();
		if (ns_line::Render(&dlina, &sqBas))
		{
			if ((dlina > 13000) || (dlina < 7500))
			{
				ns_vg::lenTubuStat = 10;
				ns_vg::lenTubeNew = 1;
			}
			else
			{
				ns_vg::lenTube = dlina;
				ns_vg::lenTubeNew = 1;
				ns_vg::nTube = ns_archive::idx + 1;
				ns_vg::sqBas = sqBas;
				CRITICAL_SECTION
				{
					ns_archive::Push(
						ns_clock::time[CT_YEAR],
						ns_clock::time[CT_MONTH],
						ns_clock::time[CT_DATE],
						ns_clock::time[CT_HOUR],
						ns_clock::time[CT_MINUTE],
						ns_archive::idx+1, // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
						dlina,
						sqBas
					);
				}
				//ns_watchdog::reset();
			}
		}
		ns_archive::Main();
		ns_menu::Main();
    }
}

