/* A stm32duino eventloop
 * Copyright (c) 2018 Andrew Goh
 * Provided as-is, No warranties of any kind, Use at your own risk
 * License: MIT
*/

#ifndef EVENTLOOP_H_
#define EVENTLOOP_H_
#include "Event.h"
#include "RingBuffer.h"

#define MAX_EVENT_HANDLERS	20

/*
 * eize of the event loop buffer
 * must be power of 2
 */
#define EVENT_BUFFER_SIZE 128

struct SEventHandlers {
	EHandleID handle_id;
	CEventHandler *handler;
};

class CEventLoop {

public:
	CEventLoop();

	/*
	 * 	@brief	setup - register handlers
	 *
	 */
	uint8_t registerhandler(EHandleID handler_id, CEventHandler& eventhandler);

	void removehandler(EHandleID handler_id);

	void setup();

	/*
	 * @brief post new event
	 * @details	post new event
	 * @param[in]	task	R-value	reference	to	new	handler	functor.
	 * @return	0	in case	the	handler	was	successfully	posted,
	 * 			-1	if there is not enough space in the execution
	 */
	int8_t post(Event& event);

	int8_t postfromInt(Event& event);

	void processevent();

	bool proceventimm(Event& event);


private:
	RingBuffer<TEvent,EVENT_BUFFER_SIZE> *m_eventbuffer;

	uint8_t m_numhandler;
	SEventHandlers m_event_handlers[MAX_EVENT_HANDLERS];

};

extern CEventLoop EventLoop;

#endif
