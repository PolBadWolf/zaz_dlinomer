/*
 * path.cpp
 *
 * Created: 27.08.2016 20:00:31
 *  Author: User
 */ 

 #include "path.h"
 #include <stdlib.h>
 #include <avr/interrupt.h>

 void *operator new(size_t s)
 {
	 return malloc(s);
 }
 
 void *operator new[](size_t s)
 {
	 return malloc(s);
 }
 
 void operator delete(void *m)
 {
	 free(m);
 }
 
 void operator delete[](void *m)
 {
	 free(m);
 }

const char tabWin2Avr[] PROGMEM =
{
	200,	201,	202,	203,	215,	216,	217,	218,	219,	220,	237,	253,	238,	239,	240,	241,
	242,	244,	204,	205,	206,	207,	208,	209,	210,	211,	212,	213,	214,	221,	222,	223,
	231,	232,	233,	234,	235,	236,	243,	245,	162,	246,	247,	248,	249,	250,	251,	252,
	254,	255,	32,		32,		32,		32,		32,		32,		181,	32,		32,		32,		32,		32,		32,		32,
	65,		160,	66,		161,	224,	69,		163,	164,	165,	166,	75,		167,	77,		72,		79,		168,
	80,		67,		84,		169,	170,	88,		225,	171,	172,	226,	173,	174,	98,		175,	176,	177,
	97,		178,	179,	180,	227,	101,	182,	183,	184,	185,	186,	187,	188,	189,	111,	190,
	112,	99,		191,	121,	228,	120,	229,	192,	193,	230,	194,	195,	196,	197,	198,	199
};


//FILE*	myStdOut1;
//FILE*	myStdOut2;
//FILE*	myStdOut3;
//tc_key4*	key4 = new tc_key4;
//FILE*		fSend232;
