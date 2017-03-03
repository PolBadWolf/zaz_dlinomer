/* 
* key4.cpp
*
* Created: 13.10.2016 15:08:42
* Author: GumerovMN
*/
#include "path.h"
#include "key4.h"

typedef struct
{
	//unsigned free:4;
	unsigned data:4;
} BYTE_KEY_DATA;

#define KEY_DDR		(*((volatile BYTE_KEY_DATA*)(&DDRG))).data
#define KEY_RD		(*((volatile BYTE_KEY_DATA*)(&PING))).data
#define KEY_WR		(*((volatile BYTE_KEY_DATA*)(&PORTG))).data

#define TIME_LINE 20
#define TIME_STAB 40
#define TIME_PUSH 80
//#define TIME_LINE 100
//#define TIME_STAB 120
//#define TIME_PUSH 400
#define TIME_AUTO 2000
#define TIME_AUTX 200
#define TIME_AUT1 (TIME_AUTO-TIME_AUTX)
#define TIME_AUT2 (20000 / TIME_AUTX)

#define  KEY_BUFFER 16

// default constructor
tc_key4::tc_key4()
{
	// настройка порта
	KEY_DDR &= ~(0x0f);
	// внутренние резисторы
	KEY_WR = 0x0f;
	//
	xx = 0xff;
	countL1 = countL2 = countAut2 = 0;
	oldKey1 = 255;
	oldKey2 = 0;
	head = tail = 0;
	autoRepeat = true;
	buffer = new unsigned char[KEY_BUFFER];
	// линии помехо защиты
	line1 = new integ<unsigned char>(TIME_LINE);
	line2 = new integ<unsigned char>(TIME_LINE);
	line3 = new integ<unsigned char>(TIME_LINE);
	line4 = new integ<unsigned char>(TIME_LINE);
} //key4

// default destructor
tc_key4::~tc_key4()
{
} //~key4

void tc_key4::Interrupt()
{
	//
	unsigned char s = KEY_RD;
	keyX = 0;
	keyX |= line1->Cirle(__port(s).bit3) << 0;
	keyX |= line2->Cirle(__port(s).bit2) << 1;
	keyX |= line3->Cirle(__port(s).bit1) << 2;
	keyX |= line4->Cirle(__port(s).bit0) << 3;
	//
	if (oldKey1!=keyX)
	{
		if (countL1>0)	countL1--;
		if (countL1==0)
		{
			oldKey1 = keyX;
		}
	}
	else
	{
		if (countL1<TIME_STAB) countL1++;
		if (countL1>=TIME_STAB)
		{
			countL1 = TIME_STAB;
			Select();
		}
	}
}
void tc_key4::Select()
{
	if (oldKey1 == 0)
	{
		if (oldKey2>0)
		{
			if (((countL2 <= TIME_PUSH) && !autoRepeat) || autoRepeat)
//			if (((countL2 <= TIME_PUSH) && !autoRepeat) || ((countL2 >= TIME_PUSH) && autoRepeat))
				Push(oldKey2);
		}
			oldKey2 = 0;
			countL2 = 0;
			countAut2 = 0;
	}
	else
	{
		if ( Nbit(oldKey1) > Nbit(oldKey2) )
		{
			oldKey2 = oldKey1;
		}
		//
		if (oldKey1 == oldKey2)
		{
			if (countL2 <= TIME_AUTO) countL2++;
			if (autoRepeat)
			{
				if (countL2 >= TIME_AUTO)
				{
					Push(oldKey2);
					if (countAut2 < TIME_AUT2)
					{
						countAut2++;
						countL2 = TIME_AUT1;
					}
					else
					{
						countL2 = TIME_AUTO - 2;
					}
					
				}
			}
			else
			{
				if (countL2 == TIME_PUSH)
				Push(oldKey2);
			}
		}
		else
		{
			countL2 = 0;
			countAut2 = 0;
		}
	}
}
unsigned char tc_key4::Nbit(unsigned char dat)
{
	unsigned char n = 0;
	for (unsigned char i=0; i<4; i++)
	{
		if ( dat & (1<<i)) n++;
	}
	return n;
}

unsigned char tc_key4::InKey()
{
	return xx;
}

unsigned char tc_key4::norm(unsigned char idx)
{
	while(idx >= KEY_BUFFER)
		idx -= KEY_BUFFER;
	return idx;
}

void tc_key4::Push(unsigned char dat)
{
	CRITICAL_SECTION
	{
		unsigned char adr = norm(head+1);
		if (tail == adr)
			return;
		buffer[head] = dat;
		head = adr;
	}
}

unsigned char tc_key4::Read(unsigned char* key)
{
	unsigned char k = 0;
	CRITICAL_SECTION
	{
		if (tail == head)
		{
			*key = 0;
			return k;
		}
		*key = buffer[tail];
		tail = norm(tail+1);
		if (*key == KEY1) k = 1;
		else
		if (*key == KEY2) k = 2;
		else
		if (*key == KEY3) k = 3;
		else
		if (*key == KEY4) k = 4;
		else k = 5;
		return k;
	}
	return 0;
}

//void (* const tc_key4::FN[][2])(unsigned char par) =
void (* const tc_key4::FN[])(unsigned char par) =
{
};
