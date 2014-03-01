/**
 * @file:   PostOffice.c
 * @brief:  Post office implementation
 */

#include "PostOffice.h"
#include "Utilities/MessageQueue.h"

MessageQueue centralMailbox;

void initializePostOffice() {
	initializeMessageQueue(&centralMailbox);
}

int k_delayed_send(int process_id, void *message_envelope, int delay) {
	
}
