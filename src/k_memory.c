/**
 * @file:   k_memory.c
 * @brief:  kernel memory managment routines
 */

#include "k_memory.h"
#define BLOCK_SIZE 128 // bytes
#define NUM_BLOCKS 10 // TODO: find out what the actual available number of blocks is


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

	Node* currentNode;
	Node* nextNode;
  
	// 4 bytes padding
	p_end += 4;

	// allocate memory for pcb pointers
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

    // prepare for alloc_stack() to allocate memory for stacks
	gp_stack = (U32 *)RAM_END_ADDR;
	if ((U32)gp_stack & 0x04) { /* 8 bytes alignment */
		--gp_stack; 
	}
  
	// allocate memory for heap
	h_begin = (Node *)p_end; /**** Check this if stuff breaks ****/
    currentNode = h_begin;
	
    // partition heap memory into a linked list of equal sized nodes
	for (i = 0; i < NUM_BLOCKS - 1; i++) {
		nextNode = currentNode + sizeof(Node) + BLOCK_SIZE;
		currentNode->isFree = 1; // 1 is true
		currentNode->next = nextNode;
		currentNode = nextNode;
	}

	// make sure last node points to null
	currentNode->isFree = 1;
	currentNode->next = (Node *)NULL;
	
#ifdef DEBUG_0
	printf("heap initialized \n");
#endif
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
	Node* currentNode = h_begin;

#ifdef DEBUG_0 	
	printf("k_request_memory_block: entering...\n");
#endif /* ! DEBUG_0 */

    // find the first free node
	while (currentNode != NULL) {
		if (currentNode->isFree) {
			currentNode->isFree = 0;
			return (void *)(currentNode + sizeof(Node));
		}
		currentNode = currentNode->next;
		// TODO: weird low level stuff...
	}
	return (void *) NULL;
}

int k_release_memory_block(void *p_mem_blk) {
    Node* memoryToFree = (Node *)((U32)p_mem_blk - sizeof(Node)); // if the node is valid, it will occur at this address

#ifdef DEBUG_0
   printf("k_release_memory_block: releasing block @ 0x%x\n", p_mem_blk);
#endif /* ! DEBUG_0 */

	if (is_valid_memory_block(p_mem_blk)) {
		if (!memoryToFree->isFree) {
			memoryToFree->isFree = 1;
			return RTX_OK;
		}
	} 
	return RTX_ERR;
}

// does not check if memory block is free
int is_valid_memory_block(void *memory_block) {
    // get the address of the Node associated with the memory block
    // (this accounts for the header size of the Node)
    // convert pointers to integers for address comparisons
	U32 nodeAddress = (U32)memory_block - (U32)sizeof(Node);
	U32 heapBeginAddress = (U32)h_begin;
	
	// check that nodeAddress is between h_begin and the last node of the linked list
	if (nodeAddress < heapBeginAddress || nodeAddress > heapBeginAddress + (NUM_BLOCKS - 1) * (sizeof(Node) + BLOCK_SIZE)) {
		return 0;
	}
	
	// check that nodeAddress occurs at some integer multiple of sizeof(Node) + BLOCK_SIZE
	return ((nodeAddress - heapBeginAddress) % (sizeof(Node) + BLOCK_SIZE) == 0);
}
