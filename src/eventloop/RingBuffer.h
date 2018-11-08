/**
 * https://github.com/AndersKaloer/Ring-Buffer
 * Copyright (c) 2014 Anders Kalør
 *
 * Adapted to use c++ templates, Copyright (c) 2018 Andrew Goh
 * Provided as-is, No warranties of any kind, Use at your own risk
 * License: MIT
 *
 */

#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <inttypes.h>

/**
 * The size of a ring buffer.
 * Due to the design only <tt> RING_BUFFER_SIZE-1 </tt> items
 * can be contained in the buffer.
 * The buffer size must be a power of two.
 */
#if (RING_BUFFER_SIZE & (RING_BUFFER_SIZE - 1)) != 0
#error "RING_BUFFER_SIZE must be a power of two"
#endif

//#define NUMTAILS 2

/**
 * The type which is used to hold the size
 * and the indicies of the buffer.
 * Must be able to fit \c DATA_FIFO_SIZE .
 */
typedef uint16_t ring_buffer_size_t;

/**
 * Used as a modulo operator
 * as <tt> a % b = (a & (b − 1)) </tt>
 * where \c a is a positive index in the buffer and
 * \c b is the (power of two) size of the buffer
 *
 * .
 * #define RING_BUFFER_MASK (DATA_FIFO_SIZE-1)
 */


template<class T, ring_buffer_size_t NBUFSIZE>
class RingBuffer {

public:
	/**
	 * Structure which holds a ring buffer.
	 * The buffer contains a buffer array
	 * as well as metadata for the ring buffer.
	 */

	T buffer[NBUFSIZE];

	/** Index of tail. */
	ring_buffer_size_t tail_index;
	/** Index of head. */
	ring_buffer_size_t head_index;

	const ring_buffer_size_t RingBufferMask = (NBUFSIZE-1);


    static RingBuffer<T, NBUFSIZE>* getInstance();

	/**
	 * Adds a byte to a ring buffer.
	 * @param buffer The buffer in which the data should be placed.
	 * @param data The byte to place.
	 * @return -1 if buffer is full
	 */
	int push(T& data);

	/**
	 * Returns the oldest byte in a ring buffer.
	 * @param buffer The buffer from which the data should be returned.
	 * @param data A pointer to the location at which the data should be placed.
	 * @return 1 if data was returned; 0 otherwise.
	 *
	 * there are 2 tails, both must be popped to dequeue the items
	 */
	T* pop();

	/* remove n items from the queue
	 * @param n number of items to remove
	 * @return data was returned; NULL otherwise.
	 */
	int8_t popn(ring_buffer_size_t n);

	/**
	 * Peeks a ring buffer, i.e. returns an element without removing it.
	 * @param buffer The buffer from which the data should be returned.
	 * @param data A pointer to the location at which the data should be placed.
	 * @param index The index to peek.
	 * @return data was returned; NULL otherwise.
	 */
	T* peek(ring_buffer_size_t index);

	/**
	 * Returns whether a ring buffer is empty based on smallest tail
	 * @return 1 if empty; 0 otherwise.
	 */
	uint8_t is_empty();


	/**
	 * Returns whether a ring buffer is full.
	 * @param buffer The buffer for which it should be returned whether it is full.
	 * @return 1 if full; 0 otherwise.
	 */
	uint8_t is_full();

	/**
	 * Returns the number of items in a ring buffer.
	 * @param buffer The buffer for which the number of items should be returned.
	 * @return The number of items in the ring buffer.
	 */
	inline ring_buffer_size_t num_items();


private:

	// constructor
	RingBuffer();


	//xSemaphoreHandle RBmutex;

	//inline void mutex_lock(void) {
		//xSemaphoreTake(RBmutex, portMAX_DELAY );
	//}

	//inline void mutex_unlock(void) {
		//xSemaphoreGive(RBmutex);
	//}

};

template<class T1, ring_buffer_size_t NSIZE1>
class RBIterator {

public:
	RBIterator();
	RBIterator(int nitems);
	//~RBIterator();

	bool hasnext();
	T1* next();

	ring_buffer_size_t numitems();
	ring_buffer_size_t cur_index();

	boolean isdone();
	void done(boolean flag);

private:
	RingBuffer<T1,NSIZE1> *m_ringbuffer;
	ring_buffer_size_t m_nitems;
	ring_buffer_size_t m_index;
	boolean m_isdone;

};

#include "RingBuffer.cpp"


//extern RingBuffer<uint16_t,DATA_FIFO_SIZE> DataFifo;

#endif /* RINGBUFFER_H */
