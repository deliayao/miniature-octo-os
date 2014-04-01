/**
 * @file:   PriorityQueue.h
 * @brief:  Priority queue collection utility for managing processes
 */

#ifndef _PRIORITY_QUEUE_
#define _PRIORITY_QUEUE_

#include "Definitions.h"
#include "ProcessQueue.h"
#include "Types.h"

/**
 * Priority queue structure for managing processes. Contains an underlying
 * array of process queues; one queue for each priority level.
 */
typedef struct PriorityQueue {
    ProcessQueue m_Queues[NUM_PRIORITIES];
} PriorityQueue;

/**
 * Removes the first PCB of the queue at the specified priority.
 * 
 * @param   priorityQueue The priority queue to operate on.
 * @param   priority The priority at which to dequeue.
 * @return  The first PCB of the specified queue, or NULL if the specified
 *          queue is empty.
 */
PCB* dequeueAtPriority(PriorityQueue* priorityQueue, int priority);

/**
 * Removes the first PCB of the highest level queue that is not empty.
 * 
 * @param   priorityQueue The priority queue to operate on.
 * @return  The first PCB of the highest level queue, or NULL if all queues are
 *          empty.
 */
PCB* dequeueHighest(PriorityQueue* priorityQueue);

/**
 * Adds the specified PCB to the back of the queue specified by the PCB's
 * priority.
 * 
 * @param   priorityQueue The priority queue to operate on.
 * @param   process The PCB to add.
 * @return  The success (1) or failure (0) of the operation.
 */
int enqueueAtPriority(PriorityQueue* priorityQueue, PCB* process);

/**
 * Gets the queue at the specified priority level.
 * 
 * @param   priorityQueue The priority queue to operate on.
 * @param   priority The priority of interest.
 * @return  A pointer to the queue at the specified priority.
 */
ProcessQueue* getQueueAtPriority(PriorityQueue* priorityQueue, int priority);

/**
 * Initializes the priority queue.
 * 
 * @param   priorityQueue The priority queue to operate on.
 */
void initializePriorityQueue(PriorityQueue* priorityQueue);

/**
 * Checks whether the priority queue is empty.
 * 
 * @param   priorityQueue The priority queue to operate on.
 * @return  1 if the priority queue is empty, 0 otherwise.
 */
int isEmptyPriorityQueue(PriorityQueue* priorityQueue);

/**
 * Serializes the priority queue. For debugging.
 * 
 * @param   priorityQueue The priority queue to operate on.
 * @param   message Memory to write to.
 * @param   startIndex First index to write to.
 */
void serializePriorityQueue(PriorityQueue* priorityQueue, char message[],  int startIndex);

/**
 * Attemps to find the process at the given old priority and update it to the
 * new priority.
 * 
 * @param   priorityQueue The priority queue to operate on.
 * @param   processId The process to update.
 * @param   oldPriority The old priority of the process.
 * @param   newPriority The priority to change to.
 * @return  1 if the operation was successful, 0 if the process was not found
 *          at the given priority.
 */
int updateProcessPriority(PriorityQueue* priorityQueue, int processId, int oldPriority, int newPriority);

#endif /* _PRIORITY_QUEUE_ */
