/* 
* menu.cpp
*
* Created: 17.10.2016 15:41:31
* Author: GumerovMN
*/
#include <avr/pgmspace.h>
#include "menu.h"
#include "system/path.h"

namespace ns_menu
{
	unsigned char mode = 0;
	uint32_t  timeOut = 0;
	void Dupm(unsigned char key);
	// 0 : no key (view)
	// 1-4 : main key
	// 5 : add key
	// 6 : set mode
	#define SETMODE 6
	// 7 : timeout
	#define TIMEOUT 7
	// *************************************************
	#define TIMEOUT_MENU (((uint32_t)FT_LcdKeyTimer) * ((uint32_t)90))
	typedef void(*TyFnMn)(unsigned char);
	extern const TyFnMn MassFnMn[][8];
	#define FnMenu(mode, stat, k) ((TyFnMn)pgm_read_ptr(& MassFnMn[mode][stat]))(k)
	//
	void SetMode(unsigned char modeS)
	{
		mode = modeS;
		FnMenu(modeS, SETMODE, 0);
	}
	void Init()
	{
		SetMode(0);
	}
	void Main()
	{
		unsigned char key;
		unsigned char keyStat = key4->Read(&key);
		uint32_t to;
		//if (keyStat>0)
		//	scr->Digit(scr->SetPosition(6,0), 2, key);
		CRITICAL_SECTION
		{
			to = timeOut;
		}
		if (to == 1)
		{
			CRITICAL_SECTION
			{
				timeOut = 0;
			}
			FnMenu(mode, TIMEOUT, key);
		}
		else
		{
			FnMenu(mode, keyStat, key);
		}
	}
	void Interrupt()
	{
		CRITICAL_SECTION
		{
			if (timeOut > 1) timeOut--;
		}
	}
	void Dupm(unsigned char par) {}
	// локальные прототипы
	void Menu_CheckPass(unsigned char k);
	void Menu_CheckPassView(unsigned char k);
	void Menu_CheckPassBack(unsigned char k);
	void Menu_CheckPassMinus(unsigned char k);
	void Menu_CheckPassPlus(unsigned char k);
	void Menu_CheckPassVv(unsigned char k);
	void Menu_Select(unsigned char);
	void Menu_SelectView(unsigned char k);
	void Menu_SelectBack(unsigned char k);
	void Menu_SelectMinus(unsigned char k);
	void Menu_SelectVV(unsigned char k);
	void Edit_Sensor(unsigned char k);
	void Edit_SensorView(unsigned char k);
	void Set_PassView(unsigned char k);
	// ============================================
		// определение строки
		#define MAIN				0
		#define MENU_CHECK_PASS		1
		#define MENU_SELECT			2
		#define EDIT_SENSOR			3
		#define TIMEOUT_TO_MAIN		4
		#define SET_PASS			5
		#define TIMEOUT_TO_MNSEL	6
		#define SET_TIME			7
		#define VIEW_ARC			8
		#define COR_CUR_TUBE		9
		#define ZERO_ARC			10
		#define PROTECTINTERVAL     11
		#define TMINTEGRAL			12
		#define CORSPEED			13
		//
		//			переменные
		// позитции постоянных сообщений
		//
		uint8_t varPos1, varPos2, varPos3, varPos4, varPos5;
		// текущяя позитция при вводе пароля
		uint8_t curPosPass;
		// вводимый пароль
		char inPass[5];
		char eePass[5] EEMEM         = {0, 0, 0, 0, 0 };
		const char ffPass[5] PROGMEM = {2, 2, 3, 6, 0 };
		// текущий датчик
		uint8_t curSensor;
		uint8_t curSensIdx;
		// временная для редактирование дистанции для датчиков
		uint16_t tmpDistance;
		// копия времени часов
		uint8_t setTime_time[7];
		uint8_t setTime_timeN;
		//
		archiveStruct corCurTube_cell;
		int16_t corCurTube_idx;
	
	
	void Main_vArc()
	{
		//uint16_t n;
		uint32_t al;
		//n = 
		ns_archive::Summ(&al);
		//scr::DigitZ(scr::SetPosition( 6, 1), 3, n);
		uint8_t pos = scr::SetPosition(7, 1);
		scr::String_P(&pos, PSTR("S:"));
		scr::DigitZ(&pos, 7, al);
	}
	void Main_SetMode(unsigned char mod)
	{
		CRITICAL_SECTION { timeOut = 0; }
		//mode = MAIN;
		key4->autoRepeat = false;
		scr::Clear();
		//scr::String_P( scr::SetPosition(0, 0), PSTR("L=") );

		/*if ( ns_vg::lenTubuStat == 1 )
		{
			scr::Digit(scr::SetPosition(2,0), 5, ns_vg::lenTube);
		}*/
		// всего
		Main_vArc();	
		if ( (ns_archive::len == 0) && (eeprom_read_word(&ns_archive::lenEE) == 0) )
			return;
		corCurTube_idx = ns_archive::idx;
		if (corCurTube_idx == 0) corCurTube_idx = ARCHIVE_LEN - 1;
		else corCurTube_idx--;
		eeprom_read_block(&corCurTube_cell, &ns_archive::archive[corCurTube_idx], sizeof(archiveStruct));
		uint8_t pos = scr::SetPosition(0,0);
		scr::DigitZ(&pos, 3, corCurTube_cell.nTube);
		scr::PutChar(&pos, ':');
		scr::Digit(&pos, 5, corCurTube_cell.len);

	}
	void Main_ViewEx(unsigned char k)
	{
		if (k != 5) return;
		uint8_t pos = scr::SetPosition(7, 1);
		int8_t sq = ns_vg::sqBas;
		if (sq < 0)
		{
			//scr::PutChar(&pos, '-');
			sq = -sq;
		}
		/*else
		{
			scr::PutChar(&pos, '+');
		}*/
		scr::DigitZ(&pos, 2, sq);
		scr::String_P(&pos, PSTR(": "));
		int16_t exl = ns_vg::lenTubuExl;
		if (exl < 0 )
		{
			scr::PutChar(&pos, '-');
			exl = -exl;
		}
		else scr::PutChar(&pos, '+');
		scr::DigitZ(&pos, 4, exl);
	}
	void Main_View(unsigned char k)
	{
		uint8_t pos;
		if ( ns_vg::lenTubeNew )
		{
			ns_vg::lenTubeNew = 0;
			scr::Clear();
			pos = scr::SetPosition(0, 0);
			if ( ns_vg::lenTubuStat == 10 )
			{
				scr::String_P(&pos, PSTR("ERROR   "));
			}
			if ( ns_vg::lenTubuStat == 1 )
			{
				scr::DigitZ(&pos, 3, ns_vg::nTube);
				scr::PutChar(&pos, ':');
				scr::Digit(&pos, 5, ns_vg::lenTube);
				//scr::String_P(scr::SetPosition(7, 0), PSTR("мм"));
				//  tele !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				/*pos = scr::SetPosition(5, 1);
				if (ns_vg::sqBas < 0)
				{
					scr::PutChar(&pos, '-');
					scr::DigitZ(&pos, 2, -ns_vg::sqBas);
				}
				else
				{
					scr::PutChar(&pos, ' ');
					scr::DigitZ(&pos, 2, ns_vg::sqBas);
				}
				scr::PutChar(&pos, '0'+ns_vg::exN);*/
				// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			}
			// всего
			Main_vArc();
		}
		// view sensors
		pos = scr::SetPosition(0, 1);
		for (uint8_t i = 0; i < 5; i++)
		{
			if ( ns_line::sensors[i]->readSensor() )
			{
				scr::PutChar(&pos, 0);
			}
			else
			{
				scr::PutChar(&pos, 1);
			}
		}
		// clock
		pos = scr::SetPosition(11, 0);
		if (ns_clock::tik)
		{
			scr::Digit (&pos, 2, ns_clock::time[CT_HOUR]);
			//scr::Digit (&pos, 2, ns_clock::time[CT_SECOND]);
			if (ns_clock::tik_cn == 0)
			{
				scr::PutChar(&pos, ':');
			}
			else
			{
				scr::PutChar(&pos, ' ');
			}
			scr::DigitZ(&pos, 2, ns_clock::time[CT_MINUTE]);
			//scr::DigitZ(scr::SetPosition(14,0), 2, ns_clock::time[CT_SECOND]);
		}
	}
	void Main_menu(unsigned char k)
	{
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!		
		SetMode(MENU_CHECK_PASS);
		//SetMode(MENU_SELECT);
	}
	void Main_mins(unsigned char k)
	{
		//xx_count--;
		Main_View(k);
	}
	void Main_plus(unsigned char k)
	{
		//xx_count++;
		Main_View(k);
	}
	void Main_rOff(unsigned char k) { key4->autoRepeat = false;	}
	void Main_rOn (unsigned char k) { key4->autoRepeat = true;	}
		// ==============================
		// MENU_CHECK_PASS
		void Menu_CheckPass(unsigned char k)
		{
			CRITICAL_SECTION { timeOut = 0; }
			//mode = MENU_CHECK_PASS;
			key4->autoRepeat = false;
			scr::Clear();
			//scr::pos = scr::SetPosition(0, 0);
			//printf_P(PSTR("Меню-пароль:"));
			scr::String_P(scr::SetPosition(0, 0), PSTR("Введите пароль:"));
			curPosPass = 0;
			for (uint8_t i = 0; i < 5; i++)
				inPass[i] = 0;
			CRITICAL_SECTION	{timeOut = TIMEOUT_MENU;}
		}
		void Menu_CheckPassView(unsigned char k)
		{
			uint8_t pos = scr::SetPosition(5, 1);
			for (uint8_t i = 0; i < 5;i++)
			{
				if (i < curPosPass)
				{
					scr::flicker = false;
					scr::PutChar(&pos, '*');
					continue;
				}
				if (i == curPosPass)
				{
					//scr::flicker = true;
					scr::PutChar(&pos, '0' + inPass[i]);
					scr::flicker = false;
					continue;
				}
				scr::flicker = false;
				scr::PutChar(&pos, '-');
			}
		}
		void Menu_CheckPassBack(unsigned char k)
		{
			if (curPosPass > 0)
			{
				curPosPass--;
				Menu_CheckPassView(k);
				CRITICAL_SECTION	{timeOut = TIMEOUT_MENU;}
			}
			CRITICAL_SECTION { timeOut = 0; }
			SetMode(MAIN);// Main_SetMode(k);
		}
		void Menu_CheckPassMinus(unsigned char k)
		{
			key4->autoRepeat = true;
			if (inPass[curPosPass] > 0) inPass[curPosPass]--;
			Menu_CheckPassView(k);
			CRITICAL_SECTION	{timeOut = TIMEOUT_MENU;}
		}
		void Menu_CheckPassPlus(unsigned char k)
		{
			key4->autoRepeat = true;
			if (inPass[curPosPass] < 9) inPass[curPosPass]++;
			Menu_CheckPassView(k);
			CRITICAL_SECTION	{timeOut = TIMEOUT_MENU;}
		}
		void Menu_CheckPassVv(unsigned char k)
		{
			CRITICAL_SECTION { timeOut = TIMEOUT_MENU; }
			key4->autoRepeat = false;
			if (curPosPass < 4)
			{
				//char t = inPass[curPosPass];
				curPosPass++;
				inPass[curPosPass] = 0;
				Menu_CheckPassView(k);
			}
			else
			{
				// проверка пароля
				bool fl1 = true;
				bool fl2 = true;
				for (uint8_t i=0; i<5; i++)
					{
						if (inPass[i] != (char)eeprom_read_byte((uint8_t *)&eePass[i])) fl1 = false;
						if (inPass[i] != (char)pgm_read_byte((uint8_t *)&ffPass[i])) fl2 = false;
					}
				if (fl1 || fl2)
					SetMode(MENU_SELECT);
				else
				{
					scr::Clear();
					//printf_P(PSTR("Ошибка ввода"));
					//scr::pos = scr::SetPosition(0, 1);
					//printf_P(PSTR("пароля"));
					scr::String_P(scr::SetPosition(0, 0), PSTR("Ошибка ввода    "));
					scr::String_P(scr::SetPosition(0, 1), PSTR("пароля          "));
					CRITICAL_SECTION	{timeOut = 5000;}
					SetMode(TIMEOUT_TO_MAIN);
				}
				
			}
		}
		void Menu_CheckPassTout(unsigned char k)
		{
			CRITICAL_SECTION { timeOut = 0; }
			SetMode(MAIN);
		}
		// ==============================
		const char menuSelectTab_str1[]  PROGMEM  = ">Просм. архив   ";
		const char menuSelectTab_str2[]  PROGMEM  = ">корр. длины    ";
		const char menuSelectTab_str3[]  PROGMEM  = ">корр. скорости ";
		const char menuSelectTab_str4[]  PROGMEM  = ">Сброс архива   ";
		const char menuSelectTab_str5[]  PROGMEM  = ">Дист. SQ1<->SQ2";
		const char menuSelectTab_str6[]  PROGMEM  = ">Дист. SQ1<->SQ3";
		const char menuSelectTab_str7[]  PROGMEM  = ">Дист. SQ1<->SQ4";
		const char menuSelectTab_str8[]  PROGMEM  = ">Дист. SQ1<->SQ5";
		const char menuSelectTab_str9[]  PROGMEM  = ">Защ. интервал  ";
		const char menuSelectTab_str10[] PROGMEM  = ">Интегр.сигнала ";
		const char menuSelectTab_str11[] PROGMEM  = ">Уст. времени   ";
		const char menuSelectTab_str12[] PROGMEM  = ">Уст. пароль    ";
		const char *menuSelectTab[] = {
			menuSelectTab_str1,
			menuSelectTab_str2,
			menuSelectTab_str3,
			menuSelectTab_str4,
			menuSelectTab_str5,
			menuSelectTab_str6,
			menuSelectTab_str7,
			menuSelectTab_str8,
			menuSelectTab_str9,
			menuSelectTab_str10,
			menuSelectTab_str11,
			menuSelectTab_str12
		};
		void Menu_Select(unsigned char k)
		{
			CRITICAL_SECTION { timeOut = 0; }
			//mode = MENU_SELECT;
			key4->autoRepeat = false;
			scr::Clear();
			scr::pos = scr::SetPosition(0, 0);
			//printf_P(PSTR("Меню-выбор:"));
			curPosPass = 0;
			Menu_SelectView(k);
			CRITICAL_SECTION { timeOut = TIMEOUT_MENU; }
		}
		void Menu_SelectBack(unsigned char k)
		{
			CRITICAL_SECTION { timeOut = 0; }
			SetMode(MAIN);
		}
		void Menu_SelectView(unsigned char k)
		{
			// очистка строки
			//for (uint8_t i = 0; i < scr::Stolbcov(); i++) scr::PutChar(scr::SetPosition(i,0), ' ');
			scr::Clear();
			scr::String_P((uint8_t)0, menuSelectTab[curPosPass]);
			/*scr::PutChar(scr::SetPosition(2, 1), char(134));
			scr::String(scr::SetPosition(5, 1), "<<");
			scr::String(scr::SetPosition(8, 1), ">>");
			scr::PutChar(scr::SetPosition(12, 1), char(126));*/
		}
		void Menu_SelectMinus(unsigned char k)
		{
			key4->autoRepeat = false;
			if (curPosPass > 0)
			{
				curPosPass--;
				Menu_SelectView(k);
			}
			CRITICAL_SECTION { timeOut = TIMEOUT_MENU; }
		}
		void Menu_SelectPlus(unsigned char k)
		{
			key4->autoRepeat = false;
			if (curPosPass < (sizeof(menuSelectTab) / sizeof(char *)) - 1 )
			{
				curPosPass++;
				Menu_SelectView(k);
			}
			CRITICAL_SECTION { timeOut = TIMEOUT_MENU; }
		}
		void Menu_SelectVV(unsigned char k)
		{
			CRITICAL_SECTION { timeOut = 0; }
			key4->autoRepeat = false;
			if (curPosPass ==  0) { SetMode(VIEW_ARC); return; }
			if (curPosPass ==  1) { SetMode(COR_CUR_TUBE); return; }
			if (curPosPass ==  2) { SetMode(CORSPEED); return; }
			if (curPosPass ==  3) { SetMode(ZERO_ARC); return; }
			if (curPosPass ==  4) { curSensIdx = 1; curSensor = 2; SetMode(EDIT_SENSOR); return; }
			if (curPosPass ==  5) { curSensIdx = 2; curSensor = 3; SetMode(EDIT_SENSOR); return; }
			if (curPosPass ==  6) { curSensIdx = 3; curSensor = 4; SetMode(EDIT_SENSOR); return; }
			if (curPosPass ==  7) { curSensIdx = 4; curSensor = 5; SetMode(EDIT_SENSOR); return; }
			if (curPosPass ==  8) { SetMode(PROTECTINTERVAL); return; }
			if (curPosPass ==  9) { SetMode(TMINTEGRAL); return; }
			if (curPosPass == 10) { SetMode(SET_TIME); return; }
			if (curPosPass == 11) { SetMode(SET_PASS); return; }
		}
		void Menu_SelectTout(unsigned char k)
		{
			CRITICAL_SECTION { timeOut = 0; }
			SetMode(MAIN);
		}
		// ==============================
		//#define  EDIT_SENSOR_TO 60000
		void Edit_Sensor(unsigned char k)
		{
			CRITICAL_SECTION { timeOut = 0; }
			//mode = EDIT_SENSOR;
			key4->autoRepeat = false;
			tmpDistance = eeprom_read_word(&ns_vg::eeDistance[curSensIdx]);
			scr::Clear();
			//scr::pos = scr::SetPosition(0, 0);
			//printf_P(PSTR("Настройка"));
			scr::String_P(scr::SetPosition(2, 0), PSTR("Установка базы"));
			uint8_t pos = scr::SetPosition(1, 1);
			scr::String_P(&pos, PSTR("SQ1<->SQ"));
			scr::DigitZ(&pos, 1, curSensor);
			scr::PutChar(&pos, '=');
			varPos1 = pos;
			Edit_SensorView(k);
			CRITICAL_SECTION { timeOut = TIMEOUT_MENU; }
		}
		void Edit_SensorView(unsigned char k)
		{
			//scr::pos = scr::SetPosition(0, 1);
			//printf_P(PSTR("Д%1d: дист.=%5d"), curSensor, tmpDistance);
			scr::DigitZ(varPos1, 5, tmpDistance );
		}
		void Edit_SensorBack(unsigned char k)
		{
			CRITICAL_SECTION { timeOut = 0; }
			key4->autoRepeat = false;
			SetMode(MENU_SELECT);
		}
		void Edit_SensorMinus(unsigned char k)
		{
			key4->autoRepeat = true;
			if (tmpDistance > 0)
			{
				tmpDistance--;
				Edit_SensorView(k);
			}
			CRITICAL_SECTION { timeOut = TIMEOUT_MENU; }
		}
		void Edit_SensorPlus(unsigned char k)
		{
			key4->autoRepeat = true;
			if (tmpDistance < 15000)
			{
				tmpDistance++;
				Edit_SensorView(k);
			}
			CRITICAL_SECTION { timeOut = TIMEOUT_MENU; }
		}
		void Edit_SensorMulti(unsigned char k)
		{
			key4->autoRepeat = true;
			if (k == 3)
			{
				if (tmpDistance >= 10)
				{
					tmpDistance -= 10;
					Edit_SensorView(k);
				}
				else if (tmpDistance > 0)
				{
					tmpDistance = 0;
					Edit_SensorView(k);
				}
			}
			if (k == 5)
			{
				if (tmpDistance <= 14990)
				{
					tmpDistance += 10;
					Edit_SensorView(k);
				}
				else if (tmpDistance < 15000)
				{
					tmpDistance = 15000;
					Edit_SensorView(k);
				}
			}
			CRITICAL_SECTION { timeOut = TIMEOUT_MENU; }
		}
		void Edit_SensorVV(unsigned char k)
		{
			CRITICAL_SECTION { timeOut = 0; }
			key4->autoRepeat = false;
			eeprom_write_word(&ns_vg::eeDistance[curSensIdx], tmpDistance);
			//SetMode(MENU_SELECT);
			//scr::pos = scr::SetPosition(0, 1);
			//printf_P(PSTR("** сохранено **"));
			scr::String_P(scr::SetPosition(0,1), PSTR("**Сохранено**"));
			CRITICAL_SECTION{ timeOut = 5000; }
			SetMode(TIMEOUT_TO_MNSEL);
		}
		void Edit_SensorTout(unsigned char k)
		{
			CRITICAL_SECTION { timeOut = 0; }
			SetMode(MENU_SELECT);
		}
		// ==============================
		//void Timeout_To_Main(unsigned char k)
		//{
			//key4->autoRepeat = false;
			//mode = TIMEOUT_TO_MAIN;
		//}
		void TimeoutToMainE(unsigned char k)
		{
			SetMode(MAIN);
		}
		// ==============================
		// установка нового пароля
		void Set_Pass(unsigned char k)
		{
			CRITICAL_SECTION { timeOut = 0; }
			scr::Clear();
			//printf_P(PSTR("новый пароль:"));
			scr::String_P(scr::SetPosition(0, 0), PSTR("Новый пароль :") );
			for (uint8_t i=0; i<5; i++) inPass[i] = 0;
			curPosPass = 0;
			Set_PassView(k);
			key4->autoRepeat = false;
			CRITICAL_SECTION { timeOut = TIMEOUT_MENU; }
		}
		void Set_PassView(unsigned char k)
		{
			for (uint8_t i = 0; i < 5;i++)
			{
				uint8_t pos = scr::SetPosition(i + 0, 1);
				if (i < curPosPass)
				{
					scr::flicker = false;
					scr::PutChar(&pos, '*');
					continue;
				}
				if (i == curPosPass)
				{
					//scr::flicker = true;
					scr::PutChar(&pos, '0' + inPass[i]);
					scr::flicker = false;
					continue;
				}
				scr::flicker = false;
				scr::PutChar(&pos, '-');
			}
		}
		void Set_PassBack(unsigned char k)
		{
			CRITICAL_SECTION { timeOut = 0; }
			key4->autoRepeat = false;
			timeOut = 0;
			SetMode(MENU_SELECT);
		}
		void Set_PassMinus(unsigned char k)
		{
			key4->autoRepeat = true;
			if (inPass[curPosPass]>0)
			{
				inPass[curPosPass]--;
				Set_PassView(k);
			}
			CRITICAL_SECTION { timeOut = TIMEOUT_MENU; }
		}
		void Set_PassPlus(unsigned char k)
		{
			key4->autoRepeat = true;
			if (inPass[curPosPass]<9)
			{
				inPass[curPosPass]++;
				Set_PassView(k);
			}
			CRITICAL_SECTION { timeOut = TIMEOUT_MENU; }
		}
		void Set_PassVV(unsigned char k)
		{
			key4->autoRepeat = false;
			if (curPosPass < 4)
			{
				char t = 0; //inPass[curPosPass];
				curPosPass++;
				inPass[curPosPass] = t;
				Set_PassView(k);
				CRITICAL_SECTION { timeOut = TIMEOUT_MENU; }
			}
			else
			{
				for (uint8_t i=0; i<5; i++)
					eeprom_write_byte((uint8_t *)&eePass[i], inPass[i]);
				scr::Clear();
				//printf_P(PSTR("Новый пароль"));
				scr::pos = scr::SetPosition(0, 1);
				//printf_P(PSTR("установлен"));
				scr::String_P(scr::SetPosition(0, 1), PSTR("установлен"));
				CRITICAL_SECTION { timeOut = 5000; }
				SetMode(TIMEOUT_TO_MAIN);
			}
		}
		void Set_PassTout(unsigned char k)
		{
			CRITICAL_SECTION { timeOut = 0; }
			SetMode(MENU_SELECT);
		}
		// ==============================
		void TimeoutToMnSel(unsigned char k)
		{
			CRITICAL_SECTION { timeOut = 0; }
			SetMode(MENU_SELECT);
		}
		// ==============================
		void Set_TimeView()
		{
			// часы
			if (setTime_timeN == CT_HOUR)
			scr::flicker = true;
			scr::DigitZ(scr::SetPosition(11, 1), 2, setTime_time[CT_HOUR]);
			scr::flicker = false;
			// минуты
			if (setTime_timeN == CT_MINUTE)
			scr::flicker = true;
			scr::DigitZ(scr::SetPosition(14, 1), 2, setTime_time[CT_MINUTE]);
			scr::flicker = false;
			// дата
			if (setTime_timeN == CT_DATE)
			scr::flicker = true;
			uint8_t m = pgm_read_byte(&ns_clock::nDayMountx[setTime_time[CT_YEAR] % 4][setTime_time[CT_MONTH]]);
			if (setTime_time[CT_DATE] < 1) setTime_time[CT_DATE] = 1;
			if (setTime_time[CT_DATE] > m) setTime_time[CT_DATE] = m;
			scr::DigitZ(scr::SetPosition(0, 1), 2, setTime_time[CT_DATE]);
			scr::flicker = false;
			// месяц
			if (setTime_timeN == CT_MONTH)
			scr::flicker = true;
			scr::DigitZ(scr::SetPosition(3, 1), 2, setTime_time[CT_MONTH]);
			scr::flicker = false;
			// год
			if (setTime_timeN == CT_YEAR)
			scr::flicker = true;
			scr::String_P(scr::SetPosition(6, 1), PSTR("20"));
			scr::DigitZ(scr::SetPosition(8, 1), 2, setTime_time[CT_YEAR]);
			scr::flicker = false;
			// знаки
			scr::PutChar(scr::SetPosition(2, 1), '-');
			scr::PutChar(scr::SetPosition(5, 1), '-');
			// секунды
			if (setTime_timeN == CT_SECOND)
			scr::flicker = true;
			scr::PutChar(scr::SetPosition(13, 1), ':');
			scr::flicker = false;
		}
		void Set_Time(unsigned char k)
		{
			CRITICAL_SECTION { timeOut = 0; }
			key4->autoRepeat = false;
			scr::Clear();
			scr::String_P(scr::SetPosition(0, 0), PSTR("Меню-время"));
			curPosPass = 0;
			CRITICAL_SECTION
			{
				for (uint8_t i = 0; i < 7; i++)
					setTime_time[i] = ns_clock::time[i];
				setTime_timeN = 6;
				timeOut = TIMEOUT_MENU;
			}
			Set_TimeView();
		}
		void Set_TimeMinus(uint8_t k)
		{
			key4->autoRepeat = true;
			uint8_t min = 1;
			if (setTime_timeN == CT_YEAR)
				min = 17;
			if (setTime_timeN == CT_HOUR)
			min = 0;
			if (setTime_timeN == CT_MINUTE)
			min = 0;
			if (setTime_time[setTime_timeN] > min) setTime_time[setTime_timeN]--;
			Set_TimeView();
			CRITICAL_SECTION {timeOut = TIMEOUT_MENU;}
		}
		void Set_TimePlus(uint8_t k)
		{
			key4->autoRepeat = true;
			uint8_t max;
			if (setTime_timeN == CT_YEAR)
				max = 25;
			if (setTime_timeN == CT_MONTH)
				max = 12;
			if (setTime_timeN == CT_DATE)
				max = pgm_read_byte(&ns_clock::nDayMountx[setTime_time[CT_YEAR] % 4][setTime_time[CT_MONTH]]);
			if (setTime_timeN == CT_HOUR)
			max = 23;
			if (setTime_timeN == CT_MINUTE)
			max = 59;
			if (setTime_time[setTime_timeN] < max) setTime_time[setTime_timeN]++;
			Set_TimeView();
			CRITICAL_SECTION {timeOut = TIMEOUT_MENU;}
		}
		void Set_TimeEnt(uint8_t k)
		{
			if (setTime_timeN > 0)
			{
				setTime_timeN--;
				if (setTime_timeN == CT_DAY) setTime_timeN--;
				Set_TimeView();
				CRITICAL_SECTION {timeOut = TIMEOUT_MENU;}
			}
			else
			{
				CRITICAL_SECTION
				{
					ns_clock::SetYear(setTime_time[CT_YEAR]);
					ns_clock::SetMonth(setTime_time[CT_MONTH]);
					ns_clock::SetDate(setTime_time[CT_DATE]);
					ns_clock::SetHour(setTime_time[CT_HOUR]);
					ns_clock::SetMinute(setTime_time[CT_MINUTE]);
					ns_clock::SetZeroSecond();
					ns_clock::Update();
				}
				SetMode(MAIN);
			}
		}
		void Set_TimeBack(uint8_t k)
		{
			if (setTime_timeN < CT_YEAR)
			{
				setTime_timeN++;
				if (setTime_timeN == CT_DAY) setTime_timeN++;
				Set_TimeView();
				CRITICAL_SECTION {timeOut = TIMEOUT_MENU;}
			}
			else
			{
				CRITICAL_SECTION { timeOut = 0; }
				SetMode(MAIN);
			}
		}
		void Set_TimeTout(unsigned char k)
		{
			CRITICAL_SECTION { timeOut = 0; }
			SetMode(MAIN);
		}
		// ==============================
		uint16_t viewarc_idx;
		uint16_t viewarc_n;
		void view_arcSh()
		{
			archiveStruct cell;
			uint16_t idx = viewarc_idx + viewarc_n;
			while (idx >= ARCHIVE_LEN) idx -= ARCHIVE_LEN;
			//
			eeprom_read_block(&cell, &ns_archive::archive[idx], sizeof(archiveStruct));
			uint8_t pos = scr::SetPosition(0, 1);
			scr::DigitZ(&pos, 2, cell.hour);
			scr::PutChar(&pos, ':');
			scr::DigitZ(&pos, 2, cell.minute);
			pos++;
			//scr::DigitZ(&pos, 3, viewarc_n + 1);
			scr::DigitZ(&pos, 3, cell.nTube);
			pos++;
			scr::DigitZ(&pos, 5, cell.len);
			if (cell.ok == 0) scr::PutChar(&pos, 'D');
			else			  scr::PutChar(&pos, ' ');
			//
			pos = scr::SetPosition(6, 0);
			scr::DigitZ(&pos, 2, cell.date);
			scr::PutChar(&pos, '-');
			scr::DigitZ(&pos, 2, cell.mounth);
			//scr::PutChar(&pos, '-');
			scr::String_P(&pos, PSTR("-20"));
			scr::DigitZ(&pos, 2, cell.year);
		}
		void view_arc(uint8_t k)
		{
			timeOut = 0;
			key4->autoRepeat = false;
			scr::Clear();
			scr::String_P(scr::SetPosition(0, 0), PSTR("View"));
			CRITICAL_SECTION
			{
				uint16_t lenA = ns_archive::len;
				if (lenA >= ARCHIVE_LEN) lenA = 0;
				if (ns_archive::idx < ns_archive::len)
				{
					viewarc_idx = ns_archive::idx + ARCHIVE_LEN - lenA;
				}
				else
				{
					viewarc_idx = ns_archive::idx - lenA;
				}
			}
			viewarc_n = 0;
			if ( (IDX_ee_read(&ns_archive::lenEE) == 0) && (ns_archive::len == 0) )
			{
				scr::String_P(scr::SetPosition(0, 1), PSTR("Архив пуст"));
				timeOut = 10000;
				SetMode(TIMEOUT_TO_MAIN);
				return;
			}
			view_arcSh();
		}
		void view_arcMulti(uint8_t k)
		{
			uint16_t idx = viewarc_idx + viewarc_n;
			while (idx >= ARCHIVE_LEN) idx -= ARCHIVE_LEN;
			key4->autoRepeat = false;
			if (k == 3) eeprom_update_byte(&ns_archive::archive[idx].ok, 0);
			if (k == 5) eeprom_update_byte(&ns_archive::archive[idx].ok, 1);
			view_arcSh();
		}
		void view_arcEn(uint8_t k)
		{
			key4->autoRepeat = false;
			uint16_t idx = viewarc_idx + viewarc_n;
			while (idx >= ARCHIVE_LEN) idx -= ARCHIVE_LEN;
			uint8_t st = eeprom_read_byte(&ns_archive::archive[idx].ok);
			if (st)
				eeprom_update_byte(&ns_archive::archive[idx].ok, 0);
			else
				eeprom_update_byte(&ns_archive::archive[idx].ok, 1);
			view_arcSh();
		}
		void view_arcMinus(uint8_t k)
		{
			key4->autoRepeat = true;
			if (viewarc_n > 0) viewarc_n--;
			view_arcSh();
		}
		void view_arcPlus(uint8_t k)
		{
			key4->autoRepeat = true;
			uint16_t ln = IDX_ee_read(&ns_archive::lenEE);
			if (ns_archive::len > ln) ln = ns_archive::len;
			if (viewarc_n < (ln - 1) ) viewarc_n++;
			view_arcSh();
		}
		void view_arcBack(uint8_t k)
		{
			key4->autoRepeat = false;
			SetMode(MAIN);
		}
		// ==============================
		uint8_t corCurTube_h, corCurTube_l;
		//int8_t  corCurTube_Cor;
		uint16_t corCurTube_dlina;
		uint16_t corCurTube_base;
		void CorCurTubeSh()
		{
			scr::DigitZ(scr::SetPosition(4, 1), 5, corCurTube_base);
			//scr::DigitZ(scr::SetPosition(10, 1), 5, corCurTube_dlina + corCurTube_Cor);
			scr::DigitZ(scr::SetPosition(10, 1), 5, corCurTube_dlina);
		}
		void CorCurTube(uint8_t k)
		{
			CRITICAL_SECTION { timeOut = 0; }
			key4->autoRepeat = false;
			timeOut = 0;
			scr::Clear();
			scr::String_P(scr::SetPosition(0, 0), PSTR("Edit len"));
			if ( (ns_archive::len == 0) && (eeprom_read_word(&ns_archive::lenEE) == 0) )
			{
				//
				SetMode(MAIN);
			}
			corCurTube_idx = ns_archive::idx;
			if (corCurTube_idx == 0) corCurTube_idx = ARCHIVE_LEN - 1;
			else corCurTube_idx--;
			eeprom_read_block(&corCurTube_cell, &ns_archive::archive[corCurTube_idx], sizeof(archiveStruct));
			uint8_t bas;
			if (corCurTube_cell.sqBas < 0) bas = -corCurTube_cell.sqBas;
			else bas = corCurTube_cell.sqBas;
			//corCurTube_h = bas / 10;
			//scr::DigitZ(scr::SetPosition(0, 1), 2, corCurTube_cell.sqBas);
			corCurTube_l = (bas % 10);
			//eeprom_read_block(&corCurTube_Cor, &ns_vg::eeDistanceCor[corCurTube_h][corCurTube_l], 1);
			corCurTube_dlina = corCurTube_cell.len;
			scr::DigitZ(scr::SetPosition(10, 0), 5, corCurTube_dlina);
			//corCurTube_dlina -= corCurTube_Cor;
			{
				corCurTube_base = eeprom_read_word(&ns_vg::eeDistance[corCurTube_l]) - eeprom_read_word(&ns_vg::eeDistance[0]);
				/*int16_t tmpDn;
				int16_t tmpD0;
				eeprom_read_block(&tmpDn, &ns_vg::eeDistance[corCurTube_l], sizeof(int16_t) );
				eeprom_read_block(&tmpD0, &ns_vg::eeDistance[0], sizeof(int16_t) );
				corCurTube_base = tmpDn - tmpD0;*/
			}
			scr::String_P(scr::SetPosition(0, 1), PSTR("SQ"));
			scr::DigitZ(scr::SetPosition(2, 1), 1, corCurTube_l+1);
			scr::String_P(scr::SetPosition(3, 1), PSTR(":"));
			scr::String_P(scr::SetPosition(9, 1), PSTR("<"));
			CorCurTubeSh();
			CRITICAL_SECTION { timeOut = TIMEOUT_MENU; }
		}
		void CorCurTubeMinus(uint8_t k)
		{
			key4->autoRepeat = true;
			//if (corCurTube_Cor > -100)
			{
				//corCurTube_Cor--;
				corCurTube_base--;
				corCurTube_dlina--;
				CorCurTubeSh();
			}
			CRITICAL_SECTION { timeOut = TIMEOUT_MENU; }
		}
		void CorCurTubePlus(uint8_t k)
		{
			key4->autoRepeat = true;
			//if (corCurTube_Cor < 100)
			{
				//corCurTube_Cor++;
				corCurTube_base++;
				corCurTube_dlina++;
				CorCurTubeSh();
			}
			CRITICAL_SECTION { timeOut = TIMEOUT_MENU; }
		}
		void CorCurTubeBack(uint8_t k)
		{
			CRITICAL_SECTION { timeOut = 0; }
			key4->autoRepeat = false;
			SetMode(MAIN);
		}
		void CorCurTubeEn(uint8_t k)
		{
			CRITICAL_SECTION { timeOut = 0; }
			key4->autoRepeat = false;
			//eeprom_update_block(&corCurTube_Cor, &ns_vg::eeDistanceCor[corCurTube_h][corCurTube_l], 1);
			eeprom_update_word(&ns_vg::eeDistance[corCurTube_l], corCurTube_base);
			SetMode(MAIN);
		}
		// ==============================
		void ZeroArc(uint8_t k)
		{
			key4->autoRepeat = false;
			scr::Clear();
			scr::String_P(scr::SetPosition(0, 0), PSTR("для удаления"));
			scr::String_P(scr::SetPosition(0, 1), PSTR("нажмите \"+\""));
//			scr::String_P(scr::SetPosition(4, 1), PSTR("нет"));
//			scr::String_P(scr::SetPosition(8, 1), PSTR("да"));
			CRITICAL_SECTION{ timeOut = TIMEOUT_MENU; }
		}
		void ZeroArc_Yes(uint8_t k)
		{
			CRITICAL_SECTION{ timeOut = 0; }
			ns_archive::SummZero();
			SetMode(MAIN);
		}
		void ZeroArc_Menu(uint8_t k)
		{
			CRITICAL_SECTION{ timeOut = 0; }
			SetMode(MENU_SELECT);
		}
		void ZeroArc_TimeOut(uint8_t k)
		{
			CRITICAL_SECTION{ timeOut = 0; }
			SetMode(MAIN);
		}
		// ==============================
		uint16_t tmpInterval;
		void ProtectIntervalView()
		{
			if (tmpInterval > 10000) tmpInterval = 10000;
			scr::DigitZ(scr::SetPosition(9, 1), 5, tmpInterval );
		}
		void ProtectInterval(uint8_t k)
		{
			CRITICAL_SECTION{ timeOut = 0; }
			key4->autoRepeat = false;
			tmpInterval = (uint16_t)( ((uint32_t)eeprom_read_word(&ns_vg::eeProtect) * ((uint32_t)1000) / ((uint32_t)FtTimerMain)   ));
			scr::Clear();
			scr::String_P(scr::SetPosition(0, 0), PSTR("Защитный") );
			scr::String_P(scr::SetPosition(0, 1), PSTR("интервал:") );
			ProtectIntervalView();
			CRITICAL_SECTION{ timeOut = TIMEOUT_MENU; }
		}
		void ProtectIntervalBack(uint8_t k)
		{
			CRITICAL_SECTION{ timeOut = 0; }
			SetMode(MENU_SELECT);
		}
		void ProtectIntervalPl(uint8_t k)
		{
			key4->autoRepeat = true;
			if (tmpInterval < 10000) tmpInterval++;
			ProtectIntervalView();
			CRITICAL_SECTION{ timeOut = TIMEOUT_MENU; }
		}
		void ProtectIntervalMn(uint8_t k)
		{
			key4->autoRepeat = true;
			if (tmpInterval > 10) tmpInterval--;
			ProtectIntervalView();
			CRITICAL_SECTION{ timeOut = TIMEOUT_MENU; }
		}
		void ProtectIntervalEn(uint8_t k)
		{
			CRITICAL_SECTION{ timeOut = 0; }
			key4->autoRepeat = false;
			eeprom_update_word(&ns_vg::eeProtect, (uint16_t)(   ((uint32_t)tmpInterval)  *  ((uint32_t)FtTimerMain) / ((uint32_t)1000)    ) );
			SetMode(MENU_SELECT);
		}
		// ==============================
		uint16_t tmIntegral;
		void TmIntegralView()
		{
			if (tmpInterval > 4000) tmIntegral = 4000;
			scr::DigitZ(scr::SetPosition(10, 1), 4, tmIntegral);
		}
		void TmIntegral(uint8_t k)
		{
			CRITICAL_SECTION { timeOut = 0; }
			key4->autoRepeat = false;
			tmIntegral =  (uint16_t)( ((uint32_t)eeprom_read_word(&ns_vg::eeInteral)) * ((uint32_t)1000) / ((uint32_t)FtTimerMain) );
			scr	::Clear();
			scr::String_P(scr::SetPosition(0, 0), PSTR("Время интер.") );
			scr::String_P(scr::SetPosition(0, 1), PSTR("сигналов: ") );
			TmIntegralView();
			CRITICAL_SECTION{ timeOut = TIMEOUT_MENU; }
		}
		void TmIntegralBc(uint8_t k)
		{
			CRITICAL_SECTION { timeOut = 0; }
			key4->autoRepeat = false;
			SetMode(MENU_SELECT);
		}
		void TmIntegralEn(uint8_t k)
		{
			CRITICAL_SECTION { timeOut = 0; }
			key4->autoRepeat = false;
			uint16_t di = (uint16_t)(  ((uint32_t)tmIntegral) * ((uint32_t)FtTimerMain) / ((uint32_t)1000)   );
			eeprom_update_word(&ns_vg::eeInteral, di);
			ns_line::sensors[0]->porog = di;
			ns_line::sensors[1]->porog = di;
			ns_line::sensors[2]->porog = di;
			ns_line::sensors[3]->porog = di;
			ns_line::sensors[4]->porog = di;
			SetMode(MENU_SELECT);
		}
		void TmIntegralMn(uint8_t k)
		{
			key4->autoRepeat = true;
			if (tmIntegral > 1) tmIntegral--;
			TmIntegralView();
			CRITICAL_SECTION{ timeOut = TIMEOUT_MENU; }
		}
		void TmIntegralPl(uint8_t k)
		{
			key4->autoRepeat = true;
			if (tmIntegral < 4000) tmIntegral++;
			TmIntegralView();
			CRITICAL_SECTION{ timeOut = TIMEOUT_MENU; }
		}
		// ==============================
		int8_t corSpeed;
		void CorSpeedView()
		{
			if (corSpeed < -120) corSpeed = -120;
			if (corSpeed >  120) corSpeed =  120;
			scr::DigitZ(scr::SetPosition(9, 1), 4, corSpeed);
		}
		void CorSpeed(uint8_t k)
		{
			CRITICAL_SECTION { timeOut = TIMEOUT_MENU; }
			key4->autoRepeat = false;
			corSpeed = (int8_t)(eeprom_read_byte(&ns_vg::eeCorSpeed) );
			scr::Clear();
			scr::String_P(scr::SetPosition(0, 0), PSTR("Коррекция") );
			scr::String_P(scr::SetPosition(0, 1), PSTR("скорости:") );
			CorSpeedView();
		}
		void CorSpeedEn(uint8_t k)
		{
			CRITICAL_SECTION { timeOut = 0; }
			key4->autoRepeat = false;
			eeprom_update_byte(&ns_vg::eeCorSpeed, (uint8_t)corSpeed);
			SetMode(MENU_SELECT);
		}
		void CorSpeedBc(uint8_t k)
		{
			CRITICAL_SECTION { timeOut = 0; }
			key4->autoRepeat = false;
			SetMode(MENU_SELECT);
		}
		void CorSpeedMn(uint8_t k)
		{
			CRITICAL_SECTION { timeOut = TIMEOUT_MENU; }
			key4->autoRepeat = true;
			if (corSpeed > -120) corSpeed--;
			CorSpeedView();
		}
		void CorSpeedPl(uint8_t k)
		{
			CRITICAL_SECTION { timeOut = TIMEOUT_MENU; }
			key4->autoRepeat = true;
			if (corSpeed < 120) corSpeed++;
			CorSpeedView();
		}
		// ==============================
		// ==============================
		// ==============================
		// ==============================
		// ==============================
		// ==============================
	// ============================================
	// 0 : no key (view)
	// 1-4 : main key
	// 5 : add key
	// 6 : timeout
	const TyFnMn MassFnMn[][8] PROGMEM =
	{//					view				key1				key2				key3				key4				key5			 setmode			 timeout          
		{	       Main_View,	       Main_menu,				Dupm,				Dupm,				Dupm,		 Main_ViewEx,       Main_SetMode,				Dupm },	// 0 main
		{ Menu_CheckPassView, Menu_CheckPassBack,Menu_CheckPassMinus, Menu_CheckPassPlus,	Menu_CheckPassVv,				Dupm,     Menu_CheckPass, Menu_CheckPassTout },	// 1 menu check pass
		{				Dupm,	 Menu_SelectBack,	Menu_SelectMinus,	 Menu_SelectPlus,	   Menu_SelectVV,				Dupm,		 Menu_Select,	 Menu_SelectTout },	// 2 menu select
		{				Dupm,	 Edit_SensorBack,	Edit_SensorMinus,	 Edit_SensorPlus,	   Edit_SensorVV,				Dupm,		 Edit_Sensor,	 Edit_SensorTout }, // 3 edit sensor
		{				Dupm,				Dupm,				Dupm,				Dupm,				Dupm,				Dupm,				Dupm,	  TimeoutToMainE }, // 4 timeout to main
		{				Dupm,		Set_PassBack,	   Set_PassMinus,		Set_PassPlus,		  Set_PassVV,				Dupm,			Set_Pass,		Set_PassTout }, // 5 set password
		{				Dupm,				Dupm,				Dupm,				Dupm,				Dupm,				Dupm,				Dupm,	  TimeoutToMnSel }, // 6 TIMEOUT_TO_MNSEL
		{				Dupm,		Set_TimeBack,	   Set_TimeMinus,		Set_TimePlus,		 Set_TimeEnt,				Dupm,			Set_Time,		Set_TimeTout }, // 7 установка времени
		{				Dupm,		view_arcBack,	   view_arcMinus,		view_arcPlus,		  view_arcEn,				Dupm,			view_arc,				Dupm },	// 8 просмотр архива
		{				Dupm,	  CorCurTubeBack,	 CorCurTubeMinus,	  CorCurTubePlus,		CorCurTubeEn,				Dupm,		  CorCurTube,	 ZeroArc_TimeOut }, // 9 коррекция по длине трубы
		{				Dupm,		ZeroArc_Menu,		ZeroArc_Menu,		 ZeroArc_Yes,		ZeroArc_Menu,				Dupm,			 ZeroArc,	 ZeroArc_TimeOut }, // 10 подтверждение сброса архива
		{				Dupm,ProtectIntervalBack,  ProtectIntervalMn,  ProtectIntervalPl,  ProtectIntervalEn,				Dupm,	 ProtectInterval,	 ZeroArc_TimeOut }, // 11 защитный интервал
		{				Dupm,		TmIntegralBc,		TmIntegralMn,		TmIntegralPl,		TmIntegralEn,				Dupm,		  TmIntegral,	 ZeroArc_TimeOut }, // 12 время интегрирования
		{				Dupm,		  CorSpeedBc,		  CorSpeedMn,		  CorSpeedPl,		  CorSpeedEn,				Dupm,			CorSpeed,	 ZeroArc_TimeOut },
		{				Dupm,				Dupm,				Dupm,				Dupm,				Dupm,				Dupm,				Dupm,				Dupm },
		{				Dupm,				Dupm,				Dupm,				Dupm,				Dupm,				Dupm,				Dupm,				Dupm }
	};

}
