/* 
* pin.h
*
* Created: 19.10.2016 11:49:01
* Author: GumerovMN
*/


#ifndef __PIN_H__
#define __PIN_H__

#include "path.h"

template <class T>
class tc_pin
{
//variables
public:
	unsigned char tr;
	T porog;
protected:
private:
	volatile unsigned char* ddr;
	volatile unsigned char* port;
	volatile unsigned char* pin;
	unsigned mask;
	T count;
	unsigned char fl;
//functions
public:
	//tc_pin() {}
	tc_pin(volatile unsigned char* ddr, volatile unsigned char* port, volatile unsigned char* pin, unsigned char bit, T porog)
//	{ Init(ddr, port, pin, bit, porog); }
//	void Init(volatile unsigned char* ddr, volatile unsigned char* port, volatile unsigned char* pin, unsigned char bit, T porog)
	{
		this->ddr = ddr;
		this->port = port;
		this->pin = pin;
		this->mask = (1<<(bit & 0x07));
		this->porog = porog;
		*this->ddr &= ~mask;
		*this->port |= 1<<mask;
		__delay_ms(1);
		tr = fl = (*this->pin & mask)?1:0;
		if (fl)
			count = this->porog;
		else
			count = (T)0;
	}
	//~tc_pin() {}
	void Interrupt()
	{
		if (*this->pin & mask)
		{
			if ( count <  porog) count++;
			if ((count >= porog) && !fl)
				tr = fl = 1;
		}
		else
		{
			if ( count >  (T)0) count--;
			if ((count == (T)0) && fl)
				tr = fl = 0;
		}
	}
	uint8_t readSensor() { return this->fl; }
protected:
private:
	tc_pin( const tc_pin &c );
	tc_pin& operator=( const tc_pin &c );

}; //pin

#endif //__PIN_H__
