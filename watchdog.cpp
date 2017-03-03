/* 
* watchdog.cpp
*
* Created: 13.02.2017 15:19:11
* Author: gumerovmn
*/


#include <avr/wdt.h>
#include "watchdog.h"

#define 	WDTO_15MS   0
#define 	WDTO_30MS   1
#define 	WDTO_60MS   2
#define 	WDTO_120MS   3
#define 	WDTO_250MS   4
#define 	WDTO_500MS   5
#define 	WDTO_1S   6
#define 	WDTO_2S   7
#define 	WDTO_4S   8
#define 	WDTO_8S   9

namespace ns_watchdog
{
    void enabled()
    {
		wdt_enable(WDTO_1S);
    }
    void disabled()
    {
		wdt_disable();
    }
    void reset()
    {
	    wdt_reset();
    }

} //watchdog
