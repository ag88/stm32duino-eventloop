/* A stm32duino eventloop
 * Copyright (c) 2018 Andrew Goh
 * Provided as-is, No warranties of any kind, Use at your own risk
 * License: MIT
*/

#ifndef EVENT_H_
#define EVENT_H_
#include <Arduino.h>

enum class EventID : uint16_t {
	Systick = 1,
	Poweroff,
	Sleep,
	DeepSleep,
	ScreenOff,
	ScreenOn,
	BattLow,
	AppStart = 100,
	AwaitComplete,
	KeyEvent = 1000,
	Key1,
	Key2,
	Key3,
	Key4,
	M_SELECT,
	M_BACK,
	M_NEXT,
	M_PREV,
	BeepOn,
	BeepOff,
	LedTaskLedOn,
	LedTaskLedOff,
	LedTaskFaster,
	LedTaskSlower,
	LedTaskShowMem,
	LedTaskProcData
};

enum class EHandleID : uint8_t {
	NoTask = 0,
	KeySenderTask = 1,
	LedTask,
	BroadCast = 255
};

typedef struct Event {
	EHandleID 	handle_id;
	//uint8_t	priority;
	EventID 	event;
	int			param1;
	int			param2;
} TEvent;

class CEventHandler {
public:
	virtual ~CEventHandler();

	virtual bool handleEvent(Event& event) = 0;
};


#endif
