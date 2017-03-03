/* 
* Archive.cpp
*
* Created: 07.02.2017 11:56:20
* Author: gumerovmn
*/


#include "system/path.h"
#include "Archive.h"
#include <avr/eeprom.h>

namespace ns_archive
{
	EEMEM archiveStruct archive[ARCHIVE_LEN];
	EEMEM TYPE_IDX idxEE;
	TYPE_IDX idx;
	EEMEM TYPE_IDX lenEE;
	TYPE_IDX len;
	uint16_t acp0;
	uint8_t acp0_new = 0;
	
	void Init()
	{
		idx = IDX_ee_read(&idxEE);
		len = IDX_ee_read(&lenEE);
		if ( (idx >= ARCHIVE_LEN) || (len >= ARCHIVE_LEN) )
		{
			idx = 0;
			len = 0;
		}
		IDX_ee_update(&idxEE, idx);
		IDX_ee_update(&lenEE, len);
		
	}
	
	void Push(uint8_t year, uint8_t mounth, uint8_t date, uint8_t hour, uint8_t minute, uint16_t nTube, uint16_t lenTube, int8_t sqBas)
	{
		archiveStruct m;
		m.year = year;
		m.mounth = mounth;
		m.date = date;
		m.hour = hour;
		m.minute = minute;
		m.nTube = nTube;
		m.len = lenTube;
		m.ok = 1;
		m.sqBas = sqBas;
		//
		eeprom_write_block(&m, &archive[idx], sizeof(archiveStruct));
		//
		idx++;
		if (idx >= ARCHIVE_LEN) idx = 0;
		if (len < ARCHIVE_LEN) len++;
				CRITICAL_SECTION
				{
					IDX_ee_update(&idxEE, idx);
					IDX_ee_update(&lenEE, len);
					//__delay_ms(10000);
				}
	}
	
	TYPE_IDX Summ(uint32_t* lenSumma)
	{
		uint32_t summa = 0;
		TYPE_IDX n = 0;
		TYPE_IDX id = idx;
		TYPE_IDX ln = len;
		while (ln > 0)
		{
			if (id == 0) id = ARCHIVE_LEN;
			id--;
			ln--;
			if ( eeprom_read_byte(&archive[id].ok) == 1 )
			{
				summa += eeprom_read_word(&archive[id].len);
				n++;
			}
		}
		CRITICAL_SECTION
		{
			*lenSumma = summa;
		}
		return n;
	}
	
	void SummZero()
	{
		idx = 0;
		len = 0;
		IDX_ee_update(&idxEE, idx);
		IDX_ee_update(&lenEE, len);
	}
	
	typedef struct
	{
		unsigned low:8;
		unsigned high:2;
	} structACP;
	
	void Interrupt()
	{
		uint16_t acp = 0;
		CRITICAL_SECTION
		(*((volatile structACP*)(&acp))).low = ADCL;
		(*((volatile structACP*)(&acp))).high = ADCH;
		acp0 = acp;
		acp0_new = 1;
		ADMUX = 0;
		ADCSR = 0xd7;
	}
	
	void Main()
	{
		uint16_t acp;
		uint8_t newd = 0;
		CRITICAL_SECTION
		{
			if (acp0_new)
			{
				acp = acp0;
				acp0_new = 0;
				newd = 1;
			}
		}
		if (newd)
		{
			newd = 0;
			if ( acp < ( ((uint16_t)20) * 1023 / 50) )
			{
				CRITICAL_SECTION
				{
					IDX_ee_update(&idxEE, idx);
					IDX_ee_update(&lenEE, len);
					//__delay_ms(10000);
				}
					//scr::DigitZ(scr::SetPosition(8, 1), 8, acp);
					//while(true) {__delay_ms(1);}
			}
		}
	}
}
