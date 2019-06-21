/*!\file cppQueue.h
** \author SMFSW
** \date 2018/05/26
** \copyright BSD 3-Clause License (c) 2017-2018, SMFSW
** \brief Queue handling library (designed on Arduino)
** \details Queue handling library (designed on Arduino)
**			This library was designed for Arduino, yet may be compiled without change with gcc for other purposes/targets
**/
/****************************************************************/
#ifndef __CPPQUEUE_H
	#define __CPPQUEUE_H

#include <inttypes.h>
/****************************************************************/


#define QUEUE_INITIALIZED	0x5AA5		//!< Queue initialized control value


/*!\enum enumQueueType
** \brief Queue behavior enumeration (FIFO, LIFO)
**/
typedef enum enumQueueType {
	FIFO = 0,	//!< First In First Out behavior
	LIFO = 1	//!< Last In First Out behavior
} QueueType;


/*!	\class Queue
**	\brief Class containing the required methods for handling the queue
**/
class Queue
{
private:
	QueueType	impl;		//!< Queue implementation: FIFO LIFO
	bool		ovw;		//!< Overwrite previous records when queue is full allowed
	uint16_t	rec_nb;		//!< number of records in the queue
	uint16_t	rec_sz;		//!< Size of a record
	uint32_t	queue_sz;	//!< Size of the full queue
	uint8_t *	queue;		//!< Queue start pointer (when allocated)
	
	uint16_t	in;			//!< number of records pushed into the queue
	uint16_t	out;		//!< number of records pulled from the queue (only for FIFO)
	uint16_t	cnt;		//!< number of records not retrieved from the queue
	uint16_t	init;		//!< set to QUEUE_INITIALIZED after successful init of the queue, 0 otherwise
public:
	/*!	\brief Queue constructor
	**	\param [in] size_rec - size of a record in the queue
	**	\param [in] nb_recs - number of records in the queue
	**	\param [in] type - Queue implementation type: FIFO, LIFO
	**	\param [in] overwrite - Overwrite previous records when queue is full
	**	\return nothing
	**/
	Queue(const uint16_t size_rec, const uint16_t nb_recs=20, const QueueType type=FIFO, const bool overwrite=false);

	/*!	\brief Queue destructor: release dynamically allocated queue
	**/
	~Queue();

	/*!	\brief Flush queue, restarting from empty queue
	**/
	void flush(void);

	/*!	\brief Clean queue, restarting from empty queue
	**	\deprecated clean was already used in Queue lib, alias is made to keep compatibility with earlier versions
	**/
	inline void clean(void)__attribute__((always_inline)) {
		flush(); }

	/*!	\brief get initialization state of the queue
	**	\return Queue initialization status
	**	\retval true if queue is allocated
	**	\retval false is queue is not allocated
	**/
	inline bool isInitialized(void) __attribute__((always_inline)) {
		return (init == QUEUE_INITIALIZED) ? true : false; }

	/*!	\brief get emptiness state of the queue
	**	\return Queue emptiness status
	**	\retval true if queue is empty
	**	\retval false is not empty
	**/
	inline bool isEmpty(void) __attribute__((always_inline)) {
		return (!cnt) ? true : false; }

	/*!	\brief get fullness state of the queue
	**	\return Queue fullness status
	**	\retval true if queue is full
	**	\retval false is not full
	**/
	inline bool isFull(void) __attribute__((always_inline)) {
		return (cnt == rec_nb) ? true : false; }

	/*!	\brief get size of queue
	**	\remark Size in bytes (like sizeof)
	**	\return Size of queue in bytes
	**/
	inline uint32_t sizeOf(void) __attribute__((always_inline)) {
		return queue_sz; }

	/*!	\brief get number of records in the queue
	**	\return Number of records stored in the queue
	**/
	inline uint16_t getCount(void) __attribute__((always_inline)) {
		return cnt; }

	/*!	\brief get number of records in the queue (same as getCount)
	**	\deprecated nbRecs was already used in Queue lib, alias is made to keep compatibility with earlier versions
	**	\return Number of records stored in the queue
	**/
	inline uint16_t nbRecs(void) __attribute__((always_inline)) {
		return getCount(); }

	/*!	\brief get number of records left in the queue
	**	\return Number of records left in the queue
	**/
	inline uint16_t getRemainingCount(void) __attribute__((always_inline)) {
		return rec_nb - cnt; }

	/*!	\brief Push record to queue
	**	\param [in] record - pointer to record to be pushed into queue
	**	\return Push status
	**	\retval true if successfully pushed into queue
	**	\retval false if queue is full
	**/
	bool push(const void * record);

	/*!	\brief Pop record from queue
	**	\warning If using push, pop, peek and/or drop in both interrupts and main application,
	**				you shall disable interrupts in main application when using these functions
	**	\param [in,out] record - pointer to record to be popped from queue
	**	\return Pop status
	**	\retval true if successfully popped from queue
	**	\retval false if queue is empty
	**/
	bool pop(void * record);

	/*!	\brief Pull record from queue (same as pop)
	**	\warning If using push, pop, peek and/or drop in both interrupts and main application,
	**				you shall disable interrupts in main application when using these functions
	**	\deprecated pull was already used in Queue lib, alias is made to keep compatibility with earlier versions
	**	\param [in,out] record - pointer to record to be pulled from queue
	**	\return Pull status
	**	\retval true if successfully pulled from queue
	**	\retval false if queue is empty
	**/
	inline bool pull(void * record)__attribute__((always_inline)) {
		return pop(record); }

	/*!	\brief Peek record from queue
	**	\warning If using push, pop, peek and/or drop in both interrupts and main application,
	**				you shall disable interrupts in main application when using these functions
	**	\param [in,out] record - pointer to record to be peeked from queue
	**	\return Peek status
	**	\retval true if successfully peeked from queue
	**	\retval false if queue is empty
	**/
	bool peek(void * record);

	/*!	\brief Drop current record from queue
	**	\warning If using push, pop, peek and/or drop in both interrupts and main application,
	**				you shall disable interrupts in main application when using these functions
	**	\return drop status
	**	\retval true if successfully dropped from queue
	**	\retval false if queue is empty
	**/
	bool drop(void);

    bool peek_last(void * record);
};

#endif /* __CPPQUEUE_H */
