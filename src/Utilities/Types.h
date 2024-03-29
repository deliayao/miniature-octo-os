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

/**
 * Memory node data structure for the heap.
 */
typedef struct Node {
    struct Node* m_Next; // pointer to the next memory block in the queue
} Node;

/**
 * Message envelope data structure for IPC.
 */
typedef struct Envelope {
    struct Envelope* m_Next; // pointer to the next envelope in the queue
    
    int m_SenderPID; // ID of source process
    int m_DestinationPID; // ID of destination process
    int m_Expiry; // message will be sent after this time is reached
} Envelope;

/**
 * Letter data structure for IPC. Letter exposes user info.
 */
typedef struct Letter {
    int m_Type; // message type
    char m_Text[MAX_LETTER_LENGTH]; // message body
} Letter;

#endif /* _TYPES_ */
