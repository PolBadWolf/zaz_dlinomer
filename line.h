/*
 * line.h
 *
 * Created: 13.01.2017 19:13:13
 *  Author: User
 */ 


#ifndef LINE_H_
#define LINE_H_

namespace ns_line
{
	void Init();
	void Interrupt();
	uint8_t Render(uint16_t* dlina, int8_t* sqBas);
	extern tc_pin<uint16_t>* sensors[];
	extern volatile uint32_t times[5][2];
}



#endif /* LINE_H_ */