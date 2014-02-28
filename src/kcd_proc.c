/**
 * @file:   kcd_process.c
 * @brief:  Keyboard Command Decoder Process Management
 */

#include "kcd_process.h"

#ifdef DEBUG_0
#include "printf.h
void* uart_msg = null;
// initialize the table




void kcdMain(void) {
	// void pointer pointing at the message body
	uart_msg = receive_message(UART_IPROCESS);
	
	//check the type (Command/ Regular string)

	
	// if it is command, then add to the table
	
	// if it is regular string, then check if it is the command
	
		// if it is command then send to the process that register it 
	
		// if it is not the command then do nothing
	
	restUARTmsg();
	
}

void resetUARTmsg(){
	uart_msg = null;
}

// table contense two things, one is the command, and one is which process register the command
