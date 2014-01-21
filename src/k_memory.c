/**
 * @file:   k_memory.c
 * @brief:  kernel memory managment routines
 * @author: Yiqing Huang
 * @date:   2014/01/17
 */

#include "k_memory.h"
#define BLOCK_SIZE 128 //bytes
#define NUM_BLOCKS 20


#ifdef DEBUG_0
#include "printf.h"
#endif /* ! DEBUG_0 */

/* ----- Global Variables ----- */
U32 *gp_stack; /* The last allocated stack low address. 8 bytes aligned */
               /* The first stack starts at the RAM high address */
	       /* stack grows down. Fully decremental stack */
Node *h_begin; /* Bottom of heap. Heap grows up from here. */


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
  
	/* 8 bytes padding */
	p_end += 32;

	/* allocate memory for pcb pointers   */
	gp_pcbs = (PCB **)p_end;
	p_end += NUM_TEST_PROCS * sizeof(PCB *);
  
	for ( i = 0; i < NUM_TEST_PROCS; i++ ) {
		gp_pcbs[i] = (PCB *)p_end;
		p_end += sizeof(PCB); 
	}
#ifdef DEBUG_0
	printf("gp_pcbs[0] = 0x%x \n", gp_pcbs[0]);
	printf("gp_pcbs[1] = 0x%x \n", gp_pcbs[1]);
#endif
	/* allocate memory for stacks */
	
	gp_stack = (U32 *)RAM_END_ADDR;
	if ((U32)gp_stack & 0x04) { /* 8 bytes alignment */
		--gp_stack; 
	}
  
	/* allocate memory for heap, not implemented yet*/
	h_begin = (Node *)p_end; /**** Check this if stuff breaks ****/
  Node* currentNode = h_begin;
	Node* nextNode = currentNode;
	for (i = 0; i < NUM_BLOCKS - 1; i++) {
		nextNode = currentNode + sizeof(Node) + BLOCK_SIZE;
		currentNode->isFree = 1; // 1 is true
		currentNode->next = nextNode;
		currentNode = nextNode;
	}
	// make sure last node points to null
	currentNode->isFree = 1;
	currentNode->next = (Node *)NULL;
}

/**
 * @brief: allocate stack for a process, align to 8 bytes boundary
 * @param: size, stack size in bytes
 * @return: The top of the stack (i.e. high address)
 * POST:  gp_stack is updated.
 */

U32 *alloc_stack(U32 size_b) 
{
	U32 *sp;
	sp = gp_stack; /* gp_stack is always 8 bytes aligned */
	
	/* update gp_stack */
	gp_stack = (U32 *)((U8 *)sp - size_b);
	
	/* 8 bytes alignement adjustment to exception stack frame */
	if ((U32)gp_stack & 0x04) {
		--gp_stack; 
	}
	return sp;
}

void *k_request_memory_block(void) {
#ifdef DEBUG_0 
	printf("k_request_memory_block: entering...\n");
	Node* currentNode = h_begin;
	while (currentNode != NULL) {
		if (currentNode->isFree) {
			currentNode->isFree = 0;
			return (void *)(currentNode + sizeof(Node));
		}
		currentNode = currentNode->next;
		// weird low level stuff...
	}
#endif /* ! DEBUG_0 */
	return (void *) NULL;
}

int k_release_memory_block(void *p_mem_blk) {
#ifdef DEBUG_0 
	printf("k_release_memory_block: releasing block @ 0x%x\n", p_mem_blk);
	Node* memoryToFree = (Node *)p_mem_blk;
	if (is_valid_memory_block(p_mem_blk)) {
		
	}
#endif /* ! DEBUG_0 */
	return RTX_OK;
}

int is_valid_memory_block(Node *memory_block) {
	U32 memoryBlockAddress = (U32)memory_block;
	U32 heapBeginAddress = (U32)h_begin;
	
	// check that memory_block is between h_begin and the last node of the linked list
	if (memoryBlockAddress < heapBeginAddress + sizeof(Node) || memoryBlockAddress > heapBeginAddress + (NUM_BLOCKS - 1) * (sizeof(Node) + BLOCK_SIZE)) {
		return 0;
	}
	
	// check that memory_block modulo BLOCK_SIZE is 0
	if ((memoryBlockAddress - heapBeginAddress) % (sizeof(Node) + BLOCK_SIZE) == 0) {
		return !memory_block->isFree;
	} else {
		return 0;
	}
}
