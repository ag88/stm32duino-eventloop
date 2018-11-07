/* A stm32duino eventloop
 * Copyright (c) 2018 Andrew Goh
 * Provided as-is, No warranties of any kind, Use at your own risk
 * License: MIT
*/
#include <Arduino.h>
#include <libmaple/systick.h>
#include "eventloop/ASyncWait.h"
#include "eventloop/Event.h"
#include "eventloop/EventLoop.h"
#include "eventloop/RingBuffer.h"
#include "eventloop/util.h"


#include "tasks/CKeySenderTask.h"
#include "tasks/CLedTask.h"

void systick_handler(void);


uint16_t systick_fired = 0;

//------------------------------------------------------------------------------
void setup() {


	pinMode(BOARD_LED_PIN, OUTPUT);
	digitalWrite(BOARD_LED_PIN, HIGH);

	Serial.begin(115200);

	delay(1000);

	// Throw away serial input
	while (Serial.available()) Serial.read();

	//Initialize Eventloop
	EventLoop.registerhandler(EHandleID::KeySenderTask, KeySenderTask);
	EventLoop.registerhandler(EHandleID::LedTask, LedTask);

	digitalWrite(BOARD_LED_PIN, LOW);

	Serial.print(F("FreeStack:"));
	Serial.println(FreeStack());

	systick_attach_callback(systick_handler);
	systick_fired = 0;

	Event event;
	event.handle_id = EHandleID::BroadCast;
	event.event = EventID::AppStart;
	EventLoop.post(event);

}


void loop() {

	//interrupts();
	if(systick_fired) {
		systick_fired = 0;

		//process waits
		WaitMgr.processwaits();

		//systick event is pre-emptive
		TEvent event;
		event.handle_id = EHandleID::BroadCast;
		event.event = EventID::Systick;
		EventLoop.proceventimm(event);

		//then we process the event queue
		EventLoop.processevent();

	}
	//noInterrupts();
	//wait for interrupts and events
	asm("wfe");
}


void systick_handler(void) {
	systick_fired++;
}
