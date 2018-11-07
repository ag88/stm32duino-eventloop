/* A stm32duino eventloop
 * Copyright (c) 2018 Andrew Goh
 * Provided as-is, No warranties of any kind, Use at your own risk
 * License: MIT
*/
#include "ASyncWait.h"

CAsyncWait::CAsyncWait() {
	m_wait.flags = 0x0;
	m_wait.last_fired = 0;
	m_wait.duration = 0;
	m_wait.event = Event();
}

void CAsyncWait::setwait(uint32_t duration, bool repeat,Event& event, bool imm) {
	m_wait.duration = duration;
	m_wait.bits.brepeat = repeat;
	m_wait.event = event;
	m_wait.bits.bimm = imm;
}

void CAsyncWait::setwait(uint32_t duration, bool repeat) {
	m_wait.duration = duration;
	m_wait.bits.brepeat = repeat;
}

void CAsyncWait::setevent(Event& event, bool imm) {
	m_wait.event = event;
	m_wait.bits.bimm = imm;
}

void CAsyncWait::setrepeat(bool repeat) {
	m_wait.bits.brepeat = repeat;
}

uint32_t CAsyncWait::getdur() {
	return m_wait.duration;
}

void CAsyncWait::await() {
	m_wait.last_fired = systick_uptime();
	m_wait.bits.bactive = true;
}

void CAsyncWait::awaituntil() {
	m_wait.bits.bactive = true;
}

void CAsyncWait::init() {
	m_wait.last_fired = systick_uptime();
}

void CAsyncWait::stop() {
	m_wait.last_fired = systick_uptime();
	m_wait.bits.bactive = false;
}

void CAsyncWait::cancel() {
	m_wait.bits.bactive = false;
}

bool CAsyncWait::isclaimed() {
	return m_wait.bits.bclaimed;
}

bool CAsyncWait::isactive() {
	return m_wait.bits.bactive;
}

void CAsyncWait::setclaimed(bool claimed) {
	m_wait.bits.bclaimed = claimed;
}

void CAsyncWait::processwait() {
	uint32_t uptime_millis = systick_uptime();
	if(m_wait.bits.bactive ) {
		if( m_wait.last_fired + m_wait.duration <= uptime_millis) {
			m_wait.last_fired = uptime_millis;
			if(m_wait.bits.bimm)
				EventLoop.proceventimm(m_wait.event);
			else
				EventLoop.post(m_wait.event);
			if(!m_wait.bits.brepeat) //single shot
				m_wait.bits.bactive = false;
		}
	}
}

CWaitMgr::CWaitMgr() {
	for(int i=0;i<MAX_WAIT_SLOTS;i++)
		m_wait[i] = CAsyncWait();
}

uint8_t CWaitMgr::getHandle() {
	for(int i=0;i<MAX_WAIT_SLOTS;i++) {
		if(!m_wait[i].isclaimed()) {
			Event event;
			m_wait[i].stop();
			m_wait[i].setwait(0,false,event,false);
			m_wait[i].setclaimed(true);
			return i+1;
		}
	}
	Serial.print(F("no more wait slots, max:"));
	Serial.println(MAX_WAIT_SLOTS);
	return 0;
}

void CWaitMgr::releaseHandle(uint8_t handle) {
	m_wait[handle-1].stop();
	m_wait[handle-1].setclaimed(false);
}

CAsyncWait& CWaitMgr::get(uint8_t handle) {
	CAsyncWait& wait = m_wait[handle-1];
	return wait;
}


void CWaitMgr::processwaits() {
	for(int i=0;i<MAX_WAIT_SLOTS;i++) {
		if(m_wait[i].isclaimed())
			m_wait[i].processwait();
	}
}

CWaitMgr WaitMgr;
