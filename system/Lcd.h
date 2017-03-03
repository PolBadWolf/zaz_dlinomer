/* 
* Lcd.h
*
* Created: 04.02.2017 12:04:57
* Author: User
*/


#ifndef __LCD_H__
#define __LCD_H__

namespace ns_lcd
{
	// var
	// указатель записи
	extern unsigned char pos;
	// с миганием
	extern volatile bool flicker;
	uint8_t Stolbcov();
	// ---------------------------------------
	// Fn
	FILE* Init(uint8_t stolbcov);
	void Interrupt();
	void Clear();
	void PutChar(uint8_t *pos, char sim);
	void PutChar(uint8_t pos, char sim);
	uint8_t SetPosition(uint8_t x, uint8_t y);
	void String(uint8_t *pos, const char *stroka);
	void String(uint8_t pos, const char *stroka);
	void String_P(uint8_t *pos, const char *stroka);
	void String_P(uint8_t pos, const char *stroka);
	void DigitZ(uint8_t *pos, uint8_t len, int32_t digit);
	void DigitZ(uint8_t pos, uint8_t len,  int32_t digit);
	void Digit(uint8_t *pos, uint8_t len,  int32_t digit);
	void Digit(uint8_t pos, uint8_t len,   int32_t digit);
	char Tetra(uint8_t dat);
	void Hex(uint8_t* pos, uint8_t dig);
	void Hex(uint8_t pos, uint8_t dig);
}


#endif //__LCD_H__
