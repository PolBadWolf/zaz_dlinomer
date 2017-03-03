/*
 * path.h
 *
 * Created: 27.08.2016 19:56:54
 *  Author: User
 */ 

#ifndef PATH_H_
#define PATH_H_

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <util/atomic.h>
#include <stddef.h>
#include <stdio.h>

#include "path_mega128.h"

#define C_Fosc 16000000

typedef struct
{
	unsigned bit0:1;
	unsigned bit1:1;
	unsigned bit2:1;
	unsigned bit3:1;
	unsigned bit4:1;
	unsigned bit5:1;
	unsigned bit6:1;
	unsigned bit7:1;
} BYTE_BITFIELD;

typedef struct
{
	unsigned Low:4;
	unsigned High:4;
} BYTE_TETRA;

#define __delay_ms(__ms) __builtin_avr_delay_cycles(((C_Fosc) / 1e3) * __ms)
#define __delay_us(__us) __builtin_avr_delay_cycles(((C_Fosc) / 1e6) * __us)

#define  CRITICAL_SECTION ATOMIC_BLOCK(ATOMIC_RESTORESTATE)

#define __port(x) (*((volatile BYTE_BITFIELD*)(&x)))
void *operator new(size_t);
void *operator new[](size_t);

void operator delete(void *);
void operator delete[](void *);
//*****************************
template <class ST>
class integ
{
	// Var
public:
	bool tr;
//protected:
//private:
	ST count;
	ST countMax;
	bool tr_int;
	// Fn
public:
	void SetMax(ST max)
	{
		CRITICAL_SECTION
		{
			countMax = max;
			if (count>countMax)
			{
				count = countMax;
				tr = tr_int = true;
			}
		}
	}
	integ()
	{
		count = (ST)0;
		countMax = (ST)40;
		tr = tr_int = false;
	}
	integ(ST max)
	{
		count = (ST)0;
		countMax = max;
		tr = tr_int = false;
	}
	unsigned char Cirle(unsigned char s)
	{
		CRITICAL_SECTION
		{
			if (s)
			{
				if (count>(ST)0) count--;
				if ((count==(ST)0) && tr_int)
				tr = tr_int = false;
			}
			else
			{
				if (count<countMax) count++;
				if ((count>=countMax) && !tr_int)
				{
					tr = tr_int = true;
					count = countMax;
				}
			}
			return tr_int;// ? 0 : 0xff;
		}
		return 0;
	}
protected:
private:
};
// ------------------------------------------
enum rsBaud
{
	baud300     = 300
	,baud600    = 600
	,baud1200   = 1200
	,baud2400   = 2400
	,baud4800   = 4800
	,baud9600   = 9600
	,baud19200  = 19200
	,baud38400  = 38400
	,baud57600  = 57600
	,baud115200 = 115200
};
enum rsParity
{
	DISABLE = 0,
	EVEN = 2,
	ODD = 3
};
enum rsBit
{
	bit5 = 0,
	bit6,
	bit7,
	bit8
};
enum rsBufferSize
{
	size2	= 2,
	size4	= 4,
	size8	= 8,
	size16	= 16,
	size32	= 32,
	size64	= 64,
	size128	= 128,
	size256	= 256
};
extern const char tabWin2Avr[];
#define tabWin2AvrOffset 128
// ------------------------------------------
//*****************************
#include "Device.h"
#include "../Module.h"

#endif /* PATH_H_ */
