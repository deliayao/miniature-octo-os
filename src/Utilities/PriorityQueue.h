/**
 * @file:   PriorityQueue.h
 * @brief:  Priority queue collection utility for managing processes
 */

#ifndef _PRIORITY_QUEUE_
#define _PRIORITY_QUEUE_

#include "ProcessQueue.h"
#include "rtx.h" // for priority definitions

typedef struct PriorityQueue {
    ProcessQueue m_Queues[LOWEST + 1]; // underlying array of queues
                                       // one queue for each priority level
} PriorityQueue;

// removes and returns the first PCB of the specified queue
// returns NULL if the queue is empty
PCB* dequeueAtPriority(PriorityQueue*, int);

// removes and returns the first PCB of the highest priority queue that isn't empty
// returns NULL if all of the queues are empty
PCB* dequeueHighest(PriorityQueue*);

// adds the PCB to the back of the queue specified by the PCB's priority
// returns the success/failure of the operation
int enqueueAtPriority(PriorityQueue*, PCB*);

// returns a pointer to the queue at the specified priority
ProcessQueue* getQueueAtPriority(PriorityQueue*, int);

// initializes the priority queue
void initializePriorityQueue(PriorityQueue*);

#endif _PRIORITY_QUEUE_