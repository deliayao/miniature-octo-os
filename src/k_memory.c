/**
 * @file:   k_memory.c
 * @brief:  kernel memory managment routines
 */

#include "k_memory.h"

#include "k_process.h"
#include "Utilities/MemoryQueue.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* ! DEBUG_0 */

/**
 * The operating system's heap.
 */
MemoryQueue g_Heap;

/**
 * The last allocated stack low address. 8 bytes aligned. The first stack
 * starts at the RAM high address and the stack grows down.
 */
static U32* s_stack_pointer;

/**
 * Memory layout:

0x10008000+---------------------------+ High Address
          |    Proc 1 STACK           |
          |---------------------------|
          |    Proc 2 STACK           |
          |---------------------------|<--- s_stack_pointer
          |                           |
          |        HEAP               |
          |                           |
          |---------------------------|
          |        PCB 2              |
          |---------------------------|
          |        PCB 1              |
          |---------------------------|<--- gp_pcbs
          |        Padding            |
          |---------------------------|
          |Image$$RW_IRAM1$$ZI$$Limit |
          |...........................|
          |       RTX  Image          |
          |                           |
0x10000000+---------------------------+ Low Address

*/

U32* alloc_stack(U32 size_b) {
    U32* sp;
    sp = s_stack_pointer; // s_stack_pointer is always 8 bytes aligned

    // update s_stack_pointer
    s_stack_pointer = (U32*)((U8*)sp - size_b);

    // 8 bytes alignment adjustment to exception stack frame
    if ((U32)s_stack_pointer & 0x04) {
        --s_stack_pointer;
    }
    return sp;
}

int k_release_memory_block(void *p_mem_blk) {
    Node* memoryToFree = (Node*)((U32)p_mem_blk - sizeof(Node) - sizeof(Envelope)); // if the node is valid, it will occur at this address

#ifdef DEBUG_0
   printf("k_release_memory_block: releasing block @ 0x%x\n", p_mem_blk);
#endif /* ! DEBUG_0 */

    if (isValidNode(&g_Heap, memoryToFree)) {
        enqueueNode(&g_Heap, memoryToFree); // if valid, add back to heap
        return handleMemoryRelease(1); // allow preemption
    }
    return RTX_ERR;
}

void* k_request_memory_block(void) {
#ifdef DEBUG_0
    printf("k_request_memory_block: entering...\n");
#endif /* ! DEBUG_0 */

    while (isEmptyMemoryQueue(&g_Heap)) {
        g_CurrentProcess->m_State = BLOCKED_MEM;
        k_release_processor();
    }

    // return the next node offset by the size of the Node
    return (void*)((U32)dequeueNode(&g_Heap) + sizeof(Node) + sizeof(Envelope));
}

void memory_init(void) {
    U8* p_end = (U8*)&Image$$RW_IRAM1$$ZI$$Limit;
    int i;

    // 4 bytes padding
    p_end += 4;

    for (i = 0; i < NUM_PROCS; i++) {
        g_ProcessTable[i] = (PCB *)p_end;
        p_end += sizeof(PCB);
    }

    // prepare for alloc_stack() to allocate memory for stacks
    s_stack_pointer = (U32*)RAM_END_ADDR;
    if ((U32)s_stack_pointer & 0x04) { // 8 bytes alignment
        --s_stack_pointer;
    }

    // initialize heap
    initializeMemoryQueue(&g_Heap, (Node*)p_end);
}

void* nonBlockingRequestMemory(void) {
    Node* memoryBlock = dequeueNode(&g_Heap);
    if (memoryBlock != NULL) {
        // we retrieved a memory block
        // add the size of the header before returning it
        return (void*)((U32)memoryBlock + sizeof(Node) + sizeof(Envelope));
    } else {
        return (void*)NULL;
    }
}

int nonPreemptiveReleaseMemory(void* memory) {
    Node* memoryToFree = (Node*)((U32)memory - sizeof(Node) - sizeof(Envelope)); // if the node is valid, it will occur at this address
    
    if (isValidNode(&g_Heap, memoryToFree)) {
        enqueueNode(&g_Heap, memoryToFree); // if valid, add back to heap
        return handleMemoryRelease(0); // do not allow preemption
    }
    return RTX_ERR;
}
