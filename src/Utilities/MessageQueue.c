/**
 * @file:   MessageQueue.c
 * @brief:  Message queue implementation
 */

#include "MessageQueue.h"

Envelope* dequeueEnvelope(MessageQueue* queue) {
    Envelope* front = queue->m_First; // this will be NULL if the queue is empty

    if (queue->m_First != NULL) {
#ifndef DEBUG_PERFORMANCE
        queue->m_First = queue->m_First->m_Next;
#endif /* DEBUG_PERFORMANCE */

        // this is true if the queue only had one element
        if (queue->m_First == NULL) {
            queue->m_Last = NULL;
        }
        
        front->m_Next = NULL;
    }

    return front;
}

Letter* dequeueLetter(MessageQueue* queue) {
    Envelope* envelope = dequeueEnvelope(queue);
    return (envelope == NULL) ? (Letter*)envelope : (Letter*)((U32)envelope + sizeof(Envelope));
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
    return RTX_OK; 
}

int enqueueLetter(MessageQueue* queue, Letter* letter) {
    return enqueueEnvelope(queue, (Envelope*)((U32)letter - sizeof(Envelope)));
}

void initializeMessageQueue(MessageQueue* queue) {
    queue->m_First = NULL;
    queue->m_Last = NULL;
}

int insertEnvelope(MessageQueue* queue, Envelope* envelope) {
    Envelope* currentEnvelope;
    Envelope* nextEnvelope;
    
    if (isEmptyMessageQueue(queue) || envelope->m_Expiry >= queue->m_Last->m_Expiry) {
        return enqueueEnvelope(queue, envelope);
    } else if (envelope->m_Expiry < queue->m_First->m_Expiry) {
        // insert envelope at the front
        envelope->m_Next = queue->m_First;
        queue->m_First = envelope;
        return RTX_OK;
    }
    
	currentEnvelope = queue->m_First;
    nextEnvelope = currentEnvelope->m_Next;
	
    while (currentEnvelope != queue->m_Last && envelope->m_Expiry >= nextEnvelope->m_Expiry) {
        currentEnvelope = nextEnvelope;
        nextEnvelope = currentEnvelope->m_Next;
    }

    currentEnvelope->m_Next = envelope;
    envelope->m_Next = nextEnvelope;
    return RTX_OK;
}

int isEmptyMessageQueue(MessageQueue* queue) {
    return queue->m_First == NULL;
}
