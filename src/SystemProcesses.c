/**
 * @file:   SystemProcesses.c
 * @brief:  Implementation of system processes such as the null process, the CRT process, and the KCD process
 */

#include "SystemProcesses.h"

#include "Utilities/String.h"
#include "Polling/uart_polling.h"

#include <LPC17xx.h>

#ifdef DEBUG_0
#include "printf.h"
#endif /* DEBUG_0 */

PROC_INIT nullProcess;
PROC_INIT CRTProcess;
PROC_INIT KCDProcess;

void initializeSystemProcesses() {
    initializeNullProcess();
    initializeCRTProcess();
    initializeKCDProcess();
}

void initializeNullProcess() {
	nullProcess.m_pid = (U32)NULL;
	nullProcess.m_priority = NULL_PRIORITY;
	nullProcess.m_stack_size = 0x100;
	nullProcess.mpf_start_pc = &runNullProcess;
}

void initializeCRTProcess(void) {
    CRTProcess.m_pid = (U32)CRT_PROCESS;
	CRTProcess.m_priority = PRIVILEGED;
	CRTProcess.m_stack_size = 0x100;
	CRTProcess.mpf_start_pc = &runCRTProcess;
}

void initializeKCDProcess(void) {
    KCDProcess.m_pid = (U32)KCD_PROCESS;
	KCDProcess.m_priority = PRIVILEGED;
	KCDProcess.m_stack_size = 0x100;
	KCDProcess.mpf_start_pc = &runKCDProcess;
}

void runNullProcess() {
	while(1) {
		release_processor();
	}
}

void runCRTProcess(void) {
    LPC_UART_TypeDef *pUart;
    void * message;
    pUart = (LPC_UART_TypeDef *) LPC_UART0;
    
    while (1) {
        message = receive_message(NULL);
        send_message(UART_IPROCESS, message);
        pUart->IER = IER_THRE | IER_RLS | IER_RBR;
		release_processor();
    }
}

//kcd process and helper functions

// initialize the commandTable with an array size of 10
Command commandTable [COMMAND_TABLE_SIZE];
char inputBuffer[MAX_LETTER_LENGTH];
int bufferIndex; // the next index in the buffer that we should be writing into

void runKCDProcess(void) {
    //KCD PSEUDOCODE
    //receive message
    // if (message->type is regular)
    //      ASSERT NOT HOTKEY
    //      its a letter, so send message along to CRT
    //      if not return key and buffer not too large, add to buffer
    //      if return key, check if its command then send to user process if it is (Have to request memory, make sure user deletes it after)
    // else if (message->type is command register)
    //      check command isn't already registered
    //      register command
        
    Letter* message;     
    int sender;
    int i;
    
    clearBuffer();
    
    // initialize command table
    for (i = 0; i < COMMAND_TABLE_SIZE; i++) {
        commandTable[i].commandPID = -1;
    }
    
    while(1) {
        message = (Letter*)receive_message(&sender);
        if (message->m_Type == DEFAULT && sender == UART_IPROCESS) { // message type is regular
            if (message->m_Text[0] == '\r') { // return character: check for command, clear buffer
                int process = getCommandProcess(inputBuffer);
                if (process != -1) {
                    strcpy(inputBuffer, message->m_Text);
                    send_message(process, (void*)message);
                } else {
                    strcpy("\r\n", message->m_Text); // append newline
                    send_message(CRT_PROCESS, (void*)message);
                }
                clearBuffer();
            } else if (message->m_Text[0] == 0x7F) { // backspace
                deleteFromBuffer();
                send_message(CRT_PROCESS, (void*)message);
            } else { // normal character
                if (inputBuffer[MAX_LETTER_LENGTH - 2] == '\0'){ // we use -2 because we need to leave room for the null character
                    writeToBuffer(message->m_Text[0]);
                    send_message(CRT_PROCESS, (void*)message);
                }
            }
        } else if (message->m_Type == KCD_REG) { // register command
            int n = strlen(message->m_Text);
            if (n > 1 && n <= MAX_COMMAND_LENGTH && message->m_Text[0] == '%' && message->m_Text[1] != ' ') {  // can't be size one as there should be a character after %
                message->m_Text[n] = ' ';   // add extra space at the end of the command 
                message->m_Text[n + 1] = '\0';
                if (getCommandProcess(message->m_Text) == -1) { // if command doesn't already exist, register it
                    addCommand(message->m_Text, sender);
                }
            }
            release_memory_block((void*)message);
        }
        release_processor();
    }
}

void addCommand(char text[], int registerPID) {
    int i;

	for(i = 0; i < COMMAND_TABLE_SIZE; i++) {
        if (commandTable[i].commandPID == -1) {
            commandTable[i].commandPID = registerPID;
            strcpy(text, commandTable[i].commandText);
            break;
        }
	}
}


int getCommandProcess(char buffer[]) {
    int i;
    int j;
    if (buffer[0] == '%') {
        for (i = 0; commandTable[i].commandPID != -1 && i < COMMAND_TABLE_SIZE; i++) {
            for (j = 0; commandTable[i].commandText[j] != '\0' && buffer[j] != '\0'; j++) {
                if (commandTable[i].commandText[j] != buffer[j]) {
                    j = -1;
                    break;
                }
            }
            if (j != -1) { //command must be valid
                return commandTable[i].commandPID;
            }
        }
    }
	// return -1 if it does not find the mtext on the table, else return the process id that register the command
	return -1; 
}

void clearBuffer(void) {
    int i;
    for (i = 0; i < MAX_LETTER_LENGTH; i++) {
        inputBuffer[i] = '\0';
    }
    bufferIndex = 0;
}

void writeToBuffer(char character) {
    if (bufferIndex <= MAX_LETTER_LENGTH - 2) { // leave room for null character
        inputBuffer[bufferIndex] = character;
        bufferIndex++;
    }
}

void deleteFromBuffer(void) {
    if (bufferIndex > 0) {
        bufferIndex--;
        inputBuffer[bufferIndex] = '\0';
    }
}


