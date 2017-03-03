/*
 * vg.h
 *
 * Created: 15.12.2016 14:36:39
 *  Author: gumerovmn
 */ 


#ifndef VG_H_
#define VG_H_

#include "system/path.h"

namespace ns_vg
{
	extern uint16_t lenTube;
	extern uint16_t nTube;
	extern int16_t  lenTubuExl;
	extern uint8_t lenTubeNew;
	extern uint8_t lenTubuStat;
	extern int8_t  sqBas;
	extern EEMEM uint16_t eeDistance[];
	//extern int8_t   eeDistanceCor[2][3] EEMEM;
	extern EEMEM uint16_t eeProtect;
	extern EEMEM uint16_t eeInteral;
	extern EEMEM uint8_t eeCorSpeed;
	extern uint8_t exN;
}


#endif /* VG_H_ */