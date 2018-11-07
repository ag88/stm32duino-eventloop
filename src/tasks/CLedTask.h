/* A stm32duino eventloop
 * Copyright (c) 2018 Andrew Goh
 * Provided as-is, No warranties of any kind, Use at your own risk
 * License: MIT
*/

#ifndef LEDTASK_H_
#define LEDTASK_H_

#include "../eventloop/Event.h"

class CLedTask : public CEventHandler {

public:

	CLedTask(void); //constructor
	~CLedTask();

	bool handleEvent(Event& event) ;

private:

	enum class LedState:uint8_t { Init=0, On, Off };

	LedState state;

	int ledduration;

	uint8_t hWaitLed;

	void setup();

	void ledon();

	void ledoff();

	void faster();

	void slower();

	void printmem();

};

extern CLedTask LedTask;

#endif
