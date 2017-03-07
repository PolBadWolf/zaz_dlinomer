/*
 * vg.cpp
 *
 * Created: 15.12.2016 14:36:22
 *  Author: gumerovmn
 */ 

#include "vg.h"

#include "system/path.h"


namespace ns_vg
{
	uint16_t lenTube = 125;
	uint16_t nTube = 0;
	int16_t  lenTubuExl = 0;
	uint8_t  lenTubeNew = 0;
	uint8_t  lenTubuStat = 0;
	int8_t   sqBas;
	EEMEM uint16_t eeDistance[5] = {0, 790, 9382, 10753, 12148};
	EEMEM uint16_t eeProtect = 1000;
	EEMEM uint16_t eeInteral = (uint16_t)(((uint32_t)10) * ((uint32_t)FtTimerMain) / ((uint32_t)1000));
	EEMEM uint8_t eeCorSpeed = 0;
	EEMEM uint8_t eeFlSq2 = 1;
	uint8_t exN;
}
