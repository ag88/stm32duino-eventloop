/* A stm32duino eventloop
 * Copyright (c) 2018 Andrew Goh
 * Provided as-is, No warranties of any kind, Use at your own risk
 * License: MIT
*/
#include "EventLoop.h"

#include "Event.h"


CEventLoop::CEventLoop() {
	m_eventbuffer = RingBuffer<TEvent,EVENT_BUFFER_SIZE>::getInstance();
	m_numhandler = 0;
	for(int i=0; i<MAX_EVENT_HANDLERS; i++) {
		m_event_handlers[i].handle_id = EHandleID::NoTask;
		m_event_handlers[i].handler = NULL;
	}

}

uint8_t CEventLoop::registerhandler(EHandleID handle_id, CEventHandler& eventhandler) {
	if(m_numhandler+1 > MAX_EVENT_HANDLERS) {
		Serial.print(F("MAX_EVEN_HANDLERS exceeded:"));
		Serial.println(MAX_EVENT_HANDLERS);
		return 0;
	}
	m_event_handlers[m_numhandler].handle_id = handle_id;
	m_event_handlers[m_numhandler].handler = &eventhandler;
	m_numhandler++;
	return static_cast<uint8_t>(handle_id);
}


void CEventLoop::removehandler(EHandleID handler_id) {
	if(m_numhandler > MAX_EVENT_HANDLERS)
		return;
	bool found = false;
	uint8_t hindex = 0;
	for(int i=0; i<m_numhandler; i++) {
		if(m_event_handlers[m_numhandler].handle_id == handler_id) {
			hindex = i;
			found = true;
			break;
		}
	}
	if(found) {
		for(int i=hindex; i<m_numhandler-1; i++) {
			m_event_handlers[i].handle_id = m_event_handlers[i+1].handle_id;
			m_event_handlers[i].handler = m_event_handlers[i+1].handler;
		}
		m_event_handlers[m_numhandler-1].handle_id = EHandleID::NoTask;
		m_event_handlers[m_numhandler-1].handler = NULL;
		m_numhandler--;
	}
}

void CEventLoop::setup() {
}


int8_t CEventLoop::post(Event& event) {

	if(m_eventbuffer->is_full()) return -1;
	noInterrupts();
	m_eventbuffer->push(event);
	interrupts();

//	Serial.print("post:");
//	uint16_t ev = static_cast<uint16_t>(event.event);
//	Serial.println(ev);


	return 0;
}


bool CEventLoop::proceventimm(Event& event) {
	//		Serial.print("pop:");
	//		uint16_t ev = static_cast<uint16_t>(event.event);
	//		Serial.println(ev);
	bool ret = false;

	if (event.handle_id == EHandleID::BroadCast) { //broadcast
		for (int i = 0; i < m_numhandler; i++) {
			m_event_handlers[i].handler->handleEvent(event);
		}
	} else {
		for (int i = 0; i < m_numhandler; i++) {
			if (m_event_handlers[i].handle_id == event.handle_id) {
				ret = m_event_handlers[i].handler->handleEvent(event);
				break;
			}
		}
	}
	return ret;
}


void CEventLoop::processevent() {

	while(!m_eventbuffer->is_empty()) {
		noInterrupts();
		TEvent *pevent = m_eventbuffer->pop();
		interrupts();
		if(pevent == NULL) continue;

		proceventimm(*pevent);
	}

}

CEventLoop EventLoop;

