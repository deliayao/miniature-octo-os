/**
 * @file:   MemoryQueue.c
 * @brief:  Heap implementation
 */

#include "MemoryQueue.h"

Node* initialFront = NULL;

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
		Node* nextNode;
		
		// for pointer arithmetic
		U32 nextNodeAddress;
	
    int i;

		initialFront = first; // save initial beginning of heap
    currentNode = first;
		
    // partition heap memory into a linked list of equal sized nodes
    for (i = 0; i < (NUM_BLOCKS - 1); i++) {
				nextNodeAddress = (U32)currentNode + sizeof(Node) + BLOCK_SIZE;
				nextNode = (Node*)nextNodeAddress;
        currentNode->m_Next = nextNode;
        currentNode = nextNode;
    }

    // make sure last node points to null
    currentNode->m_Next = (Node *)NULL;

    queue->m_First = first;
    queue->m_Last = currentNode;
}

int isEmptyMemoryQueue(MemoryQueue* queue) {
    return queue->m_First == NULL;
}

int isValidNode(MemoryQueue* queue, Node* node) {
		Node* currentNode = queue->m_First;
	
    // convert pointers to integers for address comparisons
    U32 nodeAddress = (U32)node;
    U32 heapBeginAddress = (U32)initialFront;
		
	
    // check that nodeAddress is between the first and last node of the queue
    if (nodeAddress < heapBeginAddress || nodeAddress > heapBeginAddress + (NUM_BLOCKS - 1) * (sizeof(Node) + BLOCK_SIZE)) {
        return 0;
    } else if ((nodeAddress - heapBeginAddress) % (sizeof(Node) + BLOCK_SIZE) != 0) { // check that nodeAddress occurs at some integer multiple of sizeof(Node) + BLOCK_SIZE
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
