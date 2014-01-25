/** 
 * @file:   k_rtx.h
 * @brief:  kernel definitions and data structure header file
 */
 
#ifndef K_RTX_H_
#define K_RTX_H_

/*----- Definitions -----*/

#define RTX_ERR -1
#define RTX_OK  0

#define NULL 0
#define NUM_TEST_PROCS 2

/* Process Priority. The bigger the number is, the lower the priority. */
#define HIGH    0
#define MEDIUM  1
#define LOW     2
#define LOWEST  3

#ifdef DEBUG_0
#define USR_SZ_STACK 0x200         /* user proc stack size 512B   */
#else
#define USR_SZ_STACK 0x100         /* user proc stack size 218B  */
#endif /* DEBUG_0 */

/*----- Types -----*/
typedef unsigned char U8;
typedef unsigned int U32;

/* initialization table item */
typedef struct proc_init {
	int m_pid;	        /* process id */ 
	int m_priority;         /* initial priority, not used in this example. */ 
	int m_stack_size;       /* size of stack in words */
	void (*mpf_start_pc) ();/* entry point of the process */    
} PROC_INIT;

#endif // ! K_RTX_H_
