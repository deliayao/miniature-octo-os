/**
 * @file:   PostOffice.c
 * @brief:  Post office implementation
 */

#include "PostOffice.h"
#include "Utilities/MessageQueue.h"
#include "Utilities/MemoryQueue.h"
#include "Utilities/ProcessQueue.h"
#include "Utilities/Types.h"
#include <LPC17xx.h>

MessageQueue centralMailbox;
extern MemoryQueue heap;
extern PCB* currentProcess; // points to the current RUNNING process
extern volatile uint32_t g_timer_count;

void initializePostOffice() {
	initializeMessageQueue(&centralMailbox);
}

int k_delayed_send(int process_id, void *message_envelope, int delay) {
    Envelope* envelope;
	U32 node = (U32)message_envelope - sizeof(Envelope) - sizeof(Node);
	
	if (!isValidNode(&heap, (Node*)node)) { // make sure it's a valid memory block
		return RTX_ERR;
	}
	
	node += sizeof(Node);
	envelope = (Envelope*)node;
	envelope->m_DestinationPID = process_id;
	envelope->m_SenderPID = currentProcess->m_PID;
	envelope->m_Expiry = g_timer_count + delay;
    
	return insertEnvelope(&centralMailbox, envelope);
}
