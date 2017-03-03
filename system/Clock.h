/* 
* Clock.h
*
* Created: 05.02.2017 15:24:46
* Author: User
*/


#ifndef __CLOCK_H__
#define __CLOCK_H__

// ��� ���� ������� � ������� clock_data[]
#define CC_SECOND          0
#define CC_MINUTE          1
#define CC_HOUR            2
#define CC_DAY             3
#define CC_DATE            4
#define CC_MONTH           5
#define CC_YEAR            6
#define CC_MODE            7

// ��� ���� ������� � ������� time[]
#define CT_SECOND          0
#define CT_MINUTE          1
#define CT_HOUR            2
#define CT_DAY             3
#define CT_DATE            4
#define CT_MONTH           5
#define CT_YEAR            6

namespace ns_clock
{
	extern const uint8_t nDayMountx[4][13];
	
	extern uint8_t clock_data[8]; // ������ �������� �����-���������
	extern uint8_t time[7];       // ������ ���� �������
	//void ClockBuffWrite();	        // ������ ��������� ���� ������� �� ������
	//void ClockBuffRead();             // ������ ��������� ���� ������� � �����
	extern uint16_t tik_count;
	extern uint8_t tik_cn;
	extern uint8_t tik;
	void RefreshTime();					// �������� �������� ������� � ������� time[]
	void Init();
	void Interrupt();
	//
	void RefSav();
	void SetYear(uint8_t year);
	void SetMonth(uint8_t mount);
	void SetDate(uint8_t date);
	void SetHour(uint8_t hour);
	void SetMinute(uint8_t minute);
	void SetZeroSecond();
	void Update();
}


#endif //__CLOCK_H__
