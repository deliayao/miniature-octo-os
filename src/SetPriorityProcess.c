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
    
	// register the commands to kcd
	registerCommand = (Letter*)request_memory_block(); 	
	registerCommand->m_Type = KCD_REG;
	registerCommand->m_Text[0] = '%';
	registerCommand->m_Text[1] = 'C';
	registerCommand->m_Text[2] = '\0';
	send_message(KCD_PROCESS, (void *)registerCommand);
	
	while (1) {
		int commandLength;
		int processID;
		int newPriority;
        int sender;
		
		message = (Letter*)receive_message(&sender);
		commandLength = strlen(message->m_Text);
		
		if (commandLength == 6) {
            // TODO: Check full format. For example, "%C 111" will work right now because we're not checking whitespace.
			processID = message->m_Text[3] - '0';
			newPriority = message->m_Text[5] - '0';
            
            if (processID < PROCESS_1 || processID > PROCESS_C) { // ensure process ID is between 1 to 9
				strcpy("\r\nBad ID\r\n", message->m_Text);
                send_message(CRT_PROCESS, (void*)message);
            } else if (newPriority < HIGH || newPriority > LOWEST) { // ensure new priority is between 1 to 4
				strcpy("\r\nBad Priority\r\n", message->m_Text);
                send_message(CRT_PROCESS, (void*)message);
            } else {
                set_process_priority(processID, newPriority);
                if (sender == KCD_PROCESS) {
                    strcpy("\r\n", message->m_Text);
                    send_message(CRT_PROCESS, (void*)message);
                } else {
                    release_memory_block((void*)message);
                }
            }
		} else {
			message->m_Type = DEFAULT;
			strcpy("\r\nBad Format\r\n", message->m_Text);
			send_message(CRT_PROCESS, (void*)message);
		}
		
		release_processor();
	}
}
