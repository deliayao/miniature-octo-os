/**
 * @file:   k_memory.h
 * @brief:  kernel memory management header file
 */
 
#ifndef K_MEM_H_
#define K_MEM_H_

#include "Utilities/Definitions.h"
#include "Utilities/k_rtx.h"
#include "Utilities/ProcessQueue.h"
#include "Utilities/Types.h"

extern PCB* g_CurrentProcess; // points to the current RUNNING process

/**
 * End address of RTX Image. This symbol is defined in the scatter file (see
 * RVCT Linker User Guide).
 */
extern unsigned int Image$$RW_IRAM1$$ZI$$Limit; 
extern PROC_INIT g_proc_table[NUM_PROCS];
extern PCB* g_ProcessTable[NUM_PROCS]; // kernel process table

/**
 * Allocates stack space for a process, aligned to 8 bytes boundary.
 * 
 * @param   size_b Size of stack (in bytes) to allocate.
 * @return  The new top of stack.
 */
U32* alloc_stack(U32 size_b);

/**
 * Returns the given memory block to the OS. This primitive is preemptive.
 * 
 * @param   p_mem_blk The memory block to be returned.
 * @return  The success (RTX_OK) or failure (RTX_ERR) of the operation.
 */
int k_release_memory_block(void* p_mem_blk);

/**
 * Gets a new block of memory, if available. This primitive is blocking.
 * 
 * @return  A pointer to a memory block.
 */
void* k_request_memory_block(void);

/**
 * Initializes the heap and space for PCB pointers.
 */
void memory_init(void);

/**
 * Gets a new block of memory, if available. This primitive is non-blocking.
 * 
 * @return  A pointer to a memory block, or NULL if no memory is available.
 */
void* nonBlockingRequestMemory(void);

/**
 * Returns the given memory block to the OS. This primitive is non-preemptive.
 * 
 * @param   memory The memory block to be returned.
 * @return  The success (RTX_OK) or failure (RTX_ERR) of the operation.
 */
int nonPreemptiveReleaseMemory(void* memory);

#endif /* ! K_MEM_H_ */
