/**
 * @file:   SetPriorityProcess.c
 * @brief:  Set priority process
 */
 
#include "SetPriorityProcess.h"

#include "rtx.h"
#include "Utilities/String.h"

#include <LPC17xx.h>

#ifdef DEBUG_0
#include "printf.h"
#endif /* DEBUG_0 */

/**
 * Set priority process initialization table item. Initialized with values on
 * an initializeSetPriorityProcess() call.
 */
PROC_INIT g_SetPriorityProcess;

void initializeSetPriorityProcess(void) {
    g_SetPriorityProcess.m_pid = (U32)PROCESS_SET_PRIORITY;
    g_SetPriorityProcess.m_priority = PRIVILEGED;
    g_SetPriorityProcess.m_stack_size = 0x100;
    g_SetPriorityProcess.mpf_start_pc = &runSetPriorityProcess;
}

void runSetPriorityProcess(void) {
    Letter* message;
    
// don't send messages to KCD in performance testing mode
// because the primitives will be modified,
// and doing so messes with the scheduling of the performance tests
#ifndef DEBUG_PERFORMANCE
    Letter* registerCommand;
    
    // register set priority commands to KCD
    registerCommand = (Letter*)request_memory_block();  
    registerCommand->m_Type = KCD_REG;
    strcpy("%C", registerCommand->m_Text);
    send_message(KCD_PROCESS, (void*)registerCommand);
#endif /* !DEBUG_PERFORMANCE */
    
    while (1) {
        int commandLength;
        int processID;
        int newPriority;
        int sender;
        
        message = (Letter*)receive_message(&sender);
        commandLength = strlen(message->m_Text);
        
        if (commandLength == 6 && message->m_Text[4] == ' ') {
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
