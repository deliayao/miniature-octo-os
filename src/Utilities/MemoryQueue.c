/**
 * @file:   MemoryQueue.c
 * @brief:  Heap implementation
 */

#include "MemoryQueue.h"

/*
 * The number of memory blocks currently free.
 */
int g_AvailableCount;

/*
 * The number of memory blocks currently in use.
 */
int g_UsedCount;

/*
 * The first node of the heap at the time of memory initialization (used for
 * node validation).
 */
static Node* s_InitialFront = NULL;

Node* dequeueNode(MemoryQueue* queue) {
    Node* front = queue->m_First; // this will be NULL if queue is empty

    if (queue->m_First != NULL) {
// during performance testing mode, don't modify m_First
// always return the first memory block in the queue
#ifndef DEBUG_PERFORMANCE
        queue->m_First = queue->m_First->m_Next;
#endif /* DEBUG_PERFORMANCE */       

        // this is true if the queue only had one element
        if (queue->m_First == NULL) {
            queue->m_Last = NULL;
        }
        
        front->m_Next = NULL;
        
        g_UsedCount++;
        g_AvailableCount--;
    }

    return front;
}

int enqueueNode(MemoryQueue* queue, Node* node) {
    // node will be the last element in the queue
    node->m_Next = NULL;

    if (queue->m_First == NULL) {
        queue->m_First = node;
    } else {
        queue->m_Last->m_Next = node;
    }

    queue->m_Last = node;
    
    g_UsedCount--;
    g_AvailableCount++;

    // this function doesn't actually do any checks right now,
    // so the operation always succeeds
    return 1;
}

void initializeMemoryQueue(MemoryQueue* queue, Node* first) {
    Node* currentNode;
    Node* nextNode;

    // for pointer arithmetic
    U32 nextNodeAddress;

    int i;

    s_InitialFront = first; // save initial beginning of heap
    currentNode = first;

    // partition heap memory into a linked list of equal sized nodes
    for (i = 0; i < (NUM_BLOCKS - 1); i++) {
        nextNodeAddress = (U32)currentNode + sizeof(Node) + sizeof(Envelope) + BLOCK_SIZE;
        nextNode = (Node*)nextNodeAddress;
        currentNode->m_Next = nextNode;
        currentNode = nextNode;
    }

    // make sure last node points to null
    currentNode->m_Next = (Node *)NULL;

    queue->m_First = first;
    queue->m_Last = currentNode;
    
    g_UsedCount = 0;
    g_AvailableCount = NUM_BLOCKS;
}

int isEmptyMemoryQueue(MemoryQueue* queue) {
    return queue->m_First == NULL;
}

int isValidNode(MemoryQueue* queue, Node* node) {
    Node* currentNode = queue->m_First;

    // convert pointers to integers for address comparisons
    U32 nodeAddress = (U32)node;
    U32 heapBeginAddress = (U32)s_InitialFront;

    // check that nodeAddress is between the first and last node of the queue
    if (nodeAddress < heapBeginAddress || nodeAddress > heapBeginAddress + (NUM_BLOCKS - 1) * (sizeof(Node) + sizeof(Envelope) + BLOCK_SIZE)) {
        return 0;
    } else if ((nodeAddress - heapBeginAddress) % (sizeof(Node) + sizeof(Envelope) + BLOCK_SIZE) != 0) { // check that nodeAddress occurs at some integer multiple of sizeof(Node) + BLOCK_SIZE
        return 0;
    }

    while (currentNode != NULL) {
        if (currentNode == node) { // the specified node is already in the queue
            return 0;
        }
        currentNode = currentNode->m_Next;
    }
    return 1; // passed all the tests! :)
}
