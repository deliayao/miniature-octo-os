/* @file:   kcd_proc.c
 * @brief:  Keyboard Command Decoder Process Management
 */

#include "rtx.h"
#include "Polling/uart_polling.h"
#include "kcd_proc.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* DEBUG_0 */

/* --Definition-- */
#define TABLE_SIZE 10

PROC_INIT kcdProcess;

// initialize the table with an array size of 10
struct Command* table [TABLE_SIZE] = {0};
char* buffer;

//index used for searching & adding a command in to the kcd table 
int index = 0;


void initializeKCDProcess() {
	kcdProcess.m_pid = (U32)KCD_PROCESS;
	kcdProcess.m_priority = PRIVILEGED;
	kcdProcess.m_stack_size = 0x100;
	kcdProcess.mpf_start_pc = &kcd_proc;
}

void kcd_proc() {
		int test;
		Letter* get_msginfo = (Letter*)request_memory_block();
	//buffer = (Letter*)request_memory_block();
		while(1) {
			get_msginfo = (Letter*) receive_message(&test);
			uart0_put_string("RECIEVED SUCCESSFULLY\n\r");
			release_processor();
			release_memory_block((void *) get_msginfo);
			//release_memory_block((void *) buffer);
		}
	/*	Letter* get_msginfo = (Letter*)request_memory_block();
		int test;
		int i = 0;
		buffer->m_Type = DEFAULT;
		initializeKCDTable();
		

	// void pointer pointing at the message body 
		while(1) {
	// test = PID of sender
		get_msginfo = (Letter*) receive_message(&test);
			
		if(test == UART_IPROCESS) {
			if(get_msginfo->m_Text[0] != '\n') {
					buffer->m_Text[i] = get_msginfo->m_Text[0];
					i++;
			}else{
				i = 0;
				checkMsgInfo(buffer);
			}
		// call CRT
		} else {
			checkMsgInfo(get_msginfo);
		}
		

		//get_msginfo = (Letter *)NULL;
		release_processor();
	}*/
}

void checkMsgInfo(Letter* get_msginfo) {
	
		//check the type (Command/ Regular string)
 	// if it is regular string, then check if it is the command
		if(get_msginfo->m_Type == DEFAULT) {
				if(get_msginfo->m_Text[0] == '%') {
						if(searchCommand(get_msginfo->m_Text[1]) != -1) { // when it found the m_Text[1] on the table
								delayed_send(searchCommand(get_msginfo->m_Text[1]), (void*) get_msginfo, 100);
						}else { // if it could not find m_Text[1] on the table then just print it for now
									
						}
			}
		} else if (get_msginfo->m_Type == KCD_REG) { // if it is the command string, then add it to the table
				addCommand(get_msginfo->m_Text[1], PROCESS_1);
		}

}


void addCommand(char mtext, int register_pid) {
	struct Command* newCommand;
	newCommand->command_pid = register_pid;
  newCommand->msgtext = mtext;
	
	for(index = 0; index < TABLE_SIZE; index++){
		if(table[index] == (Command *) NULL){
			table[index] = newCommand;
		}
	}
}

int searchCommand(char mtext){
	for(index = 0; index < TABLE_SIZE; index++){
		if(table[index]->msgtext == mtext){
			return table[index]->command_pid;
		}
	}
	// return -1 if it does not find the mtext on the table, else return the process id that register the command
	return -1; 
}
