/**
 * @file:   MemoryQueue.c
 * @brief:  Heap implementation
 */

#include "MemoryQueue.h"

Node* dequeueNode(MemoryQueue* queue) {
    Node* front = queue->m_First; // this will be NULL if queue is empty

    if (queue->m_First != NULL) {
        queue->m_First = queue->m_First->m_Next;

        // this is true if the queue only had one element
        if (queue->m_First == NULL) {
            queue->m_Last = NULL;
        }
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

    // this function doesn't actually do any checks right now,
    // so the operation always succeeds
    return 1;
}

void initializeMemoryQueue(MemoryQueue* queue, Node* first) {
    Node* currentNode;
    int i;

    currentNode = first;
    
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

    queue->m_First = first;
    queue->m_Last = currentNode;
}

int isEmptyMemoryQueue(MemoryQueue* queue) {
    return queue->m_First == NULL;
}
