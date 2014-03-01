/**
 * @file:   Types.h
 * @brief:  Kernel type definitions for data types and data structures
 */
 
#ifndef _TYPES_
#define _TYPES_

#include "Definitions.h"

// data types
typedef unsigned char U8;
typedef unsigned int U32;

// memory node data structure for memory queue
typedef struct Node {
    struct Node* m_Next; // pointer to the next memory block in the queue
} Node;

// message envelope data structure for message queue
typedef struct Envelope {
    struct Envelope* m_Next; // pointer to the next envelope in the queue
    
    int m_SenderPID; // ID of source process
    int m_DestinationPID; // ID of destination process
	int m_Expiry; // message will be sent after this time is reached
} Envelope;

#endif /* _TYPES_ */
