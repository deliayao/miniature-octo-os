/**
 * @file:   ProcessQueue.c
 * @brief:  Queue collection implementation
 */

#include "ProcessQueue.h"

PCB* dequeue(ProcessQueue* queue) {
    PCB* front = queue->m_First; // this will be NULL if the queue is empty

    if (queue->m_First != NULL) {
        queue->m_First = queue->m_First->m_Next;

        // this is true if the queue only had one element
        if (queue->m_First == NULL) {
            queue->m_Last = NULL;
        }
    }

    return front;
}

int enqueue(ProcessQueue* queue, PCB* process) {
    // PCB will be the last element in the queue
    process->m_Next = NULL;

    if (queue->m_First == NULL) {
        queue->m_First = process;
    } else {
        queue->m_Last->m_Next = process;
    }

    queue->m_Last = process;

    // this function doesn't actually do any checks right now,
    // so the operation always succeeds
    return 1; 
}

void initializeQueue(ProcessQueue* queue) {
    queue->m_First = NULL;
    queue->m_Last = NULL;
}

int isEmptyProcessQueue(ProcessQueue* queue) {
	return queue->m_First == NULL;
}

PCB* removeProcess(ProcessQueue* queue, int processId) {
	PCB* currentProcess = queue->m_First;
	PCB* nextProcess = queue->m_First;
	
	if (currentProcess->m_PID == processId) {
		queue->m_First = currentProcess->m_Next;
		if (queue->m_First == NULL) {
			queue->m_Last = NULL;
		}
		return currentProcess;
	}
	
	while (currentProcess != NULL) {
		nextProcess = currentProcess->m_Next;
		if (nextProcess != NULL && nextProcess->m_PID == processId) {
			currentProcess->m_Next = nextProcess->m_Next;
			if (currentProcess->m_Next == NULL) {
				queue->m_Last = NULL;
			}
			return nextProcess;
		}
		currentProcess = nextProcess;
	}
	return (PCB*)NULL;
}
