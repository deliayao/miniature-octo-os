/**
 * @file:   ProcessQueue.h
 * @brief:  Queue collection utility for managing processes
 */

#ifndef _PROCESS_QUEUE_
#define _PROCESS_QUEUE_

#include "Types.h"

typedef struct ProcessQueue {
    PCB* m_First;
    PCB* m_Last;
} ProcessQueue;

// removes and returns the first PCB of the queue
// returns NULL if the queue is empty
PCB* dequeue(ProcessQueue*);

// adds the specified PCB to the back of the queue
// returns the success/failure of the operation
int enqueue(ProcessQueue*, PCB*);

// initializes the queue
// (this also effectively clears the queue)
void initializeQueue(ProcessQueue*);

// returns 1 if the queue is empty
int isEmptyProcessQueue(ProcessQueue*);

#endif /* _PROCESS_QUEUE_ */
