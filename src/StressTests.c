/**
 * @file:   StressTests.c
 * @brief:  Unit tests that test the system's performance on limited resources.
 */

#include "StressTests.h"

#include "rtx.h"
#include "Utilities/MessageQueue.h"
#include "Utilities/String.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* DEBUG_0 */

// stress processes initialization table
PROC_INIT stressProcesses[NUM_STRESS_PROCS];

void setStressTestProcesses() {
	int i;
	
	for (i = 0; i < NUM_STRESS_PROCS; i++) {
		stressProcesses[i].m_pid = (U32)(i + 7);
		stressProcesses[i].m_stack_size = 0x100;
	}

	stressProcesses[0].m_priority = HIGH;
	stressProcesses[1].m_priority = HIGH;
	stressProcesses[2].m_priority = HIGH;
	
	stressProcesses[0].mpf_start_pc = &stressTestA;
	stressProcesses[1].mpf_start_pc = &stressTestB;
	stressProcesses[2].mpf_start_pc = &stressTestC;
}

void stressTestA(void) {
    Letter* commandRegistration;
    Letter* message;
    int count = 0;
    
    // request memory for a command registration message
    commandRegistration = (Letter*)request_memory_block();
    
    commandRegistration->m_Type = KCD_REG;
	commandRegistration->m_Text[0] = '%';
	commandRegistration->m_Text[1] = 'Z';
	commandRegistration->m_Text[2] = '\0';
    
    send_message(KCD_PROCESS, (void*)commandRegistration); // register command
    
    while (1) {
        message = (Letter*)receive_message(NULL);
        
        if (strequals(message->m_Text, "%Z")) {
            strcpy("\r\n", message->m_Text);
            send_message(CRT_PROCESS, (void*)message);
            break;
        } else {
            release_memory_block((void*)message);
        }
    }
    
    // this loop will execute once the %Z command is received
    while (1) {
        message = (Letter*)request_memory_block();
        message->m_Type = REPORT;
        message->m_Text[0] = count;
        
        // send report to Process B
        send_message(PROCESS_B, (void*)message);
        
        count++;
        release_processor();
    }
}

void stressTestB(void) {
    void* message;
    
    while (1) {
        message = receive_message(NULL);
        send_message(PROCESS_C, message);
    }
}

void stressTestC(void) {
    Letter* message;
    int count;
    
    MessageQueue mailbox;
    initializeMessageQueue(&mailbox);
    
    while (1) {
        if (isEmptyMessageQueue(&mailbox)) {
            message = receive_message(NULL); // block until we receive a message
        } else {
            message = dequeueLetter(&mailbox);
        }
		
        count = message->m_Text[0];
		if (message->m_Type == REPORT && count % 20 == 0) {
            Letter* receivedMessage;
            Letter* delayedMessage;
            
            // send message to CRT
            strcpy("Process C\r\n", message->m_Text);
            send_message(CRT_PROCESS, (void*)message);
            
            // hibernate
            delayedMessage = (Letter*)request_memory_block();
            delayedMessage->m_Type = WAKEUP;
            
            // delay for 10 seconds
            delayed_send(PROCESS_C, (void*)delayedMessage, 10000);
            
            while (1) {
                receivedMessage = (Letter*)receive_message(NULL);
                if (receivedMessage->m_Type == WAKEUP) {
                    release_memory_block((void*)receivedMessage);
                    break;
                } else {
                    enqueueLetter(&mailbox, receivedMessage);
                }
            }
		} else {
            release_memory_block((void*)message);
        }
        release_processor();
    }
}

