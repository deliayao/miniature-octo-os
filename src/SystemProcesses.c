/**
 * @file:   SystemProcesses.c
 * @brief:  Implementation of system processes such as the null process, the CRT process, and the KCD process
 */

#include "rtx.h"
#include "SystemProcesses.h"
#include <LPC17xx.h>
#include "Polling/uart_polling.h"


#include "printf.h"


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

// initialize the table with an array size of 10
struct Command* table [COMMAND_TABLE_SIZE] =  {0}; //THIS COULD BE POTENTIALLY HAZARDOUS, HAVING COMMANDS SET TO 0 BY DEFAULT?

//index used for searching & adding a command in to the kcd table 
int index = 0;

void runKCDProcess(void) {
   
	//DELIA+ANDREW DUMMY runKCD PROCESS
	/*void * message;
    
    while (1) {
        message = receive_message(NULL);
        send_message(CRT_PROCESS, message);
			  release_processor();
    }*/
		
		
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
    char buffer[MAX_LETTER_LENGTH];
    clearBuffer(buffer, MAX_LETTER_LENGTH);
    
    
    while(1) {
        message = (Letter*)receive_message(&sender);
        if (message->m_Type == DEFAULT) { // message type is regular
            if (sender == UART_IPROCESS) {
                if (message->m_Text[0] == '!') { // placeholder for hot keys
                    
                    //insert hotkey handler here
                    //might want to think about releasing the memory since we aren't sending to CRT
                    
                } else { // message contains a character from the console
                        if (message->m_Text[0] == '\n') { //TODO: make sure this comparison works. Return character: check for command, clear buffer
                            int process;
                            send_message(CRT_PROCESS, message);
                            process = getCommandProcess(buffer);
                            if (process != -1) {
                                int i;
                                Letter* commandMessage = (Letter*)request_memory_block();
                                commandMessage->m_Type = DEFAULT;
                                for (i = 0; buffer[i] != '\0'; i++) {
                                    commandMessage->m_Text[i] = buffer[i];
                                }
                                send_message(process, (void*)commandMessage);
                            }
                            clearBuffer(buffer, MAX_LETTER_LENGTH);
                        } else { //normal character
                            if (buffer[MAX_LETTER_LENGTH-2] == '\0'){ // We use -2 because we need to leave room for the enter key
                                int i ;
                                for (i = 0; i < MAX_LETTER_LENGTH-1; i++) {
                                    if (buffer[i] == '\0') {
                                        buffer[i] = message->m_Text[0];
                                    }
                                }
                            }
                        }
                }
            } else { //user process, send text to crt
                send_message(CRT_PROCESS, message);
            }
        } else if (message->m_Type == KCD_REG) { // register command
            int n = getCommandSize(message->m_Text);
            if(n > 1 && n <= MAX_COMMAND_LENGTH){  //can't be size one as there should be a character after %
                if(!(message->m_Text[0] == '\0' || message->m_Text[0] != '%' || message->m_Text[1] == ' ')){
                    message->m_Text[n] = ' ';   //add extra space at the end of the command 
                    if(getCommandProcess(message->m_Text) == -1){
                        addCommand(message->m_Text, sender);
                    }
                }
            }
        }
        release_processor();
    }
}

int getCommandSize(char* command){
    int size = 0;
    while(command[size] != '\0'){
        size++;
    }
    return size;
}
void addCommand(char* mtext, int register_pid) {
	struct Command* newCommand;
	newCommand->commandPID = register_pid;
    newCommand->commandText = mtext;

	for(index = 0; index < COMMAND_TABLE_SIZE; index++){
		if(table[index] == (Command *) NULL){
			table[index] = newCommand;
		}
	}
}


int getCommandProcess(char* buffer){
    int i;
    int j;
    if(buffer[0] == '%') {
        for(i = 0; table[i] != 0 && i < COMMAND_TABLE_SIZE; i++){
            for (j = 0; table[i]->commandText[j] != '\0' && buffer[j] != '\0'; j++) {
                if(table[i]->commandText[j] != buffer[j]){
                    j = -1;
                    break;
                }
            }
            if (j != -1) { //command must be valid
                return table[i]->commandPID;
            }
        }
    }
	// return -1 if it does not find the mtext on the table, else return the process id that register the command
	return -1; 
}

void  clearBuffer(char* buffer, int n){
    int i;
    for (i = 0; i < n; i++){
        buffer[i] = '\0';
    }
}





