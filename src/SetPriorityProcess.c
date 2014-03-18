/**
 * @file:   SetPriorityProcess.c
 * @brief:  Set Priority process
 */
 
#include "SetPriorityProcess.h"

#include "Utilities/String.h"
#include "Polling/uart_polling.h"

#include <LPC17xx.h>

#ifdef DEBUG_0
#include "printf.h"
#endif /* DEBUG_0 */

PROC_INIT setPriorityProcess;

void initializeSetPriorityProcess(void) {
	setPriorityProcess.m_pid = (U32)PROCESS_SET_PRIORITY;
	setPriorityProcess.m_priority = PRIVILEGED;
	setPriorityProcess.m_stack_size = 0x100;
	setPriorityProcess.mpf_start_pc = &runSetPriorityProcess;
}



void runSetPriorityProcess(void) {
	Letter* message;
	Letter* registerCommand;
	int sender;
	// register the commands to kcd
	registerCommand = (Letter*)request_memory_block(); 	
	registerCommand->m_Type = KCD_REG;
	registerCommand->m_Text[0] = '%';
	registerCommand->m_Text[1] = 'C';
    registerCommand->m_Text[2] = '\0';
    send_message(KCD_PROCESS, (void *)registerCommand);
	
	while(1) {
		
		message = (Letter*)receive_message(&sender);
		if (sender == KCD_PROCESS) {
			int commandLength;
			int processID;
			int newPriority;
			commandLength = strlen(message->m_Text);
			
			if (commandLength == 6) {
				processID = message->m_Text[3] - '0';
				newPriority = message->m_Text[5] - '0';
				
				if (processID >= PROCESS_1 && processID <= PROCESS_C) { // Check if process ID is between 1 to 9
					if (newPriority >= HIGH && newPriority <= LOWEST) { // check if new priority is between 1 to 4
						
						set_process_priority(processID, newPriority);
                        release_memory_block((void*)message);
					} else { // priority is not between 1 to 4
						message->m_Type = DEFAULT;
						strcpy("\r\nBad Priority\r\n", message->m_Text);
						send_message(CRT_PROCESS, (void*)message);
					}
				} else {
                    message->m_Type = DEFAULT;
                    strcpy("\r\nBad ID\r\n", message->m_Text);
					send_message(CRT_PROCESS, (void*)message);
				}
			} else {
                message->m_Type = DEFAULT;
				strcpy("\r\nBad Format\r\n", message->m_Text);
				send_message(CRT_PROCESS, (void*)message);
			}
		} else {
            release_memory_block((void*) message);
        }
		release_processor();
	}
	
}


