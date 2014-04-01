/**
 * @file:   MemoryQueue.h
 * @brief:  Heap utility for managing memory
 */

#ifndef _MEMORY_QUEUE_
#define _MEMORY_QUEUE_

#include "Definitions.h"
#include "Types.h"

/**
 * Queue structure for managing memory. Wraps a first and last pointer.
 */
typedef struct MemoryQueue {
    Node* m_First;
    Node* m_Last;
} MemoryQueue;

/**
 * Removes the first Node of the queue.
 * 
 * @param   queue The memory queue to operate on.
 * @return  The first Node of the queue, or NULL if the queue is empty.
 */
Node* dequeueNode(MemoryQueue* queue);

/**
 * Adds the specified Node to the back of the queue.
 * 
 * @param   queue The memory queue to operate on.
 * @param   node The Node to add.
 * @return  The success (1) or failure (0) of the operation.
 */
int enqueueNode(MemoryQueue* queue, Node* node);

/**
 * Initializes the queue.
 * 
 * @param   queue The memory queue to operate on.
 * @param   node The first Node to add to the queue.
 */
void initializeMemoryQueue(MemoryQueue* queue, Node* node);

/**
 * Checks whether the queue is empty.
 * 
 * @param   queue The memory queue to operate on.
 * @return  1 if the queue is empty, 0 otherwise.
 */
int isEmptyMemoryQueue(MemoryQueue* queue);

/**
 * Checks whether the specified Node occurs at a valid address.
 * 
 * @param   queue The memory queue to operate on.
 * @param   node The Node of interest.
 * @return  1 if the Node is valid, 0 otherwise.
 */
int isValidNode(MemoryQueue* queue, Node* node);

#endif /*_MEMORY_QUEUE_*/
