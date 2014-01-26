/** 
 * @file:   k_rtx.h
 * @brief:  kernel definitions and data structure header file
 */
 
#ifndef K_RTX_H_
#define K_RTX_H_

/* initialization table item */
typedef struct proc_init {
	int m_pid;	        /* process id */ 
	int m_priority;         /* initial priority, not used in this example. */ 
	int m_stack_size;       /* size of stack in words */
	void (*mpf_start_pc) ();/* entry point of the process */    
} PROC_INIT;

#endif // ! K_RTX_H_
