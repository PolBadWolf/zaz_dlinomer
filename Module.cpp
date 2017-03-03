/* 
* Module.cpp
*
* Created: 04.02.2017 16:46:43
* Author: User
*/


#include "Module.h"
#include "system/path.h"

namespace ns_module
{
	uint16_t mSec = 0;
	uint8_t sec = 0;
	void Init()
	{
		ns_archive::Init();
		ns_menu::Init();
		ns_line::Init();
	}
	void InterruptLcdTimer()
	{
		ns_menu::Interrupt();
		ns_archive::Interrupt();
	}
	void InterruptMainTimer()
	{
		ns_line::Interrupt();
		/*mSec++;
		if (mSec < FtTimerMain) return;
		mSec = 0;
		sec++;
		if (sec >= 60) sec = 0;
		scr::DigitZ(scr::SetPosition(0,0), 2, sec);*/
	}
	void Main()
	{
		
	}
}
