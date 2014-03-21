/**
 * @file:   k_process.c
 * @brief:  process management C file
 */

#include <LPC17xx.h>
#include <system_LPC17xx.h>

#include "Polling/uart_polling.h"
#include "Utilities/MessageQueue.h"
#include "Utilities/MemoryQueue.h"

#include "k_process.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* ! DEBUG_0 */

PCB* processTable[NUM_PROCS]; // kernel process table
PCB* currentProcess; // points to the current RUNNING process

/* Master Queue */
PriorityQueue* masterPQs[QUEUED_STATES]; // array of all queued states
                                         // this array is iterated when the priority of a process is changed
                                         // and we must find the process to remove and re-enqueue it

/* Priority queues */
PriorityQueue readyQueue;
PriorityQueue blockedOnMemoryQueue;

/* process initialization table */
PROC_INIT g_proc_table[NUM_PROCS];
extern PROC_INIT g_test_procs[NUM_TEST_PROCS];
extern PROC_INIT stressProcesses[NUM_STRESS_PROCS];
extern PROC_INIT nullProcess;
extern PROC_INIT clockProcess;
extern PROC_INIT KCDProcess;
extern PROC_INIT CRTProcess;
extern PROC_INIT timerProcess;
extern PROC_INIT UARTProcess;
extern PROC_INIT setPriorityProcess;

extern MemoryQueue heap;
extern volatile uint32_t g_timer_count;

/**
 * @brief: initialize all processes in the system
 */
void process_init() {
	int i;
	U32 *sp;

	// initialize processes
    initializeSetPriorityProcess();
    initializeClockProcess();
	initializeSystemProcesses();
    initializeTimerProcess();
    initializeUARTProcess();
    
	set_test_procs();
    setStressTestProcesses();

    // system processes
	g_proc_table[NULL_PROCESS].m_pid = nullProcess.m_pid;
	g_proc_table[NULL_PROCESS].m_priority = nullProcess.m_priority;
	g_proc_table[NULL_PROCESS].m_stack_size = nullProcess.m_stack_size;
	g_proc_table[NULL_PROCESS].mpf_start_pc = nullProcess.mpf_start_pc;
    
    g_proc_table[PROCESS_SET_PRIORITY].m_pid = setPriorityProcess.m_pid;
	g_proc_table[PROCESS_SET_PRIORITY].m_priority = setPriorityProcess.m_priority;
	g_proc_table[PROCESS_SET_PRIORITY].m_stack_size = setPriorityProcess.m_stack_size;
	g_proc_table[PROCESS_SET_PRIORITY].mpf_start_pc = setPriorityProcess.mpf_start_pc;
    
    g_proc_table[CLOCK_PROCESS].m_pid = clockProcess.m_pid;
	g_proc_table[CLOCK_PROCESS].m_priority = clockProcess.m_priority;
	g_proc_table[CLOCK_PROCESS].m_stack_size = clockProcess.m_stack_size;
	g_proc_table[CLOCK_PROCESS].mpf_start_pc = clockProcess.mpf_start_pc;
    
    g_proc_table[KCD_PROCESS].m_pid = KCDProcess.m_pid;
	g_proc_table[KCD_PROCESS].m_priority = KCDProcess.m_priority;
	g_proc_table[KCD_PROCESS].m_stack_size = KCDProcess.m_stack_size;
	g_proc_table[KCD_PROCESS].mpf_start_pc = KCDProcess.mpf_start_pc;
    
    g_proc_table[CRT_PROCESS].m_pid = CRTProcess.m_pid;
	g_proc_table[CRT_PROCESS].m_priority = CRTProcess.m_priority;
	g_proc_table[CRT_PROCESS].m_stack_size = CRTProcess.m_stack_size;
	g_proc_table[CRT_PROCESS].mpf_start_pc = CRTProcess.mpf_start_pc;
    
    g_proc_table[TIMER_IPROCESS].m_pid = timerProcess.m_pid;
	g_proc_table[TIMER_IPROCESS].m_priority = timerProcess.m_priority;
	g_proc_table[TIMER_IPROCESS].m_stack_size = timerProcess.m_stack_size;
	g_proc_table[TIMER_IPROCESS].mpf_start_pc = timerProcess.mpf_start_pc;
    
    g_proc_table[UART_IPROCESS].m_pid = UARTProcess.m_pid;
	g_proc_table[UART_IPROCESS].m_priority = UARTProcess.m_priority;
	g_proc_table[UART_IPROCESS].m_stack_size = UARTProcess.m_stack_size;
	g_proc_table[UART_IPROCESS].mpf_start_pc = UARTProcess.mpf_start_pc;

    // test processes
	for (i = 1; i <= NUM_TEST_PROCS; i++) {
		g_proc_table[i].m_pid = g_test_procs[i - 1].m_pid;
		g_proc_table[i].m_priority = g_test_procs[i - 1].m_priority;
		g_proc_table[i].m_stack_size = g_test_procs[i - 1].m_stack_size;
		g_proc_table[i].mpf_start_pc = g_test_procs[i - 1].mpf_start_pc;
	}
    
    // stress test processes
	for (i = 1; i <= NUM_STRESS_PROCS; i++) {
		g_proc_table[i + 6].m_pid = stressProcesses[i - 1].m_pid;
		g_proc_table[i + 6].m_priority = stressProcesses[i - 1].m_priority;
		g_proc_table[i + 6].m_stack_size = stressProcesses[i - 1].m_stack_size;
		g_proc_table[i + 6].mpf_start_pc = stressProcesses[i - 1].mpf_start_pc;
	}

	/* initialize exception stack frame (i.e. initial context) for each process */
	for ( i = 0; i < NUM_PROCS; i++ ) {
		int j;
		(processTable[i])->m_PID = (g_proc_table[i]).m_pid;
		(processTable[i])->m_Priority = (g_proc_table[i]).m_priority;
		(processTable[i])->m_State = NEW;
		initializeMessageQueue(&((processTable[i])->m_Mailbox));

		sp = alloc_stack((g_proc_table[i]).m_stack_size);
		*(--sp)  = INITIAL_xPSR;      // user process initial xPSR
		*(--sp)  = (U32)((g_proc_table[i]).mpf_start_pc); // PC contains the entry point of the process
		for ( j = 0; j < 6; j++ ) { // R0-R3, R12 are cleared with 0
			*(--sp) = 0x0;
		}
		(processTable[i])->m_ProcessSP = sp;
	}

	// initialize priority queues
	initializePriorityQueue(&readyQueue);
	initializePriorityQueue(&blockedOnMemoryQueue);

	masterPQs[0] = &readyQueue;
	masterPQs[1] = &blockedOnMemoryQueue;

	// all processes are currently new and ready
	for (i = 0; i < NUM_PROCS - NUM_IPROCS; i++) {
		enqueueAtPriority(&readyQueue, processTable[i]);
	}

	// no running process
	currentProcess = NULL;
}

/*@brief: scheduler, pick the pid of the next to run process
 *@return: PCB pointer of the next to run process
 */
PCB *scheduler(void) {
	return dequeueHighest(&readyQueue);
}

/*@brief: switch out old pcb, run the new pcb
 *@return: RTX_OK upon success
 *         RTX_ERR upon failure
 */
int process_switch() {
	PCB* oldProcess;

	if (currentProcess != NULL) {
        // if current process is an i-process, don't add it to any priority queue
		// else, add process to appropriate queue and save context
        if (currentProcess->m_State == BLOCKED_RECEIVE) {
        } else if (currentProcess->m_State == BLOCKED_MEM) { // blocked on memory
			enqueueAtPriority(&blockedOnMemoryQueue, currentProcess);
		} else { // ready
			enqueueAtPriority(&readyQueue, currentProcess);
			currentProcess->m_State = READY;
		}
	}

	oldProcess = currentProcess;
	currentProcess = scheduler();

	// save context of old process if we're actually switching
	if (currentProcess != oldProcess) {
        if (oldProcess != NULL) {
            oldProcess->m_ProcessSP = (U32 *)__get_MSP();
        }
        
		if (currentProcess->m_State == READY || currentProcess->m_State == NEW) {
            ProcessState state = currentProcess->m_State;
            currentProcess->m_State = RUNNING;
			__set_MSP((U32) currentProcess->m_ProcessSP); // switch to the new processes's stack

			if (state == NEW) {
				__rte(); // pop exception stack frame from the stack for new processes
			}
		} else { // if the scheduler chose a process that isn't READY or NEW, something broke
			currentProcess = oldProcess;
			return RTX_ERR;
		}
	} else {
		currentProcess->m_State = RUNNING;
	}

	return RTX_OK;
}

/**
 * @brief release_processor().
 * @return RTX_ERR on error and zero on success
 */
int k_release_processor(void) {
	return process_switch(); 
}

int k_set_process_priority(int process_id, int priority) {
	int oldPriority;
	int i;

	if (process_id <= NULL_PROCESS || process_id > (NUM_TEST_PROCS + NUM_STRESS_PROCS)) { // cannot change priority of system processes
		return RTX_ERR;
	}
	if (priority < HIGH || priority > LOWEST) { // cannot change to PRIVILEGED or NULL_PRIORITY
		return RTX_ERR;
	}

	oldPriority = processTable[process_id]->m_Priority;

	if (oldPriority == priority) {
		// expected behaviour:
		// if there is no change in priority, the current process is not preempted
		// and the specified process retains its position in the priority queue
		return RTX_OK;
	}

	// if the current process is the specified process,
	// change its priority and preempt it
	if (currentProcess->m_PID == process_id) {
		currentProcess->m_Priority = priority;
		return k_release_processor();
	}
    
    // handle changing priority of a blocked on receive process
    // (blocked on receive processes are not in any priority queue)
    if (processTable[process_id]->m_State == BLOCKED_RECEIVE) {
        processTable[process_id]->m_Priority = priority;
        return RTX_OK; // should this case preempt?
    } else { // if the specified process is currently queued
        for (i = 0; i < QUEUED_STATES; i++) {
            if (updateProcessPriority(masterPQs[i], process_id, oldPriority, priority)) {
                k_release_processor(); // allow the processor to preempt the current process if it wants to
                return RTX_OK;
            }
        }
    }
    
	return RTX_ERR;
}

int k_get_process_priority(int process_id) {
	if (process_id <= 0 || process_id >= NUM_PROCS) { // cannot get priority of null process
		return RTX_ERR;
	}
	return processTable[process_id]->m_Priority;
}

int k_delayed_send(int process_id, void *message_envelope, int delay) {
    return deliverMessage(currentProcess->m_PID, process_id, TIMER_IPROCESS, message_envelope, delay);
}

int k_send_message(int process_id, void *message_envelope) {
    PCB* destination = processTable[process_id];
    int result = deliverMessage(currentProcess->m_PID, process_id, process_id, message_envelope, 0);
    
    // if the destination process is blocked on receive, unblock it
    if (destination->m_State == BLOCKED_RECEIVE) {
        destination->m_State = READY;
        enqueueAtPriority(&readyQueue, destination);
        
        // preempt current process if destination process has higher priority and is blocked on receive
        if (destination->m_Priority < currentProcess->m_Priority) {
            return k_release_processor();
        }
    }
    
    return result;
}

void *k_receive_message(int *sender_id) {
    Envelope * envelope;
    
    while (isEmptyMessageQueue(&(currentProcess->m_Mailbox))) {
		currentProcess->m_State = BLOCKED_RECEIVE;
		k_release_processor();
	}
    
    envelope = dequeueEnvelope(&(currentProcess->m_Mailbox));
	if (sender_id != NULL) {
		*sender_id = envelope->m_SenderPID; // return ID of sender
	}
    
	return (void*)((U32)envelope + sizeof(Envelope)); // return the envelope offset by the size of Envelope
}

int deliverMessage(int sourceProcess, int envelopeDestinationProcess, int destinationProcess, void* message, int delay) {
    Envelope* envelope;
    PCB* destination = processTable[destinationProcess];
	U32 node = (U32)message - sizeof(Envelope) - sizeof(Node); // for error checking
	
	if (!isValidNode(&heap, (Node*)node)) { // make sure it's a valid memory block
		return RTX_ERR;
	}
	
	node += sizeof(Node);
	envelope = (Envelope*)node;
	envelope->m_DestinationPID = envelopeDestinationProcess;
	envelope->m_SenderPID = sourceProcess;
	envelope->m_Expiry = g_timer_count + delay;
    
    return enqueueEnvelope(&(destination->m_Mailbox), envelope);
}

int handleMemoryRelease(int preempt) {
	if (!isEmptyPriorityQueue(&blockedOnMemoryQueue)) {
		// clear blocked on memory queue, move processes to ready
		PCB* process = dequeueHighest(&blockedOnMemoryQueue);
		if (process != NULL) {
			process->m_State = READY;
			enqueueAtPriority(&readyQueue, process);
		}
        if (preempt) {
            return k_release_processor();
        }
	}
	return RTX_OK;
}

void* nonBlockingReceiveMessage(int receiverID, int *senderIDOutput) {
    Envelope* envelope = dequeueEnvelope(&(processTable[receiverID]->m_Mailbox));
	if (senderIDOutput != NULL) {
		*senderIDOutput = (envelope == NULL) ? -1 : envelope->m_SenderPID;
    }
    return (envelope == NULL) ? (void*)envelope : (void*)((U32)envelope + sizeof(Envelope)); // return the envelope offset by the size of Envelope
}

int nonPreemptiveSendMessage(int sourceID, int destinationID, void* message) {
    PCB* destination = processTable[destinationID];
    int result = deliverMessage(sourceID, destinationID, destinationID, message, 0);
    
    // if the destination process is blocked on receive, unblock it
    if (destination->m_State == BLOCKED_RECEIVE) {
        destination->m_State = READY;
        enqueueAtPriority(&readyQueue, destination);
    }
    
    return result;
}

void serializeQueue(char debugInfo[], int start, int queueNumber){
    int j = start;
    if (queueNumber == 2) { // blocked on receive, special case
        int i;
        for ( i = 0; i < NUM_PROCS; i++ ) {
            if (processTable[i]->m_State == BLOCKED_RECEIVE) {
                debugInfo[j] = '(';
                j++;
                if (processTable[i]->m_PID / 10 > 0) {// assuming less than 99 processes
                    debugInfo[j] = (processTable[i]->m_PID/10) + '0';
                    j++;
                }
                debugInfo[j] = (processTable[i]->m_PID % 10) + '0';
                j++;
                debugInfo[j] = ',';
                j++;
                debugInfo[j] = processTable[i]->m_Priority + '0';
                j++;
                debugInfo[j] = ')';
                j++;
				debugInfo[j] = '\r';
                j++;
                debugInfo[j] = '\n';
                j++;
            }
        }
				debugInfo[j] = '\0';
    } else { // any priority queued state
        if (queueNumber >= 0 && queueNumber < QUEUED_STATES) {
            serializePriorityQueue(masterPQs[queueNumber], debugInfo, j);
        }
    }
}
