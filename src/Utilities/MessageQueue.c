/**
 * @file:   MessageQueue.c
 * @brief:  Message queue implementation
 */

#include "MessageQueue.h"

Envelope* dequeueEnvelope(MessageQueue* queue) {
    Envelope* front = queue->m_First; // this will be NULL if the queue is empty

    if (queue->m_First != NULL) {
        queue->m_First = queue->m_First->m_Next;

        // this is true if the queue only had one element
        if (queue->m_First == NULL) {
            queue->m_Last = NULL;
        }
    }

    return front;
}

int enqueueEnvelope(MessageQueue* queue, Envelope* envelope) {
    // new envelope will be the last element in the queue
    envelope->m_Next = NULL;

    if (queue->m_First == NULL) {
        queue->m_First = envelope;
    } else {
        queue->m_Last->m_Next = envelope;
    }

    queue->m_Last = envelope;

    // this function doesn't actually do any checks right now,
    // so the operation always succeeds
    return 1; 
}

void initializeMessageQueue(MessageQueue* queue) {
    queue->m_First = NULL;
    queue->m_Last = NULL;
}

int isEmptyMessageQueue(MessageQueue* queue) {
	return queue->m_First == NULL;
}
