/**
 * @file:   MemoryQueue.h
 * @brief:  Heap utility for managing memory
 */

#ifndef _MEMORY_QUEUE_
#define _MEMORY_QUEUE_

#define BLOCK_SIZE 128 // bytes
#define NUM_BLOCKS 10 // TODO: find out what the actual available number of blocks is

// Memory node data structure for memory queue
typedef struct Node {
    struct Node m_Next; // pointer to the next memory block in the queue
} Node;

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

#endif /*_MEMORY_QUEUE_*/
