/* @file:   kcd_proc.c
 * @brief:  Keyboard Command Decoder Process Management
 */

#include "rtx.h"
#include "Polling/uart_polling.h"
#include "kcd_proc.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* DEBUG_0 */

// initialize the table, with linked lists you need to make sure the nodes are being created in valid memory
struct KCDTable* table ;
struct Command* search;
char* buffer;

void KCDproc(void) {
		Letter* get_msginfo;
		int test;
		int i = 0;
		initializeKCDTable();

	// void pointer pointing at the message body 
		while(1) {
	// test = PID of sender
		get_msginfo = (Letter*) receive_message(&test);
			
		if(test == UART_IPROCESS) {
			if(get_msginfo->m_Text[0] != '/n') {
					buffer[i] = get_msginfo->m_Text[0];
					i++;
			}else{
					i = 0;
					checkMsgInfo(buffer);
			}
			// call CRT
		}else {
			checkMsgInfo(get_msginfo);
		}
		

		//get_msginfo = (Letter *)NULL;
		release_processor();
	}
}

void checkMsgInfo(void* get_msginfo) {
	
		//check the type (Command/ Regular string)
 	// if it is regular string, then check if it is the command
		if(get_msginfo->m_Type == DEFAULT) {
				if(get_msginfo->m_Text[0] == '%') {
						if(searchCommand(get_msginfo->m_Text[1]) != -1) { // when it found the m_Text[1] on the table
								delayed_send(searchCommand(get_msginfo->m_Text[1]), (void*) get_msginfo, 100);
						}else { // if it could not find m_Text[1] on the table then just print it for now
							uart1_put_char(get_msginfo->m_Text[1]);
						}
			}
		} else if (get_msginfo->m_Type == KCD_REG) { // if it is the command string, then add it to the table
				addCommand(get_msginfo->m_Text[1], PROCESS_1);
		}

}


void addCommand(char mtext, int register_pid) {
	struct Command* newCommand = table->last->Next;
	newCommand->command_pid = register_pid;
  newCommand->msgtext = mtext;
	newCommand->Next = (Command *) NULL;
	
	if(table->head == NULL){
		table->head = newCommand;
		table->last = newCommand;
	}else{
		table->last->Next = newCommand;
		table->last = newCommand;
	}
}

void initializeKCDTable(void) {
    // make sure last node points to null
		table = (KCDTable *)request_memory_block();
    table->head = (Command *)NULL;
		table->last = (Command *)NULL;
}

int searchCommand(char mtext){
	search = table->head;
	while(search != NULL){
		if(search->msgtext == mtext){
			return search->command_pid;
		}
		search = search->Next;		
	}
	// return -1 if it does not find the mtext on the table, else return the process id that register the command
	return -1; 
}
