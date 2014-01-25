/**
 * @file:   k_process.c  
 * @brief:  process management C file
 * NOTE: The example code shows one way of implementing context switching.
 *       The code only has minimal sanity check. There is no stack overflow check.
 *       The implementation assumes only two simple user processes and NO HARDWARE INTERRUPTS. 
 *       The purpose is to show how context switch could be done under stated assumptions. 
 *       These assumptions are not true in the required RTX Project!!!
 *       If you decide to use this piece of code, you need to understand the assumptions and
 *       the limitations. 
 */

#include <LPC17xx.h>
#include <system_LPC17xx.h>
#include "uart_polling.h"
#include "k_process.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* DEBUG_0 */

/* ----- Global Variables ----- */
PCB **gp_pcbs;                  /* array of pcbs */
PCB *gp_current_process = NULL; /* always point to the current RUNNING process */

/* Priority queues */
PriorityQueue ready_queue;
PriorityQueue blocked_on_memory_queue;

/* process initialization table */
PROC_INIT g_proc_table[NUM_TEST_PROCS];
extern PROC_INIT g_test_procs[NUM_TEST_PROCS];

/**
 * @brief: initialize all processes in the system
 * NOTE: We assume there are only two user processes in the system in this example.
 */
void process_init() {
	int i;
	U32 *sp;
  
    /* fill out the initialization table */
	set_test_procs();
	for ( i = 0; i < NUM_TEST_PROCS; i++ ) {
		g_proc_table[i].m_pid = g_test_procs[i].m_pid;
		g_proc_table[i].m_priority = g_test_procs[i].m_priority;
		g_proc_table[i].m_stack_size = g_test_procs[i].m_stack_size;
		g_proc_table[i].mpf_start_pc = g_test_procs[i].mpf_start_pc;
	}
  
	/* initialize exception stack frame (i.e. initial context) for each process */
	for ( i = 0; i < NUM_TEST_PROCS; i++ ) {
		int j;
		(gp_pcbs[i])->m_PID = (g_proc_table[i]).m_pid;
		(gp_pcbs[i])->m_Priority = (g_proc_table[i]).m_priority;
		(gp_pcbs[i])->m_State = NEW; 
		
		sp = alloc_stack((g_proc_table[i]).m_stack_size);
		*(--sp)  = INITIAL_xPSR;      // user process initial xPSR  
		*(--sp)  = (U32)((g_proc_table[i]).mpf_start_pc); // PC contains the entry point of the process
		for ( j = 0; j < 6; j++ ) { // R0-R3, R12 are cleared with 0
			*(--sp) = 0x0;
		}
		(gp_pcbs[i])->m_ProcessSP = sp;
	}
	
	// initialize priority queues
	initializePriorityQueue(&ready_queue);
	initializePriorityQueue(&blocked_on_memory_queue);
	
	// all processes are currently new and ready
	for (i = 0; i < NUM_TEST_PROCS; i++) {
		enqueueAtPriority(&ready_queue, gp_pcbs[i]);
	}
}

/*@brief: scheduler, pick the pid of the next to run process
 *@return: PCB pointer of the next to run process
 *         NULL if error happens
 *POST: if gp_current_process was NULL, then it gets set to pcbs[0].
 *      No other effect on other global variables.
 */

PCB *scheduler(void) {
	return dequeueHighest(&ready_queue);
}

/*@brief: switch out old pcb (p_pcb_old), run the new pcb (gp_current_process)
 *@param: p_pcb_old, the old pcb that was in RUN
 *@return: RTX_OK upon success
 *         RTX_ERR upon failure
 *PRE:  p_pcb_old and gp_current_process are pointing to valid PCBs.
 *POST: if gp_current_process was NULL, then it gets set to pcbs[0].
 *      No other effect on other global variables.
 */
int process_switch(PCB *p_pcb_old) {
	ProcessState state;
	state = gp_current_process->m_State;

	if (state == NEW) {
		if (gp_current_process != p_pcb_old && p_pcb_old->m_State != NEW) {
			p_pcb_old->m_State = READY;
			p_pcb_old->m_ProcessSP = (U32 *) __get_MSP();
		}
		gp_current_process->m_State = RUNNING;
		__set_MSP((U32) gp_current_process->m_ProcessSP);
		__rte();  // pop exception stack frame from the stack for a new processes
	} 
	
	/* The following will only execute if the if block above is FALSE */

	if (gp_current_process != p_pcb_old) {
		if (state == READY) { 		
			p_pcb_old->m_State = READY; 
			p_pcb_old->m_ProcessSP = (U32 *) __get_MSP(); // save the old process's sp
			gp_current_process->m_State = RUNNING;
			__set_MSP((U32) gp_current_process->m_ProcessSP); //switch to the new proc's stack    
		} else {
			gp_current_process = p_pcb_old; // revert back to the old proc on error
			return RTX_ERR;
		} 
	}
	return RTX_OK;
}
/**
 * @brief release_processor(). 
 * @return RTX_ERR on error and zero on success
 * POST: gp_current_process gets updated to next to run process
 */
int k_release_processor(void) {
	PCB *oldProcess = NULL;
	
	oldProcess = gp_current_process; // save current process
	gp_current_process = scheduler(); // get next scheduled process
	
	if (gp_current_process == NULL) {
		gp_current_process = oldProcess; // revert back to the old process
		return RTX_ERR;
	}
    if (oldProcess == NULL) {
		oldProcess = gp_current_process;
	}
	process_switch(oldProcess);
	return RTX_OK;
}
