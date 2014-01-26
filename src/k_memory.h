/**
 * @file:   k_memory.h
 * @brief:  kernel memory management header file
 */
 
#ifndef K_MEM_H_
#define K_MEM_H_

#include "Utilities/Definitions.h"
#include "Utilities/Types.h"
#include "Utilities/k_rtx.h"

/* ----- Variables ----- */
/* This symbol is defined in the scatter file (see RVCT Linker User Guide) */  
extern unsigned int Image$$RW_IRAM1$$ZI$$Limit; 

extern PROC_INIT g_proc_table[NUM_TEST_PROCS];

extern PCB* processTable[NUM_TEST_PROCS]; // kernel process table
extern PCB* currentProcess; // points to the current RUNNING process

/* ----- Functions ------ */
void memory_init(void);
U32 *alloc_stack(U32 size_b);
void *k_request_memory_block(void);
int k_release_memory_block(void *);

#endif /* ! K_MEM_H_ */
