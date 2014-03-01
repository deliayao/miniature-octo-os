/**
 * @file:   Definitions.h
 * @brief:  Kernel macro definitions and enums
 */

#ifndef _DEFINITIONS_
#define _DEFINITIONS_

// error states
#define RTX_ERR -1
#define RTX_OK  0

// memory management
#define BLOCK_SIZE 128 // bytes
#define NUM_BLOCKS 40 // TODO: find out what the actual available number of blocks is
#define RAM_END_ADDR 0x10008000

// process management
#define INITIAL_xPSR 0x01000000 // user process initial xPSR value
#define NUM_PRIORITIES 5

#define NUM_PROCS 8
#define NUM_TEST_PROCS 6
#define NUM_IPROCS 1

#define NULL 0

// process IDs
#define NULL_PROCESS 					0
#define PROCESS_1   					1
#define PROCESS_2   					2
#define PROCESS_3   					3
#define PROCESS_4   					4
#define PROCESS_5   					5
#define PROCESS_6   					6
#define PROCESS_A    					7
#define PROCESS_B    					8
#define PROCESS_C    					9
#define PROCESS_SET_PRIORITY 	10
#define CLOCK_PROCESS        	11
#define KCD_PROCESS          	12
#define CRT_PROCESS          	13
#define TIMER_IPROCESS  			7
#define UART_IPROCESS   			15

// process priority
// the higher the number, the lower the priority
#define HIGH    0
#define MEDIUM  1
#define LOW     2
#define LOWEST  3
#define NULL_PRIORITY 4

// IPC
#define DEFAULT 0
#define KCD_REG 1

#ifdef DEBUG_0
#define USR_SZ_STACK 0x200         /* user proc stack size 512B   */
#else
#define USR_SZ_STACK 0x100         /* user proc stack size 218B  */
#endif /* DEBUG_0 */

// process states
#define QUEUED_STATES 2
typedef enum {
    NEW = 0, // in same queue as ready
    READY, // queued state
    RUNNING,
    BLOCKED_MEM, // queued state
    BLOCKED_IO 
} ProcessState;

#endif /* _DEFINITIONS_ */
