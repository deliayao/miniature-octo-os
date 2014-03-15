/**
 * @file:   k_process.c
 * @brief:  process management C file
 */

#include <LPC17xx.h>
#include <system_LPC17xx.h>

#include "Polling/uart_polling.h"
#include "Utilities/PriorityQueue.h"

#include "k_process.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* DEBUG_0 */

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
extern PROC_INIT nullProcess;

/**
 * @brief: initialize all processes in the system
 */
void process_init() {
	int i;
	U32 *sp;

	// initialize processes
	initializeNullProcess();
	set_test_procs();

	g_proc_table[NULL_PROCESS].m_pid = nullProcess.m_pid;
	g_proc_table[NULL_PROCESS].m_priority = nullProcess.m_priority;
	g_proc_table[NULL_PROCESS].m_stack_size = nullProcess.m_stack_size;
	g_proc_table[NULL_PROCESS].mpf_start_pc = nullProcess.mpf_start_pc;

	for ( i = 1; i < NUM_PROCS; i++ ) {
		g_proc_table[i].m_pid = g_test_procs[i - 1].m_pid;
		g_proc_table[i].m_priority = g_test_procs[i - 1].m_priority;
		g_proc_table[i].m_stack_size = g_test_procs[i - 1].m_stack_size;
		g_proc_table[i].mpf_start_pc = g_test_procs[i - 1].mpf_start_pc;
	}

	/* initialize exception stack frame (i.e. initial context) for each process */
	for ( i = 0; i < NUM_PROCS; i++ ) {
		int j;
		(processTable[i])->m_PID = (g_proc_table[i]).m_pid;
		(processTable[i])->m_Priority = (g_proc_table[i]).m_priority;
		(processTable[i])->m_State = NEW;

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
	for (i = 0; i < NUM_PROCS; i++) {
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
		// add process to appropriate queue and save context
		if (currentProcess->m_State == BLOCKED_MEM) { // blocked on memory
			enqueueAtPriority(&blockedOnMemoryQueue, currentProcess);
		} else { // ready
			enqueueAtPriority(&readyQueue, currentProcess);
			currentProcess->m_State = READY;
		}
		currentProcess->m_ProcessSP = (U32 *)__get_MSP();
	}

	oldProcess = currentProcess;
	currentProcess = scheduler();

	// save context of old process if we're actually switching
	if (currentProcess != oldProcess) {
		if (currentProcess->m_State == READY || currentProcess->m_State == NEW) {
			__set_MSP((U32) currentProcess->m_ProcessSP); // switch to the new processes's stack

			if (currentProcess->m_State == NEW) {
				__rte(); // pop exception stack frame from the stack for new processes
			}
			currentProcess->m_State = RUNNING;
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

	if (process_id <= 0 || process_id >= NUM_PROCS) { // cannot change priority of null process
		return RTX_ERR;
	}
	if (priority < HIGH || priority > LOWEST) { // cannot change to NULL_PRIORITY
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

    // if the specific process is currently queued
	for (i = 0; i < QUEUED_STATES; i++) {
		if (updateProcessPriority(masterPQs[i], process_id, oldPriority, priority)) {
			k_release_processor(); // allow the processor to preempt the current process if it wants to
			return RTX_OK;
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

int handleMemoryRelease(void) {
	if (!isEmptyPriorityQueue(&blockedOnMemoryQueue)) {
		// clear blocked on memory queue, move processes to ready
		PCB* process = dequeueHighest(&blockedOnMemoryQueue);
		if (process != NULL) {
			process->m_State = READY;
			enqueueAtPriority(&readyQueue, process);
		}
		return k_release_processor();
	}
	return RTX_OK;
}
