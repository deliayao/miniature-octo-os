/**
 * @file:   NullProcess.h
 * @brief:  Null process implementation
 */

#include "rtx.h"
#include "Polling/uart_polling.h"
#include "NullProcess.h"

PROC_INIT nullProcess;

void initializeNullProcess() {
	nullProcess.m_pid = (U32)NULL;
	nullProcess.m_priority = NULL_PRIORITY;
	nullProcess.m_stack_size = 0x100;
	nullProcess.mpf_start_pc = &runNullProcess;
}

void runNullProcess() {
	while(1) {
		release_processor();
	}
}
