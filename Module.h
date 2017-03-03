/* 
* Module.h
*
* Created: 04.02.2017 16:46:44
* Author: User
*/


#ifndef __MODULE_H__
#define __MODULE_H__

#include "watchdog.h"
#include "vg.h"
#include "menu.h"
#include "line.h"
#include "Archive.h"

namespace ns_module
{
	void Init();
	void InterruptLcdTimer();
	void InterruptMainTimer();
	void Main();
}



#endif //__MODULE_H__
