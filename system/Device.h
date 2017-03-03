/* 
* Device.h
*
* Created: 04.02.2017 11:51:20
* Author: User
*/


#ifndef __DEVICE_H__
#define __DEVICE_H__

#include "LcdKey_Timer.h"
#include "Lcd.h"
#include "key4.h"
#include "pin.h"
#include "timer2.h"
#include "rs232.h"
#include "Clock.h"


namespace ns_device
{
	void Init();
}

extern FILE* lcdStdOut;
extern FILE* rs232StdOut;
extern tc_key4*	key4;

#define scr ns_lcd
#define rsTele ns_rs232

#endif //__DEVICE_H__
