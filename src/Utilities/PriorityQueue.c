/**
 * @file:   PriorityQueue.c
 * @brief:  Priority queue collection implementation
 */

#include "PriorityQueue.h"

PCB* dequeueAtPriority(PriorityQueue* priorityQueue, int priority) {
    PCB* front = NULL;

    if (priority >= PRIVILEGED && priority <= NULL_PRIORITY) {
        front = dequeue(getQueueAtPriority(priorityQueue, priority));
    }
    return front;
}

PCB* dequeueHighest(PriorityQueue* priorityQueue) {
    PCB* front = NULL;
    int i;

    for (i = 0; i < NUM_PRIORITIES; ++i) {
        front = dequeue(getQueueAtPriority(priorityQueue, i));
        if (front != NULL) {
            break;
        }
    }
    return front;
}

int enqueueAtPriority(PriorityQueue* priorityQueue, PCB* process) {
	if (process->m_Priority < PRIVILEGED || process->m_Priority > NULL_PRIORITY) {
		return -1;
	}
	return enqueue(getQueueAtPriority(priorityQueue, process->m_Priority), process);
}

ProcessQueue* getQueueAtPriority(PriorityQueue* priorityQueue, int priority) {
    return &(priorityQueue->m_Queues[priority]);
}

void initializePriorityQueue(PriorityQueue* priorityQueue) {
	int i;
	for (i = 0; i < NUM_PRIORITIES; ++i) {
		initializeQueue(getQueueAtPriority(priorityQueue, i));
	}
}

int isEmptyPriorityQueue(PriorityQueue* priorityQueue) {
	int i;
	for (i = 0; i < NUM_PRIORITIES; ++i) {
		if (!isEmptyProcessQueue(&priorityQueue->m_Queues[i])) {
			return 0;
		}
	}
	return 1;
}

int updateProcessPriority(PriorityQueue* queue, int processId, int oldPriority, int newPriority) {
	PCB* process = removeProcess(getQueueAtPriority(queue, oldPriority), processId);
	
	if (process == NULL) {
		return 0; // error
	} else {
		process->m_Priority = newPriority;
		return enqueue(getQueueAtPriority(queue, newPriority), process);
	}
}

void serializePriorityQueue(PriorityQueue* priorityQueue, char message[],  int startIndex) {
    int j = startIndex; // j is the current write index of message
    int i;
    for (i = 0; i < NUM_PRIORITIES; i++) {
        message[j] = 'P';
        j++;
        message[j] = i + '0';
        j++;
        message[j] = ':';
        j++;
        message[j] = ' ';
        j++;
		j = serializeProcessQueue(&priorityQueue->m_Queues[i], message, j);
        message[j] = '\n';
        j++;
	}
    message[j] = '\0';
}
