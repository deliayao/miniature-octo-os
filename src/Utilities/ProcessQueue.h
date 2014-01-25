/**
 * @file:   ProcessQueue.h
 * @brief:  Queue collection utility for managing processes
 */

#ifndef _PROCESS_QUEUE_
#define _PROCESS_QUEUE_

// process states
typedef enum {
    NEW = 0,
    READY,
    RUNNING,
    BLOCKED_MEM,
    BLOCKED_IO
} ProcessState;

// Process Control Block data structure
// these act as the nodes in a process queue
typedef struct PCB {
    U32 m_PID; // process id
    U32* m_ProcessSP; // pointer to top of process stack
    void* m_ProcessPC; // pointer to the next instruction to execute in the program code
                       // when the process is activated
                       // if the process is new, this points to the start of the process

    int m_Priority; // process priority
    ProcessState m_State; // current state of the process

    struct PCB* m_Next; // pointer to the next process in the queue
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

#endif _PROCESS_QUEUE_