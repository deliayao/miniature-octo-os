/**
 * @file:   PriorityQueue.h
 * @brief:  Priority queue collection utility for managing processes
 */

#ifndef _PRIORITY_QUEUE_
#define _PRIORITY_QUEUE_

#include "Definitions.h"
#include "Types.h"
#include "ProcessQueue.h"

typedef struct PriorityQueue {
    ProcessQueue m_Queues[NUM_PRIORITIES]; // underlying array of queues
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

// returns 1 if the priority queue is empty
int isEmptyPriorityQueue(PriorityQueue*);

// attempts to find process at given old priority and update it to new priority
// returns the success/failure of the operation
int updateProcessPriority(PriorityQueue*, int processId, int oldPriority, int newPriority);

#endif /* _PRIORITY_QUEUE_ */
