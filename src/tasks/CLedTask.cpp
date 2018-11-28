/* A stm32duino eventloop
 * Copyright (c) 2018 Andrew Goh
 * Provided as-is, No warranties of any kind, Use at your own risk
 * License: MIT
*/
#include "CLedTask.h"

#include <Arduino.h>
#include "../eventloop/ASyncWait.h"
#include "../eventloop/util.h"
#include "../mainloop.h"


CLedTask::CLedTask(void) {
	state = LedState::Init;

	hWaitLed = 0;

	// duration for 1/2 period i.e. on or off
	// 1 seconds initially
	ledduration = 1000;

}

CLedTask::~CLedTask() {
}


bool CLedTask::handleEvent(Event& event) {


	switch(event.event) {
	case EventID::AppStart:
		setup();
		break;
	case EventID::LedTaskLedOn:
		ledon();
		break;
	case EventID::LedTaskLedOff:
		ledoff();
		break;
	case EventID::LedTaskFaster:
		faster();
		break;
	case EventID::LedTaskSlower:
		slower();
		break;
	case EventID::LedTaskShowMem:
		printmem();
		break;
	default:
		break;
	}

	return true;
}

void CLedTask::setup() {

	ledon();

}


void CLedTask::ledon() {
	Event event;

	digitalWrite(BOARD_LED_PIN, HIGH);

	event.handle_id = EHandleID::LedTask;
	event.event = EventID::LedTaskLedOff;
	WaitMgr.await(ledduration,event,false);
}


void CLedTask::ledoff() {
	Event event;

	digitalWrite(BOARD_LED_PIN, LOW);

	event.handle_id = EHandleID::LedTask;
	event.event = EventID::LedTaskLedOn;
	WaitMgr.await(ledduration,event,false);
}

void CLedTask::faster() {
	ledduration /= 2;
}

void CLedTask::slower() {
	ledduration *= 2;
}

void CLedTask::printmem() {
 	Serial.print(F("FreeStack:"));
 	Serial.println(FreeStack());
}

CLedTask LedTask;
