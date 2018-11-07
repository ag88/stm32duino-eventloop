/* A stm32duino eventloop
 * Copyright (c) 2018 Andrew Goh
 * Provided as-is, No warranties of any kind, Use at your own risk
 * License: MIT
*/

#ifndef ASYNCWAIT_H_
#define ASYNCWAIT_H_
#include "../eventloop/Event.h"
#include "../eventloop/EventLoop.h"

#define MAX_WAIT_SLOTS	32


struct SWait {
	union {
		struct {
			uint8_t bactive :1; //wait is active
			uint8_t bclaimed :1; //claimed
			uint8_t brepeat :1; //repeats
			uint8_t bimm :1; 	//immediate
		} bits;
		uint8_t flags;
	};
	uint32_t last_fired;
	uint32_t duration;
	Event event;
};

class CAsyncWait {
public:
	CAsyncWait();

	void setwait(uint32_t duration, bool repeat, Event& event, bool imm);
	void setwait(uint32_t duration, bool repeat);
	void setevent(Event& event, bool imm);
	void setrepeat(bool repeat);
	uint32_t getdur();
	bool isactive();
	bool isclaimed();
	void setclaimed(bool claimed);
	void await();
	void awaituntil();
	void init();
	void stop();
	void cancel();

	void processwait();

private:
	SWait m_wait;

};

class CWaitMgr {
public:
	CWaitMgr();

	uint8_t getHandle();
	void releaseHandle(uint8_t handle);
	CAsyncWait& get(uint8_t handle);

	void processwaits();

private:
	CAsyncWait m_wait[MAX_WAIT_SLOTS];

};

extern CWaitMgr WaitMgr;


#endif
