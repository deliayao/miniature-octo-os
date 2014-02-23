/**
 * @file:   MessageQueue.h
 * @brief:  Queue collection utility for managing messages
 */

#ifndef _MESSAGE_QUEUE_
#define _MESSAGE_QUEUE_

#include "Definitions.h"
#include "Types.h"

typedef struct MessageQueue {
    Envelope* m_First;
    Envelope* m_Last;
} MessageQueue;

// removes and returns the first Envelope of the queue
// returns NULL if the queue is empty
Envelope* dequeueEnvelope(MessageQueue*);

// adds the specified Envelope to the back of the queue
// returns the success/failure of the operation
int enqueueEnvelope(MessageQueue*, Envelope*);

// initializes the queue
void initializeMessageQueue(MessageQueue*);

// returns 1 if the queue is empty
int isEmptyMessageQueue(MessageQueue*);

#endif /*_MESSAGE_QUEUE_*/
