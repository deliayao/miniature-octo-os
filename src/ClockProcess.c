/**
 * @file:   ClockProcess.c
 * @brief:  Wall Clock Process
 */
#include "rtx.h"
#include "ClockProcess.h"
#include <LPC17xx.h>
#include "Polling/uart_polling.h"

#ifdef DEBUG_0
#include "printf.h"
#include <stdint.h>

#endif /* DEBUG_0 */

extern volatile uint32_t g_timer_count; // get the g_timer_count value
PROC_INIT CLOCKProcess;
char* clockString; // the message to CRT
uint32_t start;
uint32_t end;
uint32_t baseTime = 0;

void initializeClockProcess(void) {
	CLOCKProcess.m_pid = (U32)CLOCK_PROCESS;
	CLOCKProcess.m_priority = HIGH;  // unsure.. but we know it is unprivileged level since it is a user process
	CLOCKProcess.m_stack_size = 0x100;
	CLOCKProcess.mpf_start_pc = &runClockProcess;
}

void runClockProcess(void){
	Letter* message; 
	int sender;
	
	//register the command to kcd
	Letter* msg_info = (Letter*)request_memory_block(); 	
	msg_info->m_Type = KCD_REG;
	msg_info->m_Text[0] = '%';
	msg_info->m_Text[1] = 'W';
	
	send_message(KCD_PROCESS, (void *)msg_info);
	
	while(1) {
		message = (Letter*)receive_message(&sender);
		if(sender == KCD_PROCESS){
			if(message->m_Text[2] == 'S'){
				setClock(message->m_Text);
			}else if(message->m_Text[2] == 'R'){
				resetClock();
			}
		}
		//not sure if we can update the clock for more than 1 sec.... as it will try to receive a message after this if statement and if there is no message then it will get blocked
		if(message->m_Text[2] != 'T'){
			//if there is no terminate clock command then update the clock
			updateClock();
		}
	}
}

void updateClock(){
	uint32_t hour;
	uint32_t minute;
	uint32_t second;
	uint32_t duration;
	end = g_timer_count;
	duration = (end - start);	// convert to second
	
	if((duration % 1000) == 0){ // update the baseTime every second
		baseTime += 1;
		// convert baseTime to HH:MM:SS formate
		hour = baseTime/3600;
		minute = (baseTime%3600)/60;
		second = baseTime%60;
		
		clockString[0] = hour/10 + '0';
		clockString[1] = hour%10 + '0';
		clockString[2] = minute/10 + '0';
		clockString[3] = minute%10 + '0';
		clockString[4] = second/10 + '0';
		clockString[5] = second%10 + '0';
		
		messageToCRT();
	}
}

void messageToCRT(void){
	int i;
	Letter* clockMessage;
	clockMessage->m_Type = DEFAULT;
	for(i = 0; i < CLOCK_STRING_LENGTH; i++){
		clockMessage->m_Text[i] = clockString[i];
	}
	send_message(CRT_PROCESS, (void *)clockMessage);
}
	
void resetClock(void){
	int i;
	for(i = 0; i < CLOCK_STRING_LENGTH; i++){
		if(i == 0 || i == 1 || i == 3 || i == 4 || i == 6 || i == 7 ){
			clockString[i] = '0';
		}else{
			clockString[i] = ':';
		}
	}
	baseTime = 0;
	start = g_timer_count;	//set start time
	messageToCRT(); //send the 00:00:00 to CRT 
}

void setClock(char* m_Text){
	int i;
	// check if the set clock is valid time (should be bettwen 00:00:00 and 24:59:59)
	if(m_Text[3] == ' ' 
		&& m_Text[6] == ':' 
		&& m_Text[7] >= '0' && m_Text[7] <= '5' 
		&& m_Text[8] >= '0' && m_Text[8] <= '9'
		&& m_Text[9] == ':' 
		&& m_Text[10] >= '0' && m_Text[10] <= '5'
		&& m_Text[11] >= '0' && m_Text[11] <= '9'
		&& ((m_Text[4] == '0' || m_Text[4] == '1') && (m_Text[5] >= '0' && m_Text[5] <= '9')) || (m_Text[4] == '2' && (m_Text[5] >= '0' && m_Text[5] <= '4'))){

			baseTime = ((m_Text[4] - '0')*10 + m_Text[5] - '0')*3600 + ((m_Text[7] - '0')*10 + m_Text[8] - '0')*60 + ((m_Text[10] - '0')*10 + m_Text[11] - '0');
			
			for(i = 0; i < CLOCK_STRING_LENGTH; i++){
				clockString[i] = m_Text[i+4];
			}
	}
	start = g_timer_count;	//set start time
	messageToCRT(); //send the current clock to CRT 
}

