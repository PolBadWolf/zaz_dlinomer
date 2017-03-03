/* 
* Archive.h
*
* Created: 07.02.2017 11:56:23
* Author: gumerovmn
*/


#ifndef __ARCHIVE_H__
#define __ARCHIVE_H__

#define  ARCHIVE_LEN 300
#if (ARCHIVE_LEN < 256)
#define  TYPE_IDX	uint8_t
#define  IDX_ee_read eeprom_read_byte
#define  IDX_ee_update eeprom_update_byte
#else
#define  TYPE_IDX	uint16_t
#define  IDX_ee_read eeprom_read_word
#define  IDX_ee_update eeprom_update_word
#endif

typedef struct
{
	uint8_t year;
	uint8_t mounth;
	uint8_t date;
	uint8_t hour;
	uint8_t minute;
	uint16_t nTube;
	uint16_t len;
	uint8_t ok;
	 int8_t sqBas;
} archiveStruct;

namespace ns_archive
{
	extern EEMEM archiveStruct archive[ARCHIVE_LEN];
	extern TYPE_IDX idx;
	extern TYPE_IDX len;
	extern EEMEM TYPE_IDX idxEE;
	extern EEMEM TYPE_IDX lenEE;
	void Init();
	void Interrupt();
	void Main();
	void Push(uint8_t year, uint8_t mounth, uint8_t date, uint8_t hour, uint8_t minute, uint16_t nTube, uint16_t lenTube, int8_t sqBas);
	TYPE_IDX Summ(uint32_t* lenSumma);
	void SummZero();
}


#endif //__ARCHIVE_H__
