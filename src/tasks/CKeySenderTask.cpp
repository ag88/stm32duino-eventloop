/* A stm32duino eventloop
 * Copyright (c) 2018 Andrew Goh
 * Provided as-is, No warranties of any kind, Use at your own risk
 * License: MIT
*/
#include "CKeySenderTask.h"

#include <Arduino.h>
#include "../eventloop/ASyncWait.h"
#include "../eventloop/EventLoop.h"

CKeySenderTask::CKeySenderTask(void) {
}

CKeySenderTask::~CKeySenderTask() {
}

bool CKeySenderTask::handleEvent(Event& event) {

	switch(event.event) {
	case EventID::AppStart:
		setup();
		break;
	case EventID::Systick:
		sendkey();
		break;
	default:
		break;
	}
	return 0;
}

void CKeySenderTask::setup() {
	showhelp();

}

void CKeySenderTask::showhelp() {
	Serial.println(F("f - faster, s - slower, m - memory"));
}

void CKeySenderTask::sendkey() {
	// read serial input
	while (Serial.available()) {
		Event event;
		int c = Serial.read();
		if(c != -1) {
			if(!handlekey(c)) { //send the key
				event.handle_id = EHandleID::BroadCast;
				event.event = EventID::KeyEvent;
				event.param1 = c;
				EventLoop.post(event);
			}
		}
	}
}

bool CKeySenderTask::handlekey(int c) {

	bool flag = false;

	switch (c) {
	case 's':
	case 'S':
		slower();
		flag = true;
		break;
	case 'f':
	case 'F':
		faster();
		flag = true;
		break;
	case 'm':
	case 'M':
		showmem();
		flag = true;
		break;
	case '?':
	case 'h':
	case 'H':
		showhelp();
		flag = true;
		break;
	default:
		break;
	}

	if(c == '\r')
		Serial.println();
	else
		Serial.print((char) c);

	return flag;
}


void CKeySenderTask::faster() {
	Event event;
	event.handle_id = EHandleID::LedTask;
	event.event = EventID::LedTaskFaster;
	EventLoop.post(event);
}

void CKeySenderTask::slower() {
	Event event;
	event.handle_id = EHandleID::LedTask;
	event.event = EventID::LedTaskSlower;
	EventLoop.post(event);
}


void CKeySenderTask::showmem() {
	Event event;
	event.handle_id = EHandleID::LedTask;
	event.event = EventID::LedTaskShowMem;
	EventLoop.post(event);
}


CKeySenderTask KeySenderTask;

