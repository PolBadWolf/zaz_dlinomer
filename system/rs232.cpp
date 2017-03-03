/* 
* rs232.cpp
*
* Created: 31.10.2016 10:07:37
* Author: GumerovMN
*/


#include "path.h"
#include "rs232.h"
//#undef  putchar

namespace ns_rs232
{
	uint8_t rxBufMask;
	uint8_t txBufMask;
	// буфер приема
	volatile uint8_t*	RxBuf = NULL;
	volatile uint8_t	RxHead;
	volatile uint8_t	RxTail;
	// буфер передачи
	volatile uint8_t*	TxBuf = NULL;
	volatile uint8_t	TxHead;
	volatile uint8_t	TxTail;
	
	rsBit bit;

	int putChar(char c, FILE *stream)
	{
		// tabWin2Avr
		//if (c >= tabWin2AvrOffset)
		//c = pgm_read_byte(& tabWin2Avr[c - tabWin2AvrOffset]);
		SendByte(c);
		return 1;
	}
	FILE* Init(rsBaud baud, rsParity parity, rsBit bit, rsBufferSize rxSize, rsBufferSize txSize)
	{
		ns_rs232::bit = bit;
		// установка скорости
		//uint16_t fBaud = (uint16_t)( ( ((uint32_t)C_Fosc) / ( ((uint32_t)16) * ((uint32_t)baud) ) ) -1 );
		uint16_t ubbrr = (uint16_t)( ( ((uint32_t)C_Fosc) / ( ((uint32_t)16) * ((uint32_t)baud) ) ) -1 );
		UBRR0H = (uint8_t)(ubbrr >> 8);
		UBRR0L = (uint8_t)(ubbrr);
		//
		UCSR0B = 0;
		uint8_t tmp = 0;
		// контроль четности
		tmp |= __port(parity).bit0;
		tmp |= __port(parity).bit1;
		// установка битности
		tmp |= __port(bit).bit0 << UCSZ00;
		tmp |= __port(bit).bit1 << UCSZ01;
		tmp |= __port(bit).bit2 << UCSZ02;
		// стоп битов 1
		tmp |= 0 << USBS0;
		UCSR0C = tmp;// | (1 << URSEL);
		// enable UART reciver & transmitte, recive & transmitte interrupt
		UCSR0B_RXEN0 = 1;
		UCSR0B_TXEN0 = 1;
		UCSR0B_RXCIE0 = 1;
		// init buffers
		RxBuf = new uint8_t[rxSize];
		rxBufMask = rxSize - 1;
		TxBuf = new uint8_t[txSize];
		txBufMask = txSize - 1;

		return fdevopen(putChar, NULL);
	}
	// ==========================================
	ISR(USART0_RX_vect)
	//ISR(USART_RXC_vect)
	{
		uint8_t tmpHead = (RxHead + 1) & rxBufMask;
		if (tmpHead != RxTail)
		{
			/*#if defined(b)
			RxBuf[RxHead] = UDR & 0x1f;
			#elif defined(RS_BITS_6)
			RxBuf[RxHead] = UDR & 0x3f;
			#elif defined(RS_BITS_7)
			RxBuf[RxHead] = UDR & 0x7f;
			#else*/
			RxBuf[RxHead] = UDR0;
			//#endif
			RxHead = tmpHead;
		}
	}
	// ==========================================
	ISR(USART0_UDRE_vect)
	{
		if (TxHead == TxTail)
			UCSR0B_UDRIE0 = 0;
		else
		{
			UCSR0B_TXB80 = 1;
			UDR0 = TxBuf[TxTail];
			TxTail = (TxTail + 1) & txBufMask;
		}
	}
	// ==========================================
	uint8_t ReadByte(uint8_t *byte)
	{
		CRITICAL_SECTION
		{
			if (RxHead == RxTail)
				return 0;
			else
			{
				*byte = RxBuf[RxTail];
				RxTail = (RxTail + 1) & rxBufMask;
				return 1;
			}
		}
		return 0;
	}
	// ==========================================
	uint8_t ReadByte(char *byte)
	{
		CRITICAL_SECTION
		{
			if (RxHead == RxTail)
			return 0;
			else
			{
				*byte = RxBuf[RxTail];
				RxTail = (RxTail + 1) & rxBufMask;
				return 1;
			}
		}
		return 0;
	}
	// ==========================================
	uint8_t SendByte(uint8_t *byte)
	{
		CRITICAL_SECTION
		{
			uint8_t tmpHead = (TxHead + 1) & txBufMask;
			if (tmpHead == TxTail)
				return 0;
			else
			{
				TxBuf[TxHead] = *byte;
				TxHead = tmpHead;
				UCSR0B_UDRIE0 = 1;
				//UDR = *byte;
				return 1;
			}
		}
		return 0;
	}
	// ==========================================
	uint8_t SendByte(uint8_t byte)
	{
		CRITICAL_SECTION
		{
			uint8_t tmpHead = (TxHead + 1) & txBufMask;
			if (tmpHead == TxTail)
			return 0;
			else
			{
				TxBuf[TxHead] = byte;
				TxHead = tmpHead;
				UCSR0B_UDRIE0 = 1;
				//UDR = byte;
				return 1;
			}
		}
		return 0;
	}
	// ==================================================
	void String(const char *stroka)
	{
		char sim;
		while (true)
		{
			sim = *stroka;
			if (sim == 0) break;
			SendByte(sim);
			stroka++;
		}
	}
	// ==================================================
	void String_P(const char *stroka)
	{
		char sim;
		while (true)
		{
			sim = pgm_read_byte(stroka);
			if (sim == 0) break;
			SendByte(sim);
			stroka++;
		}
	}
	// ==================================================
	void Digit(uint8_t len, int32_t digit)
	{
		if (digit < 0)
		{
			SendByte('-');
			DigitZ(len-1, -digit);
			return;
		}
		char sss[16];
		sss[len+1] = 0;
		//len--;

		uint8_t low = digit % 10;
		sss[len] = '0' + low;
		digit /= 10;
		while(len-->0)
		{
			low = digit % 10;
			if ( (low==0) && (digit==0) )
				sss[len] = ' ';
			else
				sss[len] = '0' + low;
			digit /= 10;
		}
		String(sss);
	}
	void DigitZ(uint8_t len, int32_t digit)
	{
		if (digit < 0)
		{
			SendByte('-');
			len--;
			digit = -digit;
		}
		char sss[16];
		sss[len] = 0;
		//len--;

		uint8_t low;// = digit % 10;
		//sss[len] = '0' + low;
		//digit /= 10;
		//len--;
		while(len-->0)
		{
			low = digit % 10;
			sss[len] = '0' + low;
			digit /= 10;
		}
		String(sss);
	}
	// ==================================================
	char Tetra(uint8_t dat)
	{
		dat &= 0x0f;
		if (dat>9)
		dat += 7;
		dat += '0';
		return dat;
	}
	// ==================================================
	void Hex(uint8_t dig)
	{
		SendByte( Tetra( (*((volatile BYTE_TETRA*)(&dig))).High ) );
		SendByte( Tetra( (*((volatile BYTE_TETRA*)(&dig))).Low ) );
	}
	// ==================================================
}
