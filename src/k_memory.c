/**
 * @file:   k_memory.c
 * @brief:  kernel memory managment routines
 */

#include "Utilities/MemoryQueue.h"
#include "k_process.h"
#include "k_memory.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* ! DEBUG_0 */

/* ----- Global Variables ----- */
U32 *gp_stack; /* The last allocated stack low address. 8 bytes aligned */
               /* The first stack starts at the RAM high address */
	           /* stack grows down. Fully decremental stack */
						 
MemoryQueue heap; // heap representation

/**
 * @brief: Initialize RAM as follows:

0x10008000+---------------------------+ High Address
          |    Proc 1 STACK           |
          |---------------------------|
          |    Proc 2 STACK           |
          |---------------------------|<--- gp_stack
          |                           |
          |        HEAP               |
          |                           |
          |---------------------------|
          |        PCB 2              |
          |---------------------------|
          |        PCB 1              |
          |---------------------------|
          |        PCB pointers       |
          |---------------------------|<--- gp_pcbs
          |        Padding            |
          |---------------------------|  
          |Image$$RW_IRAM1$$ZI$$Limit |
          |...........................|          
          |       RTX  Image          |
          |                           |
0x10000000+---------------------------+ Low Address

*/

void memory_init(void) {
	U8 *p_end = (U8 *)&Image$$RW_IRAM1$$ZI$$Limit;
	int i;
  
	// 4 bytes padding
	p_end += 4;
  
	for ( i = 0; i < NUM_PROCS; i++ ) {
		processTable[i] = (PCB *)p_end;
		p_end += sizeof(PCB);
	}

  // prepare for alloc_stack() to allocate memory for stacks
	gp_stack = (U32 *)RAM_END_ADDR;
	if ((U32)gp_stack & 0x04) { /* 8 bytes alignment */
		--gp_stack; 
	}
	
	// initialize heap
  initializeMemoryQueue(&heap, (Node *)p_end);
}

/**
 * @brief: allocate stack for a process, align to 8 bytes boundary
 * @param: size, stack size in bytes
 * @return: The top of the stack (i.e. high address)
 * POST:  gp_stack is updated.
 */

U32 *alloc_stack(U32 size_b) {
	U32 *sp;
	sp = gp_stack; /* gp_stack is always 8 bytes aligned */
	
	/* update gp_stack */
	gp_stack = (U32 *)((U8 *)sp - size_b);
	
	/* 8 bytes alignment adjustment to exception stack frame */
	if ((U32)gp_stack & 0x04) {
		--gp_stack; 
	}
	return sp;
}

void *k_request_memory_block(void) {
#ifdef DEBUG_0 	
	printf("k_request_memory_block: entering...\n");
#endif /* ! DEBUG_0 */
	
	while (isEmptyMemoryQueue(&heap)) {
		currentProcess->m_State = BLOCKED_MEM;
		k_release_processor();
	}
	
	return (void*)((U32)dequeueNode(&heap) + sizeof(Node));
}

int k_release_memory_block(void *p_mem_blk) {
    Node* memoryToFree = (Node *)((U32)p_mem_blk - sizeof(Node)); // if the node is valid, it will occur at this address

#ifdef DEBUG_0
   printf("k_release_memory_block: releasing block @ 0x%x\n", p_mem_blk);
#endif /* ! DEBUG_0 */

	if (isValidNode(&heap, memoryToFree)) {
			enqueueNode(&heap, memoryToFree);
			return handleMemoryRelease();
	} 
	return RTX_ERR;
}
