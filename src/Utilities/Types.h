/**
 * @file:   Types.h
 * @brief:  Kernel type definitions for data types and data structures
 */
 
#ifndef _TYPES_
#define _TYPES_

#include "Definitions.h"

// data types
typedef unsigned char U8;
typedef unsigned int U32;

// Process Control Block data structure
// these act as the nodes in a process queue
typedef struct PCB {
    U32 m_PID; // process id
    U32* m_ProcessSP; // pointer to top of process stack
    int m_Priority; // process priority
    ProcessState m_State; // current state of the process

    struct PCB* m_Next; // pointer to the next process in the queue
} PCB;

// memory node data structure for memory queue
typedef struct Node {
    struct Node* m_Next; // pointer to the next memory block in the queue
} Node;

#endif /* _TYPES_ */
