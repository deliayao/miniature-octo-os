/**
 * @file:   PriorityQueue.c
 * @brief:  Priority queue collection implementation
 */

#include "PriorityQueue.h"

PCB* dequeueAtPriority(PriorityQueue* priorityQueue, int priority) {
    PCB* front = NULL;

    if (priority >= HIGH && priority <= LOWEST) {
        front = dequeue(getQueueAtPriority(priorityQueue, priority));
    }
    return front;
}

PCB* dequeueHighest(PriorityQueue* priorityQueue) {
    PCB* front = NULL;
    int i;

    for (i = HIGH; i <= LOWEST; ++i) {
        front = dequeue(getQueueAtPriority(priorityQueue, i));
        if (front != NULL) {
            break;
        }
    }
    return front;
}

int enqueueAtPriority(PriorityQueue* priorityQueue, PCB* process) {
	if (process->m_Priority < HIGH || process->m_Priority > LOWEST) {
		return -1;
	}
	return enqueue(getQueueAtPriority(priorityQueue, process->m_Priority), process);
}

ProcessQueue* getQueueAtPriority(PriorityQueue* priorityQueue, int priority) {
    return &(priorityQueue->m_Queues[priority]);
}

void initializePriorityQueue(PriorityQueue* priorityQueue) {
	int i;
	for (i = HIGH; i <= LOWEST; ++i) {
		initializeQueue(getQueueAtPriority(priorityQueue, i));
	}
}

int isEmptyPriorityQueue(PriorityQueue* priorityQueue) {
	int i;
	for (i = 0; i < LOWEST + 1; ++i) {
		if (!isEmptyProcessQueue(&priorityQueue->m_Queues[i])) {
			return 0;
		}
	}
	return 1;
}
