/**
 * @file:   crt_proc.c
 * @brief:  CRT Display
 * @author: Andrew Gillies, Delia Yao, Joohee Lee, Yiying Ho
 * @date:   2014/03/01
 */

#include "rtx.h"
#include "Polling/uart_polling.h"
#include "crt_proc.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* DEBUG_0 */

void CRTproc(void) {
	Letter* get_msginfo;
	int test;

	while(1) {
		// test = PID of sender
		get_msginfo = (Letter*) receive_message(&test);
		//check if we have HOTKEY
		//else do the following
		
		//send a message to UARTiprocess
		
		relaese_memory_block((void *) get_msginfo);
		
		release_processor();
	}
}

