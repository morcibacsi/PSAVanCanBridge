/*!\file cppQueue.cpp
** \author SMFSW
** \date 2018/05/26
** \copyright BSD 3-Clause License (c) 2017-2018, SMFSW
** \brief Queue handling library (designed on Arduino)
** \details Queue handling library (designed on Arduino)
**			This library was designed for Arduino, yet may be compiled without change with gcc for other purposes/targets
**/
/****************************************************************/
extern "C" {
	#include <string.h>
	#include <stdlib.h>
}

#include "cppQueue.h"
/****************************************************************/


#define INC_IDX(ctr, end, start)	if (ctr < (end-1))	{ ctr++; }		\
									else				{ ctr = start; }	//!< Increments buffer index \b ctr rolling back to \b start when limit \b end is reached

#define DEC_IDX(ctr, end, start)	if (ctr > (start))	{ ctr--; }		\
									else				{ ctr = end-1; }	//!< Decrements buffer index \b ctr rolling back to \b end when limit \b start is reached


Queue::Queue(const uint16_t size_rec, const uint16_t nb_recs, const QueueType type, const bool overwrite)
{
	uint32_t size = nb_recs * size_rec;

	rec_nb = nb_recs;
	rec_sz = size_rec;
	impl = type;
	ovw = overwrite;

	init = 0;

	//if (queue)	{ free(queue); }	// Free existing data (if any)
	queue = (uint8_t *) malloc(size);

	if (queue == NULL)	{ queue_sz = 0; return; }	// Return here if Queue not allocated
	else				{ queue_sz = size; }

	init = QUEUE_INITIALIZED;
	flush();
}

Queue::~Queue()
{
	if (init == QUEUE_INITIALIZED)	free(queue);
}


void Queue::flush(void)
{
	in = 0;
	out = 0;
	cnt = 0;
}


bool Queue::push(const void * record)
{
	if ((!ovw) && isFull())	{ return false; }
	
	uint8_t * pStart = queue + (rec_sz * in);
	memcpy(pStart, record, rec_sz);
	
	INC_IDX(in, rec_nb, 0);
	
	if (!isFull())	{ cnt++; }	// Increase records count
	else if (ovw)				// Queue is full and overwrite is allowed
	{
		if (impl == FIFO)			{ INC_IDX(out, rec_nb, 0); }	// as oldest record is overwritten, increment out
		//else if (impl == LIFO)	{}								// Nothing to do in this case
	}
	
	return true;
}

bool Queue::pop(void * record)
{
	uint8_t * pStart;
	
	if (isEmpty())	{ return false; }	// No more records
	
	if (impl == FIFO)
	{
		pStart = queue + (rec_sz * out);
		INC_IDX(out, rec_nb, 0);
	}
	else if (impl == LIFO)
	{
		DEC_IDX(in, rec_nb, 0);
		pStart = queue + (rec_sz * in);
	}
	else	{ return false; }
	
	memcpy(record, pStart, rec_sz);
	cnt--;	// Decrease records count
	return true;
}


bool Queue::peek(void * record)
{
	uint8_t *	pStart;
	
	if (isEmpty())	{ return false; }	// No more records
	
	if (impl == FIFO)
	{
		pStart = queue + (rec_sz * out);
		// No change on out var as it's just a peek
	}
	else if (impl == LIFO)
	{
		uint16_t rec = in;	// Temporary var for peek (no change on in with DEC_IDX)
		DEC_IDX(rec, rec_nb, 0);
		pStart = queue + (rec_sz * rec);
	}
	else	{ return false; }
	
	memcpy(record, pStart, rec_sz);
	return true;
}


bool Queue::drop(void)
{
	if (isEmpty())			{ return false; }	// No more records
	
	if (impl == FIFO)		{ INC_IDX(out, rec_nb, 0); }
	else if (impl == LIFO)	{ DEC_IDX(in, rec_nb, 0); }
	else					{ return false; }
	
	cnt--;	// Decrease records count
	return true;
}

bool Queue::peek_last(void * record)
{
    uint8_t *	pStart;

    if (isEmpty()) { return false; }	// No more records

    uint16_t rec = in;	// Temporary var for peek (no change on in with DEC_IDX)
    DEC_IDX(rec, rec_nb, 0);
    pStart = queue + (rec_sz * rec);

    memcpy(record, pStart, rec_sz);
    return true;
}
