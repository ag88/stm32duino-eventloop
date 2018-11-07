/* A stm32duino eventloop
 * Copyright (c) 2018 Andrew Goh
 * Provided as-is, No warranties of any kind, Use at your own risk
 * License: MIT
*/

#ifndef KEYSENDERTASK_H_
#define KEYSENDERTASK_H_

#include "../eventloop/Event.h"
#include "../mainloop.h"


class CKeySenderTask : public CEventHandler {

public:

	CKeySenderTask(void); //constructor
	~CKeySenderTask();

	bool handleEvent(Event& event) ;

private:


//	uint8_t hwait;

//	uint8_t evnum;


	void setup();

	void sendkey();

	bool handlekey(int c);

	void showhelp();

	void faster();

	void slower();

	void showmem();
};

extern CKeySenderTask KeySenderTask;


#endif
