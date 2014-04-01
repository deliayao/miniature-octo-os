/**
 * @file:   MessageQueue.h
 * @brief:  Queue collection utility for managing messages
 */

#ifndef _MESSAGE_QUEUE_
#define _MESSAGE_QUEUE_

#include "Types.h"

/**
 * Queue structure for managing messages. Wraps a first and last pointer.
 */
typedef struct MessageQueue {
    Envelope* m_First;
    Envelope* m_Last;
} MessageQueue;

/**
 * Removes the first Envelope of the queue.
 * 
 * @param   queue The message queue to operate on.
 * @return  The first Envelope of the queue, or NULL if the queue is empty.
 */
Envelope* dequeueEnvelope(MessageQueue* queue);

/**
 * Removes the first Envelope of the queue.
 * 
 * @param   queue The message queue to operate on.
 * @return  The first Envelope of the queue, modified to only expose the Letter
 *          portion; or NULL if the queue is empty.
 */
Letter* dequeueLetter(MessageQueue* queue);

/**
 * Adds the specified Envelope to the back of the queue.
 * 
 * @param   queue The message queue to operate on.
 * @param   envelope The Envelope to add.
 * @return  The success (RTX_OK) or failure (RTX_ERR) of the operation.
 */
int enqueueEnvelope(MessageQueue* queue, Envelope* envelope);

/**
 * Converts the specified Letter to an Envelope and adds it to the back of the
 * queue.
 * 
 * @param   queue The message queue to operate on.
 * @param   letter The Letter to add.
 * @return  The success (RTX_OK) or failure (RTX_ERR) of the operation.
 */
int enqueueLetter(MessageQueue* queue, Letter* letter);

/**
 * Initializes the queue.
 * 
 * @param   queue The message queue to operate on.
 */
void initializeMessageQueue(MessageQueue* queue);

/**
 * Inserts the specified Envelope at the correct position in the queue (queue
 * is sorted by ascending expiry time).
 * 
 * @param   queue The message queue to operate on.
 * @param   envelope The Envelope to add.
 * @return  The success (RTX_OK) or failure (RTX_ERR) of the operation.
 */
int insertEnvelope(MessageQueue* queue, Envelope* envelope);

/**
 * Checks whether the queue is empty.
 * 
 * @param   queue The message queue to operate on.
 * @return  1 if the queue is empty, 0 otherwise.
 */
int isEmptyMessageQueue(MessageQueue* queue);

#endif /*_MESSAGE_QUEUE_*/
