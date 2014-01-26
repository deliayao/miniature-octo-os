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
#define NUM_BLOCKS 10 // TODO: find out what the actual available number of blocks is
#define RAM_END_ADDR 0x10008000

// process management
#define INITIAL_xPSR 0x01000000 // user process initial xPSR value

#define NULL 0
#define NUM_TEST_PROCS 6

// process priority
// the higher the number, the lower the priority
#define HIGH    0
#define MEDIUM  1
#define LOW     2
#define LOWEST  3

#ifdef DEBUG_0
#define USR_SZ_STACK 0x200         /* user proc stack size 512B   */
#else
#define USR_SZ_STACK 0x100         /* user proc stack size 218B  */
#endif /* DEBUG_0 */

// process states
typedef enum {
    NEW = 0,
    READY,
    RUNNING,
    BLOCKED_MEM,
    BLOCKED_IO
} ProcessState;

#endif /* _DEFINITIONS_ */
