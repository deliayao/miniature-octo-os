/**
 * @file:   kcd_process.c
 * @brief:  Keyboard Command Decoder Process Management
 */

#include "kcd_process.h"

#ifdef DEBUG_0
#include "printf.h"
void* uart_msg = null;
// initialize the table



void kcdMain(void) {
	// void pointer pointing at the message body
	uart_msg = receive_message(UART_IPROCESS);
	
	if(uart_msg != null) {
		//check the type (Command/ Regular string)
		
	}
	
	

	
	// if it is command, then add to the table
	
	// if it is regular string, then check if it is the command
	
		// if it is command then send to the process that register it 
	
		// if it is not the command then do nothing
	
	restUARTmsg();
	
}

void addItems(char mtext, int register_pid) {
	
}

// table contense two things, one is the command, and one is which process register the command
void initTable() {
	/*REGISTER_TABLE *commandList;
REGISTER_TABLE *current_node;
commandList = (REGISTER_TABLE*)malloc(sizeof(REGISTER_TABLE));
current_node = list;
*/
}

void resetUARTmsg(){
	uart_msg = null;
}



