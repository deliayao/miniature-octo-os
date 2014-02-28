/**
 * @file:   ProcessQueue.h
 * @brief:  Queue collection utility for managing processes
 */

#ifndef _PROCESS_QUEUE_
#define _PROCESS_QUEUE_

#include "Types.h"
#include "MessageQueue.h"

// Process Control Block data structure
// these act as the nodes in a process queue
typedef struct PCB {
    struct PCB* m_Next; // pointer to the next process in the queue

    U32 m_PID; // process id
    U32* m_ProcessSP; // pointer to top of process stack
    int m_Priority; // process priority
    ProcessState m_State; // current state of the process
	  struct MessageQueue m_Mailbox; // process mailbox
} PCB;

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

// attempts to find and remove the process with the given ID
// returns NULL if the process is not found
PCB* removeProcess(ProcessQueue*, int processId);

#endif /* _PROCESS_QUEUE_ */
