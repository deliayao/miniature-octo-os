/**
 * @file:   ProcessQueue.h
 * @brief:  Queue collection utility for managing processes
 */

#ifndef _PROCESS_QUEUE_
#define _PROCESS_QUEUE_

#include "Types.h"
#include "MessageQueue.h"

/**
 * Process control block data structure. These act as the nodes in a process
 * queue.
 */
typedef struct PCB {
    struct PCB* m_Next; // pointer to the next process in the queue

    U32 m_PID; // process id
    U32* m_ProcessSP; // pointer to top of process stack
    int m_Priority; // process priority
    ProcessState m_State; // current state of the process
    struct MessageQueue m_Mailbox; // process mailbox
} PCB;

/**
 * Queue structure for managing processes. Wraps a first and last pointer.
 */
typedef struct ProcessQueue {
    PCB* m_First;
    PCB* m_Last;
} ProcessQueue;

/**
 * Removes the first PCB of the queue.
 * 
 * @param   queue The process queue to operate on.
 * @return  The first PCB of the queue, or NULL if the queue is empty.
 */
PCB* dequeue(ProcessQueue* queue);

/**
 * Adds the specified PCB to the back of the queue.
 * 
 * @param   queue The process queue to operate on.
 * @param   process The PCB to add.
 * @return  The success (1) or failure (0) of the operation.
 */
int enqueue(ProcessQueue* queue, PCB* process);

/**
 * Initializes the queue.
 * 
 * @param   queue The process queue to operate on.
 */
void initializeQueue(ProcessQueue* queue);

/**
 * Checks whether the queue is empty.
 * 
 * @param   queue The message queue to operate on.
 * @return  1 if the queue is empty, 0 otherwise.
 */
int isEmptyProcessQueue(ProcessQueue* queue);

/**
 * Checks whether the specified process ID is the ID of an i-process.
 * 
 * @param   processID The ID of the process of interest.
 * @return  1 if the process is an i-process, 0 otherwise.
 */
int isIProcess(int processID);

/**
 * Attempts to find and remove the process with the given ID.
 * 
 * @param   queue The process queue to operate on.
 * @param   processId The ID of the process to remove.
 * @return  The removed process, or NULL if the process was not found.
 */
PCB* removeProcess(ProcessQueue* queue, int processId);

/**
 * Serializes the process queue. For debugging.
 * 
 * @param   queue The process queue to operate on.
 * @param   message Memory to write to.
 * @param   startIndex First index to write to.
 */
int serializeProcessQueue(ProcessQueue* queue, char message[], int startIndex);

#endif /* _PROCESS_QUEUE_ */
