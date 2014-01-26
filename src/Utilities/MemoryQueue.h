/**
 * @file:   MemoryQueue.h
 * @brief:  Heap utility for managing memory
 */

#ifndef _MEMORY_QUEUE_
#define _MEMORY_QUEUE_

#include "Definitions.h"
#include "Types.h"

typedef struct MemoryQueue {
    Node* m_First;
    Node* m_Last;
} MemoryQueue;

// removes and returns the first Node of the queue
// returns NULL if the queue is empty
Node* dequeueNode(MemoryQueue*);

// adds the specified Node to the back of the queue
// returns the success/failure of the operation
int enqueueNode(MemoryQueue*, Node*);

// initializes the queue
void initializeMemoryQueue(MemoryQueue*, Node*);

// returns 1 if the queue is empty
int isEmptyMemoryQueue(MemoryQueue*);

// returns 1 if the node occurs at a valid address
int isValidNode(MemoryQueue*, Node*);

#endif /*_MEMORY_QUEUE_*/
