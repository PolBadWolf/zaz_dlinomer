/*
 * line.cpp
 *
 * Created: 13.01.2017 19:00:07
 *  Author: User
 */ 

 #include "system/path.h"
 #include "line.h"

namespace ns_line
{
	tc_pin<uint16_t>* sensors[5];		// массив датчиков
	//#define countType uint32_t
	uint32_t count = 0;
	volatile uint32_t times[5][2];
	uint32_t timeEnd = 0;
	uint32_t timeCrh = 0;
	// время краха в секундах
	#define TIME_CRASH ((uint32_t)60 * FtTimerMain)
	// защитный интервал 2000 мСек
	//#define PROTECT_INTERVAL (((uint32_t)500) * FtTimerMain / ((uint32_t)1000))
	uint16_t protect_interval = (((uint32_t)1000) * FtTimerMain / ((uint32_t)1000));
	uint8_t mode = 1;

//	uint16_t distance[5] = {0, 750, 8500, 10000, 11500};
	int16_t distance[5] = {0, 500, 2000, 3000, 4000};
	uint16_t protect[5] = {0, 0, 0, 0, 0};
	void Init()
	{
		//uint16_t k = (uint16_t)(((uint32_t)10) * ((uint32_t)FtTimerMain) / ((uint32_t)1000));
		uint16_t k = eeprom_read_word(&ns_vg::eeInteral);
		sensors[0] = new tc_pin<uint16_t>(&DDRC, &PORTC, &PINC, 0, k);
		sensors[1] = new tc_pin<uint16_t>(&DDRC, &PORTC, &PINC, 1, k);
		sensors[2] = new tc_pin<uint16_t>(&DDRC, &PORTC, &PINC, 2, k);
		sensors[3] = new tc_pin<uint16_t>(&DDRC, &PORTC, &PINC, 3, k);
		sensors[4] = new tc_pin<uint16_t>(&DDRC, &PORTC, &PINC, 4, k);
		mode = 1;
		if ( (ns_archive::len == 0) && (eeprom_read_word(&ns_archive::lenEE) == 0) )
		{
			ns_vg::nTube = 0;
		}
		else
		{
			int16_t corCurTube_idx = ns_archive::idx;
			archiveStruct corCurTube_cell;
			if (corCurTube_idx == 0) corCurTube_idx = ARCHIVE_LEN - 1;
			else corCurTube_idx--;
			eeprom_read_block(&corCurTube_cell, &ns_archive::archive[corCurTube_idx], sizeof(archiveStruct));
			ns_vg::nTube = corCurTube_cell.nTube;
		}
	}
	void Interrupt()
	{
		// чтение датчиков & сброс защитного интервала
		for (uint8_t i=0; i<5; i++)
		{
			sensors[i]->tr = 10;
			sensors[i]->Interrupt();
			if (protect[i] > 0) protect[i]--;
		}
		if (mode == 0)
		{	// render
			return;
		}
		if (mode == 1)
		{	// wait free
			uint8_t fl = 1;
			//for (uint8_t i=0; i<2; i++) // свободны первые 2 датчика
			{
				if (sensors[0]->readSensor() == 0 ) fl = 0;
				if (sensors[1]->readSensor() == 0 ) fl = 0;
				//if (sensors[2]->readSensor() == 0 ) fl = 0;
				//if (sensors[3]->readSensor() == 0 ) fl = 0;
				//if (sensors[4]->readSensor() == 0 ) fl = 0;
			}
			if (fl)
			{	// установка в исходное состояние
				for (uint8_t i=0; i<5; i++)
				{
					times[i][0] = 0;
					times[i][1] = 0;
					protect[i] = 0;
					protect_interval = eeprom_read_word(&ns_vg::eeProtect);
				}
				mode = 2;
			}
			if (count > 0)
			{
				count = 0;
				timeEnd = 0;
				timeCrh = 0;
			}
			return;
		}
		if (mode == 2)
		{	// read data
			if (count == 0)
			{
				if (
				   ( (sensors[0]->readSensor() == 0) && (sensors[1]->readSensor() == 0) ) // !!!!!!!!!!!!! на обоих
				   ||
				   ( (sensors[0]->tr == 0) && (sensors[1]->readSensor() != 0) )			// 0 сработал, 1 - выключен
				)
				{	// начало измерительного отсчета
					count = 1;
				}
			}
			else
			{
				count++;
			}
			for (uint8_t i=0; i<5; i++)
			{
				if (sensors[i]->tr == 0) // наезд на датчик
				{
					if (count > 0)	// отладочный вывод в rs сработки датчика на наезд
					{
						ns_rs232::SendByte('d');
						ns_rs232::SendByte('0'+i);
						ns_rs232::SendByte(':');
						ns_rs232::DigitZ(6, count);
						ns_rs232::SendByte('\r');
						ns_rs232::SendByte('\n');
					}
					{
						// текущее состояние защитного интервала на датчике i
						uint16_t tmpProtect = protect[i];
						// перезапуск защитного интервала на датчике i
						protect[i] = protect_interval;
						// регистрация, если пройден защитный интервал
						if (tmpProtect == 0) times[i][0] = count;
					}
				}
				if (sensors[i]->tr == 1)	// запись по последнему событию (cход с датчика)
				{
					times[i][1] = count;
					if (count > 0)
					{
						ns_rs232::SendByte('D');
						ns_rs232::SendByte('0'+i);
						ns_rs232::SendByte(':');
						ns_rs232::DigitZ(6, count);
						ns_rs232::SendByte('\r');
						ns_rs232::SendByte('\n');
					}
				}
			}
			// авария
			{
				uint8_t fl = 0;
				for (uint8_t i=0; i<5; i++)
				{
					if (sensors[i]->readSensor() == 1)
					{
						fl = 1;
					}
				}
				if (!fl)
				{
					if (timeCrh == 0)
					{
						timeCrh = TIME_CRASH;
					}
					else
					{
						timeCrh--;
						if (timeCrh == 1)
						{
							timeCrh = 0;
							ns_vg::lenTubuStat = 10;
							mode = 1;
							return;
						}
					}
				}
			}
			
			// завершение замеров после схода с датчика 2
			if ( (sensors[1]->tr == 1) && (sensors[0]->readSensor() != 0) )
				{
					if (times[2][0] == 0)
					{
						timeEnd = ((uint32_t)3) * (times[1][1] - times[0][1]) / ((uint32_t)2); // завершение после 3/4 времени прохождения sq1->sq2 @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
					}
					else
					{
						timeEnd = ((uint32_t)FtTimerMain) / 4;
					}
				}
			if (timeEnd > 1) timeEnd--;
			if (timeEnd == 1)
			{
				timeEnd = 0;
				mode = 0; // render
			}
			return;
		}
	}
	// ====
	void sendTele(uint16_t dlina, int8_t sqBas, uint16_t base, uint16_t exL)
	{
		// send to rs tele
		ns_rs232::String_P(PSTR("№ tube: ") );
		ns_rs232::DigitZ(3, ns_vg::nTube + 1 );
		ns_rs232::String_P(PSTR("; L=") );
		ns_rs232::DigitZ(5, dlina );
		ns_rs232::String_P(PSTR("\r\n") );
		for (uint8_t i=0; i<5; i++) { ns_rs232::DigitZ(8, eeprom_read_word(&ns_vg::eeDistance[i]) ); ns_rs232::SendByte(' '); }
		ns_rs232::String_P(PSTR("\r\n") );
		// наезд
		for (uint8_t i=0; i<5; i++) { ns_rs232::DigitZ(8, times[i][0] ); ns_rs232::SendByte(' '); }
		ns_rs232::String_P(PSTR("\r\n") );
		// съезд
		for (uint8_t i=0; i<5; i++) { ns_rs232::DigitZ(8, times[i][1] ); ns_rs232::SendByte(' '); }
		ns_rs232::String_P(PSTR("\r\nsqBase=") );
		//uint8_t sqB;
		if (sqBas<0)
		{
			ns_rs232::SendByte('-');
			ns_rs232::DigitZ(2, -sqBas);
			//sqB = -sqBas;
		}
		else
		{
			ns_rs232::SendByte(' ');
			ns_rs232::DigitZ(2, sqBas);
			//sqB = sqBas;
		}
		ns_rs232::String_P(PSTR(" exN="));
		ns_rs232::DigitZ(2, ns_vg::exN);
		ns_rs232::String_P(PSTR(" SpeedCor="));
		ns_rs232::DigitZ(4, (int8_t)eeprom_read_byte(&ns_vg::eeCorSpeed) );
		ns_rs232::String_P(PSTR(" Base="));
		ns_rs232::DigitZ(5, base);
		ns_rs232::String_P(PSTR(" exL="));
		ns_rs232::DigitZ(5, exL);

		//
		ns_rs232::String_P(PSTR("\r\n") );
		ns_rs232::String_P(PSTR("\r\n") );
	}
	//
	uint8_t Render(uint16_t* dlina, int8_t* sqBas)
	{
		if (mode != 0) return 0; // no render mode
		
		// load distance
		for (uint8_t i=0; i<5; i++)
			distance[i] = eeprom_read_word(&ns_vg::eeDistance[i]);
		
		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		/*distance[0] = 0;
		distance[1] = 790;
		distance[2] = 9404;
		distance[3] = 10775;
		distance[4] = 12170;*/
		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

		volatile uint8_t		n;
		volatile uint16_t  base;
		volatile uint16_t	exLen;
		volatile uint16_t  sampleD;
		volatile uint32_t sampleT;
		volatile uint32_t extendT;
		// sample for render (d1-d2)
		sampleD = distance[1] - distance[0] + (int8_t)( eeprom_read_byte(&ns_vg::eeCorSpeed) );
		sampleT = times[1][1] - times[0][1];
		//
		if ( times[2][0] >= times[1][1] )
		{	// tube small d2-d3
			base = distance[2] - distance[1];
			extendT = times[2][0] - times[1][1]; // !!! time < 0
			exLen = (uint16_t)( ((uint64_t)extendT) * ((uint64_t)sampleD) / ((uint64_t)sampleT) );
			*dlina = base - exLen;
			ns_vg::lenTubuStat = 1;
			ns_vg::exN = 8;
			ns_vg::lenTubuExl = -exLen;
			*sqBas = -12;
			sendTele(*dlina, *sqBas, base, exLen);
			mode = 1;
			return 1;
		}
		else
		if ( (times[0][1] >= times[4][0]) && (times[4][0] != 0) )
		{	// tube long d1-d5
			base = distance[4] - distance[0];
			extendT = times[0][1] - times[4][0];
			exLen = (uint16_t)( ((uint64_t)extendT) * ((uint64_t)sampleD) / ((uint64_t)sampleT) );
			*dlina = base + exLen;
			ns_vg::lenTubuStat = 1;
			ns_vg::exN = 9;
			ns_vg::lenTubuExl = exLen;
			*sqBas = 04;
			sendTele(*dlina, *sqBas, base, exLen);
			ns_vg::sqBas = *sqBas;
			mode = 1;
			return 1;
		}
		else
		{
			// find right sensor
			for (n = 2; n < 4; n++)
			{
				if ( times[n+1][0] == 0)
				{
					n--;
					break;
				}
				if ( (times[1][1] >= times[n][0]) && (times[1][1] < times[n+1][0]) )
					break;
			}
			
			uint32_t ex[7];
			for (uint8_t i=0; i<7; i++) ex[i] = 99999;
			uint8_t exN;
			
			// render all metod
			
			if ( (times[n][0] > times[0][1]) && (times[n][0] != 0) && (n < 5) )
					{
						extendT = times[n][0] - times[0][1];		// 1920 = -20	base[n-0]		- ex	:	n-0
						ex[0] = extendT;
					}
			else    ex[0] = 99999;
			
			if ( (times[0][1] >= times[n][0]) && (times[n][0] != 0) && (n < 5) )
					{
						extendT = times[0][1] - times[n][0];		// 2013 = +12	base[n-0]		+ ex	:	0-n
						ex[1] = extendT;
					}
			else	ex[1] = 99999;
			
			if ( (times[0][1] >= times[n+1][0]) && (times[n+1][0] != 0) && ((n + 1) < 5) )
					{
						extendT = times[0][1] - times[n+1][0];		// 2013 = +xx	base[(n+1)-0]	+ ex	:	0-(n+1)
						ex[5] = extendT;
					}
			else	ex[5] = 99999;

			if ( (times[1][1] >= times[n][0]) && (times[n][0] != 0) && (n < 5) )
					{
						extendT = times[1][1] - times[n][0];		// 2013 = +512	base[n-1]		+ ex	:	1-n
						ex[2] = extendT;
					}
			else	ex[2] = 99999;

			if ( (times[1][1] >= times[n+1][0]) && (times[n+1][0] != 0) && ((n + 1) < 5) )
					{
						extendT = times[1][1] - times[n+1][0];		// 2013 = +xx	base[(n+1)-1]		+ ex	:	1-(n+1)
						ex[6] = extendT;
					}
			else	ex[6] = 99999;

			if ( (times[n+1][0] > times[1][1]) && (times[n+1][0] != 0) && ((n + 1) < 5) )
					{
						extendT = times[n+1][0] - times[1][1];	// 2013 = -488	base[(n+1)-1]	- ex	:	(n+1)-1
						ex[3] = extendT;
					}
			else	ex[3] = 99999;

			if ( (times[n+1][0] > times[0][1]) && (times[n+1][0] != 0) && ((n + 1) < 5) )
					{
						extendT = times[n+1][0] - times[0][1];	// 2013 = -988	base[(n+1)-0]	- ex	:	(n+1)-0
						ex[4] = extendT;
					}
			else	ex[4] = (uint32_t)99999;

			extendT = 99999;
			exN = 10;
			
			// find min render
			for (uint8_t i=0; i<7; i++)
			{
				if ( ex[i] < extendT )
				{
					extendT = ex[i];
					exN = i;
				}
			}
			
			if (extendT == 99999)
			{	// error - no solution
				ns_vg::lenTubuStat = 10;
				mode = 1;
				return 0;
			}
			ns_vg::exN = exN;
			// new render tube
			ns_vg::lenTubuStat = 1;
			if ( exN == 0 )
			{	// 1920 = -20	base[n-0]		- ex	:	n-0
				base = distance[n] - distance[0];
				exLen = (uint16_t)((((((uint64_t)extendT) * ((uint64_t)sampleD) * ((uint64_t)10) ) / ((uint64_t)sampleT)) + 5) / 10);
				*dlina = base - exLen;
				*sqBas = -((0 * 10) + (n + 0));
				ns_vg::lenTubuExl = -exLen;
				sendTele(*dlina, (*sqBas), base, exLen );
				mode = 1;
				return 1;
			}
			if ( exN == 1 )
			{	// 2013 = +12	base[n-0]		+ ex	:	0-n
				base = distance[n] - distance[0];
				exLen = (uint16_t)( ( ( ((uint64_t)10) * ((uint64_t)extendT) * ((uint64_t)sampleD) / ((uint64_t)sampleT) ) + 5) / 10);
				*dlina = base + exLen;
				*sqBas = ((0 * 10) + (n + 0));
				ns_vg::lenTubuExl = exLen;
				sendTele(*dlina, (*sqBas), base, exLen);
				mode = 1;
				return 1;
			}
			if ( exN == 5 )
			{	// 2013 = +12	base[(n+1)-0]		+ ex	:	0-(n+1)
				base = distance[n+1] - distance[0];
				exLen = (uint16_t)( ( ( ((uint64_t)10) * ((uint64_t)extendT) * ((uint64_t)sampleD) / ((uint64_t)sampleT) ) + 5) / 10);
				*dlina = base + exLen;
				*sqBas = ((0 * 10) + (n + 1));
				ns_vg::lenTubuExl = exLen;
				sendTele(*dlina, (*sqBas), base, exLen);
				mode = 1;
				return 1;
			}
			if ( exN == 2 )
			{	// 2013 = +512	base[n-1]		+ ex	:	1-n
				base = distance[n] - distance[1];
				exLen = (uint16_t)( ( ( ((uint64_t)10) * ((uint64_t)extendT) * ((uint64_t)sampleD) / ((uint64_t)sampleT) ) + 5) / 10);
				*dlina = base + exLen;
				*sqBas = ((1 * 10) + (n + 0));
				ns_vg::lenTubuExl = exLen;
				sendTele(*dlina, (*sqBas), base, exLen);
				mode = 1;
				return 1;
			}
			if ( exN == 6 )
			{	// 2013 = +xx	base[(n+1)-1]		+ ex	:	1-(n+1)
				base = distance[n+1] - distance[1];
				exLen = (uint16_t)( ( ( ((uint64_t)10) * ((uint64_t)extendT) * ((uint64_t)sampleD) / ((uint64_t)sampleT) ) + 5) / 10);
				*dlina = base + exLen;
				*sqBas = ((1 * 10) + (n + 1));
				ns_vg::lenTubuExl = exLen;
				sendTele(*dlina, (*sqBas), base, exLen);
				mode = 1;
				return 1;
			}
			if ( exN == 3 )
			{	// 2013 = -488	base[(n+1)-1]	- ex	:	(n+1)-1
				base = distance[n+1] - distance[1];
				exLen = (uint16_t)( ( ( ((uint64_t)10) * ((uint64_t)extendT) * ((uint64_t)sampleD)  / ((uint64_t)sampleT) ) + 5) / 10);
				*dlina = base - exLen;
				*sqBas = -((1 * 10) + (n + 1));
				ns_vg::lenTubuExl = -exLen;
				sendTele(*dlina, (*sqBas), base, exLen);
				mode = 1;
				return 1;
			}
			if ( exN == 4 )
			{	// 2013 = -988	base[(n+1)-0]	- ex	:	(n+1)-0
				base = distance[n+1] - distance[0];
				exLen = (uint16_t)( ( ( ((uint64_t)10) * ((uint64_t)extendT) * ((uint64_t)sampleD) / ((uint64_t)sampleT) ) + 5) / 10);
				*dlina = base - exLen;
				*sqBas = -((0 * 10) + (n + 1));
				ns_vg::lenTubuExl = -exLen;
				sendTele(*dlina, (*sqBas), base, exLen);
				mode = 1;
				return 1;
			}
		}
			ns_rs232::String_P(PSTR("unknow error\r\n\r\n"));
			mode = 1;
			return 0;
	}
	// ====
	// ====
}