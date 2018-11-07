/**
 * https://github.com/AndersKaloer/Ring-Buffer
 * Copyright (c) 2014 Anders Kalør
 * Provided as-is, No warranties of any kind, Use at your own risk
 */

#ifndef RINGBUFFER_TPP
#define RINGBUFFER_TPP
#include <Arduino.h>

#include "RingBuffer.h"

template <class T, ring_buffer_size_t NBUFSIZE>
RingBuffer<T, NBUFSIZE>::RingBuffer() {

	tail_index = 0;
	head_index = 0;
	//RBmutex = xSemaphoreCreateMutex();
}

template<class T, ring_buffer_size_t NBUFSIZE>
RingBuffer<T, NBUFSIZE>* RingBuffer<T, NBUFSIZE>::getInstance() {
    static RingBuffer instance; // Guaranteed to be destroyed.
                          // Instantiated on first use.
    return &instance;
};



template <class T, ring_buffer_size_t NBUFSIZE>
int RingBuffer<T, NBUFSIZE>::push(T& data) {

	/* Is buffer full? */
	if (is_full()) {
		/* Is going to overwrite the oldest byte */
		/* Increase tail index */
		//tail = ((tail + 1) & RingBufferMask);
		return -1; // do not overwrite
	}

	//mutex_lock();
	/* Place data in buffer */
	buffer[head_index] = data;
	head_index = ((head_index + 1) & RingBufferMask);
	//mutex_unlock();

	return 0;
}

template<class T, ring_buffer_size_t NBUFSIZE>
T* RingBuffer<T, NBUFSIZE>::pop() {

	T* pitem = NULL;
	if (is_empty()) {
		/* No items */
		return NULL;
	}

	//mutex_lock();
	pitem = &buffer[tail_index];
	tail_index = ((tail_index + 1) & RingBufferMask);
	return pitem;
	//mutex_unlock();

}

template<class T, ring_buffer_size_t NBUFSIZE>
int8_t RingBuffer<T, NBUFSIZE>::popn(ring_buffer_size_t n) {

	if (n > num_items()) {
		return -1;
	}

	//mutex_lock();
	tail_index = ((tail_index + n) & RingBufferMask);
	//mutex_unlock();

	return 0;
}



template<class T, ring_buffer_size_t NBUFSIZE>
T* RingBuffer<T, NBUFSIZE>::peek(ring_buffer_size_t index) {

	if (index >= num_items()) {
		/* No items at index */
		return NULL;
	}

	//mutex_lock();
	/* Add index to pointer */
	ring_buffer_size_t data_index = ((tail_index + index) & RingBufferMask);
	return &buffer[data_index];
	//mutex_unlock();
}


template<class T, ring_buffer_size_t NBUFSIZE>
uint8_t RingBuffer<T, NBUFSIZE>::is_full() {

	return ((head_index - tail_index) & RingBufferMask) == RingBufferMask;
}

template<class T, ring_buffer_size_t NBUFSIZE>
uint8_t RingBuffer<T, NBUFSIZE>::is_empty() {

	return (head_index == tail_index);
}

template<class T, ring_buffer_size_t NBUFSIZE>
inline ring_buffer_size_t RingBuffer<T, NBUFSIZE>::num_items() {

	return ((head_index - tail_index) & RingBufferMask);
}


//Ring buffer iterator implementation

template<class T1, ring_buffer_size_t NSIZE1>
RBIterator<T1, NSIZE1>::RBIterator() {
	m_ringbuffer = RingBuffer<T1,NSIZE1>::getInstance();
	m_nitems = m_ringbuffer->num_items();
	m_index = 0;
	if(m_nitems == 0)
		m_isdone = true;
	else
		m_isdone = false;
}

template<class T1, ring_buffer_size_t NSIZE1>
RBIterator<T1, NSIZE1>::RBIterator(int nitems) {
	m_ringbuffer = RingBuffer<T1,NSIZE1>::getInstance();

	m_nitems = m_ringbuffer->num_items();
	if(nitems <= m_nitems )
		m_nitems = nitems;
	m_index = 0;
	if(m_nitems == 0)
		m_isdone = true;
	else
		m_isdone = false;
}

template<class T1, ring_buffer_size_t NSIZE1>
bool RBIterator<T1, NSIZE1>::hasnext() {
	if(m_index < m_nitems)
		return true;
	else
		return false;
}

template<class T1, ring_buffer_size_t NSIZE1>
T1* RBIterator<T1, NSIZE1>::next() {
	T1 *pitem = NULL;
	if (m_index < m_nitems) {
		pitem = m_ringbuffer->peek(m_index);
		m_index++;
		if(m_index == m_nitems) {
			m_isdone = true;
		}
		return pitem;
	} else
		return NULL;
}

template<class T1, ring_buffer_size_t NSIZE1>
ring_buffer_size_t RBIterator<T1, NSIZE1>::numitems() {
	return m_nitems;
}

template<class T1, ring_buffer_size_t NSIZE1>
ring_buffer_size_t RBIterator<T1, NSIZE1>::cur_index() {
	return m_index;
}

template<class T1, ring_buffer_size_t NSIZE1>
boolean RBIterator<T1, NSIZE1>::isdone() {
	return m_isdone;
}

template<class T1, ring_buffer_size_t NSIZE1>
void RBIterator<T1, NSIZE1>::done(boolean flag) {
	m_isdone = flag;
}

//RingBuffer<uint16_t> DataFifo;
#endif
