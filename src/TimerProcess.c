/**
 * @file:   TimerProcess.c
 * @brief:  Timer interrupt process implementation
 */

#include "TimerProcess.h"
#include "Utilities/MessageQueue.h"
#include "Utilities/MemoryQueue.h"
#include "Utilities/ProcessQueue.h"
#include "Utilities/Types.h"
#include "k_process.h"
#include <LPC17xx.h>
#include "Polling/uart_polling.h"

#include "printf.h"

static MessageQueue centralMailbox;
extern MemoryQueue heap;
extern PCB* currentProcess; // points to the current RUNNING process
extern volatile uint32_t g_timer_count;

PROC_INIT timerProcess;

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
    
    return deliverMessage(process_id, envelope);
}

void initializeTimerProcess() {
    initializeMessageQueue(&centralMailbox);
	timerProcess.m_pid = (U32)TIMER_IPROCESS;
	timerProcess.m_priority = NULL_PRIORITY;
	timerProcess.m_stack_size = 0x100;
	timerProcess.mpf_start_pc = &runTimerProcess;
}

void runTimerProcess() {
    Envelope* newMessage;
    U32 messageBody;
    int senderID = -1;
    
    while (1) {
        #ifdef DEBUG_0
        uart0_put_char('.');
        #endif /* DEBUG_0 */
        
        // get current mail
        newMessage = nonBlockingReceiveMessage(TIMER_IPROCESS, &senderID);
        while (newMessage != NULL) {
            insertEnvelope(&centralMailbox, newMessage);
        }
            
        // send all expired mail
        while (centralMailbox.m_First != NULL && centralMailbox.m_First->m_Expiry <= g_timer_count) {
            newMessage = dequeueEnvelope(&centralMailbox);
            messageBody = (U32)newMessage + sizeof(Envelope);
            k_send_message(newMessage->m_DestinationPID, (void *)messageBody);
        }
        
        __enable_irq();
        release_processor();
    }
    
}
