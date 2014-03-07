/**
 * @file:   SystemProcesses.c
 * @brief:  Implementation of system processes such as the null process, the CRT process, and the KCD process
 */

#include "rtx.h"
#include "SystemProcesses.h"
#include <LPC17xx.h>

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

void runKCDProcess(void) {
    void * message;
    
    while (1) {
        message = receive_message(NULL);
        send_message(CRT_PROCESS, message);
        release_processor();
    }
}

