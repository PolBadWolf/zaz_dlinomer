/* 
* Lcd.cpp
*
* Created: 04.02.2017 12:04:57
* Author: User
*/


#include "path.h"
#include "Lcd.h"

#define FlashMaxCount  1024

typedef struct// BYTE_LCD_DATAs
{
	unsigned data:4;
} BYTE_LCD_DATA;
#define LCD_D_DDR	(*((volatile BYTE_LCD_DATA*)(&DDRA))).data
#define LCD_D		(*((volatile BYTE_LCD_DATA*)(&PORTA))).data
//#define LCD_D_PIN	(*((volatile BYTE_LCD_DATA*)(&PINA))

#define LCD_EI_DDR		__port(DDRA).bit4
#define LCD_EI			__port(PORTA).bit4

#define LCD_RS_DDR		__port(DDRA).bit5
#define LCD_RS			__port(PORTA).bit5

namespace ns_lcd
{
	// var
	const uint8_t c_strok = 2;
	uint8_t c_stolbcov = 16;
	uint8_t maxPos = c_strok*c_stolbcov;
	// *******************************************
	#define tabDecoderOffset 128
	const char tabDecoder[] PROGMEM =
	{
		200,	201,	202,	203,	215,	216,	217,	218,
		219,	220,	237,	253,	238,	239,	240,	241,
		242,	244,	204,	205,	206,	207,	208,	209,
		210,	211,	212,	213,	214,	221,	222,	223,
		231,	232,	233,	234,	235,	236,	243,	245,
		162,	246,	247,	248,	249,	250,	251,	252,
		254,	255,	32,		32,		32,		32,		32,		32,
		181,	32,		32,		32,		32,		32,		32,		32,
		65,		160,	66,		161,	224,	69,		163,	164,
		165,	166,	75,		167,	77,		72,		79,		168,
		80,		67,		84,		169,	170,	88,		225,	171,
		172,	226,	173,	174,	98,		175,	176,	177,
		97,		178,	179,	180,	227,	101,	182,	183,
		184,	185,	186,	187,	188,	189,	111,	190,
		112,	99,		191,	121,	228,	120,	229,	192,
		193,	230,	194,	195,	196,	197,	198,	199
	};
	const uint8_t symbol_work[16] PROGMEM =
	{ 0x06, 0x09, 0x09, 0x06, 0x00, 0x00, 0x00, 0x00,
	  0x06, 0x0f, 0x0f, 0x06, 0x00, 0x00, 0x00, 0x00 };

	// *******************************************
	// копия экрана
	char*	screen;
	uint32_t	scrFlash[c_strok]; // 32 бита
	uint32_t*	ptrScrFlash = NULL;
	// указатель записи
	uint8_t		pos;
	uint16_t	scrFlashCount;
	// указатель позитции для регенерации экрана
	volatile uint8_t	indxRefresh;
	// с миганием
	volatile bool flicker;
	// ------------------------
	// local porototype
	void SendCommand(uint8_t data);
	void SendData(uint8_t data);
	void Send(uint8_t data);
	int	 putChar(char c, FILE *stream);
	void NormalizePos(uint8_t *poz);
	uint8_t SetPosition(uint8_t x, uint8_t y);
	void FlashFn(uint8_t* pos, volatile bool* onOff);
	void Clear();
	void PutChar(uint8_t *pos, char sim);
	void PutChar(uint8_t pos, char sim);
	char Tetr(uint8_t dat);
	uint8_t Stolbcov() {return c_stolbcov;}
	// ==========================================
	// fn
	FILE* Init(uint8_t stolbcov)
	{
		// region настройка портов
		LCD_D_DDR	= 0x0f;
		LCD_D		= 0x0f;
		LCD_EI_DDR	= 1;
		LCD_EI		= 1;
		LCD_RS_DDR	= 1;
		LCD_RS		= 1;
		__delay_ms(500);
		// настройка переменных
		c_stolbcov = stolbcov;
		maxPos = c_strok*c_stolbcov;
		screen = new char[c_strok*c_stolbcov];
		indxRefresh = 0;
		ptrScrFlash = &scrFlash[0];
		pos = 0;
		scrFlash[0] = 0;
		scrFlash[1] = 0;
		scrFlashCount = 0;
		flicker = 0;

		// настройка контроллера lcd
		SendCommand(0x28);	//4-ех разрядная шина, 2 строки
		__delay_ms(2);
		SendCommand(0x28);	//4-ех разрядная шина, 2 строки
		__delay_ms(2);
		SendCommand (0xc);  //0b00001111 - дисплей, курсор, мерцание включены
		SendCommand (0x1);  //0b00000001 - очистка дисплея
		__delay_ms(2);
		SendCommand (0x6);  //0b00000110 - курсор движется вправо, сдвига нет
		SendCommand (0x2);  //0b00000001 - очистка дисплея
		__delay_ms(2);
		SendCommand(0x80);
		__delay_ms(2);
		// ================================
		// load simbols
		SendCommand(0x40);
		__delay_ms(1);
		for (unsigned char i=0;i<16;i++)
		{
			SendData(pgm_read_byte(&symbol_work[i]));
			__delay_us(15);
		}
		SendCommand(0x80);
		__delay_ms(1);
		// ================================
		Clear();
		return fdevopen(putChar, NULL);
	}
	// ==================================================
	void SendCommand(uint8_t data)
	{
		CRITICAL_SECTION
		{
			//__delay_us(50);
			LCD_RS = 0;
			__delay_us(50);
			Send(data);
		}
	}
	// ==================================================
	void SendData(uint8_t data)
	{
		CRITICAL_SECTION
		{
			//__delay_ms(50);
			LCD_RS = 1;
			__delay_us(50);
			Send(data);
		}
	}
	// ==================================================
	void Send(uint8_t data)
	{
		LCD_D = ((data>>4) & 0x0f);
		LCD_EI = 1;
		__builtin_avr_delay_cycles(10);
		LCD_EI = 0;
		__builtin_avr_delay_cycles(10);
		LCD_D = (data & 0x0f);
		LCD_EI = 1;
		__builtin_avr_delay_cycles(10);
		LCD_EI = 0;
		__builtin_avr_delay_cycles(10);
	}
	// ==================================================
	void Clear()
	{
		for (unsigned char i = 0; i < maxPos; i++)
		screen[i] = ' ';
		scrFlash[0] = 0;
		scrFlash[1] = 0;
		pos = 0;
	}
	// ==================================================
	void Interrupt()
	{
		scrFlashCount++;
		if (scrFlashCount>=FlashMaxCount)
		scrFlashCount = 0;

		indxRefresh++;
		if (indxRefresh >= maxPos)
		{
			indxRefresh = 0;
			ptrScrFlash = &scrFlash[0];
			SendCommand(0x80);
		}
		if (indxRefresh==c_stolbcov)
		{
			ptrScrFlash = &scrFlash[1];
			SendCommand(0xc0);
		}
		__delay_us(50);
		
		unsigned char tmp = screen[indxRefresh];

		if (tmp >= tabDecoderOffset)
		tmp = pgm_read_byte(& tabDecoder[tmp - tabDecoderOffset]);
		
		{	// мерцание
			if ( ((*ptrScrFlash) & (((uint32_t)1)<<(indxRefresh%c_stolbcov))) && (scrFlashCount < (FlashMaxCount/2)) )
			tmp = ' ';
		}
		
		SendData(tmp);
	}
	// ==================================================
	int putChar(char c, FILE *stream)
	{
		uint8_t *p;
		p = &pos;
		PutChar(p, c);
		return 1;
	}
	// ==================================================
	void NormalizePos(uint8_t *poz)
	{
		while (*poz >= maxPos)
		{
			*poz = *poz - maxPos;
		}
	}
	// ==================================================
	uint8_t SetPosition(uint8_t x, uint8_t y)
	{
		unsigned char pos = y*c_stolbcov + x;
		NormalizePos(&pos);
		return pos;
	}
	// ==================================================
	void PutChar(uint8_t *pos, char sim)
	{
		NormalizePos(pos);
		FlashFn(pos, &flicker);

		screen[*pos] = sim;
		(*pos)++;
	}
	// ==================================================
	void PutChar(uint8_t pos, char sim)
	{
		PutChar(&pos, sim);
	}
	// ==================================================
	void FlashFn(uint8_t* pos, volatile bool* onOff)
	{
		if (*onOff)
		scrFlash[(*pos)/c_stolbcov] |=  (((uint32_t)1)<<((*pos)%c_stolbcov));
		else
		scrFlash[(*pos)/c_stolbcov] &= ~(((uint32_t)1)<<((*pos)%c_stolbcov));
	}
	// ==================================================
	void String(uint8_t *pos, const char *stroka)
	{
		uint8_t max = maxPos;
		while ((*stroka)>0)
		{
			NormalizePos(pos);
			FlashFn(pos, &flicker);
			screen[*pos] = (*stroka);
			(*pos)++;
			stroka++;
			if (--max == 0)
			{
				break;
			}
		}
	}
	// ==================================================
	void String(uint8_t pos, const char *stroka)
	{
		String(&pos, stroka);
	}
	// ==================================================
	void String_P(uint8_t *pos, const char *stroka)
	{
		uint8_t max = maxPos;
		char sim;
		while ((sim = pgm_read_byte(stroka))>0)
		{
			NormalizePos(pos);
			FlashFn(pos, &flicker);
			// screen[*pos] = (*stroka);
			screen[*pos] = sim;
			(*pos)++;
			stroka++;
			if (--max==0)
			{
				break;
			}
		}
	}
	// ==================================================
	void String_P(uint8_t pos, const char *stroka)
	{
		String_P(&pos, stroka);
	}
	// ==================================================
	void DigitZ(uint8_t *pos, uint8_t len, int32_t digit)
	{
		if (digit < 0)
		{
			PutChar(pos, '-');
			len--;
			digit = -digit;
		}
		*pos += len;
		NormalizePos(pos);
		int8_t poz = *pos - 1;
		char low;
		while(len-->0)
		{
			low = digit % 10;
			if (poz<0)	poz += (c_strok*c_stolbcov);
			FlashFn((uint8_t*)&poz, &flicker);
			screen[poz--] = '0' + low;
			digit /= 10;
		}
	}
	// ==================================================
	void DigitZ(uint8_t pos, uint8_t len, int32_t digit)
	{
		DigitZ(&pos, len, digit);
	}
	// ==================================================
	void Digit(uint8_t *pos, uint8_t len, int32_t digit)
	{
		if (digit < 0)
		{
			DigitZ(pos, len, digit);
			return;
		}
		*pos += len;
		NormalizePos(pos);
		int8_t poz = *pos - 1;
		char low = digit % 10;
		if (poz<0)	poz += (c_strok*c_stolbcov);
		FlashFn((uint8_t*)&poz, &flicker);
		screen[poz--] = '0' + low;
		digit /= 10;
		len--;
		while(len-->0)
		{
			low = digit % 10;
			if (poz<0)	poz += (c_strok*c_stolbcov);
			FlashFn((uint8_t*)&poz, &flicker);
			if ( (low==0) && (digit==0) )
			screen[poz] = ' ';
			else
			screen[poz] = '0' + low;
			poz--;
			digit /= 10;
		}
	}
	// ==================================================
	void Digit(uint8_t pos, uint8_t len, int32_t digit)
	{
		Digit(&pos, len, digit);
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
	void Hex(uint8_t* pos, uint8_t dig)
	{
		FlashFn((uint8_t*)pos, &flicker);
		screen[*pos] = Tetra( (*((volatile BYTE_TETRA*)(&dig))).High );
		(*pos)++;
		FlashFn((uint8_t*)pos, &flicker);
		screen[*pos] = Tetra( (*((volatile BYTE_TETRA*)(&dig))).Low );
		(*pos)++;
	}
	// ==================================================
	void Hex(uint8_t pos, uint8_t dig)
	{
		Hex(&pos, dig);
	}
	// ==================================================
}
