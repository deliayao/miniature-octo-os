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
#define NUM_BLOCKS 40
#define RAM_END_ADDR 0x10008000

// process management
#define INITIAL_xPSR 0x01000000 // user process initial xPSR value
#define NUM_PRIORITIES 6

#define NUM_PROCS 16
#define NUM_TEST_PROCS 6
#define NUM_STRESS_PROCS 3
#define NUM_IPROCS 2
#define NUM_SYSTEM_PROCS 3

#define NULL 0

// process IDs
#define NULL_PROCESS            0
#define PROCESS_1               1
#define PROCESS_2               2
#define PROCESS_3               3
#define PROCESS_4               4
#define PROCESS_5               5
#define PROCESS_6               6
#define PROCESS_A               7
#define PROCESS_B               8
#define PROCESS_C               9
#define PROCESS_SET_PRIORITY    10
#define CLOCK_PROCESS           11
#define KCD_PROCESS             12
#define CRT_PROCESS             13
#define TIMER_IPROCESS          14
#define UART_IPROCESS           15

// process priority
// the higher the number, the lower the priority
#define PRIVILEGED       0
#define HIGH             1
#define MEDIUM           2
#define LOW              3
#define LOWEST           4
#define NULL_PRIORITY    5

// IPC
#define DEFAULT 0
#define KCD_REG 1
#define REPORT 2
#define WAKEUP 3

#define MAX_LETTER_LENGTH 35
#define COMMAND_TABLE_SIZE 10
#define MAX_COMMAND_LENGTH 3

#ifdef DEBUG_0
#define USR_SZ_STACK 0x200 // user proc stack size 512B
#else
#define USR_SZ_STACK 0x100 // user proc stack size 218B
#endif /* DEBUG_0 */

// process states
#define QUEUED_STATES 2

/*
 * Various states that a process can be in.
 */
typedef enum {
    NEW = 0, // in same queue as ready
    READY, // queued state
    RUNNING,
    BLOCKED_MEM, // queued state
    BLOCKED_IO,
    BLOCKED_RECEIVE
} ProcessState;

// UART flags
#define IER_RBR     0x01
#define IER_THRE    0x02
#define IER_RLS     0x04

#define IIR_PEND    0x01
#define IIR_RLS     0x03
#define IIR_RDA     0x02
#define IIR_CTI     0x06
#define IIR_THRE    0x01

#define LSR_RDR     0x01
#define LSR_OE      0x02
#define LSR_PE      0x04
#define LSR_FE      0x08
#define LSR_BI      0x10
#define LSR_THRE    0x20
#define LSR_TEMT    0x40
#define LSR_RXFE    0x80

#define BUFSIZE     0x40

// convenient macro for bit operation
#define BIT(X) (1 << X)

#define UART_8N1  0x83

#endif /* _DEFINITIONS_ */
