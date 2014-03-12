/**
 * @file:   ClockProcess.c
 * @brief:  Wall clock process
 */
 
#include "ClockProcess.h"

#include "rtx.h"
#include "Utilities/String.h"
#include "Polling/uart_polling.h"

#include <LPC17xx.h>

//#ifdef DEBUG_0
#include "printf.h"
//#endif /* DEBUG_0 */

extern volatile uint32_t g_timer_count; // get the g_timer_count value, in milliseconds
PROC_INIT clockProcess;

char clockString[CLOCK_STRING_LENGTH + 1]; // the message to CRT

// if clock time is changed by user,
// ignore the next message that comes in from the wall clock
// because it will be from before the time was set
int ignoreNext;

// 1 (true) if the clock is running
int isRunning;

uint32_t start;
uint32_t end;
uint32_t baseTime = 0; // in seconds

void initializeClockProcess(void) {
    ignoreNext = 0;
    isRunning = 0;
    
	clockProcess.m_pid = (U32)CLOCK_PROCESS;
	clockProcess.m_priority = PRIVILEGED;
	clockProcess.m_stack_size = 0x200;
	clockProcess.mpf_start_pc = &runClockProcess;
}

void startRelay(Letter* toCRT, Letter* toWallClock) {
    strcpy(clockString, toCRT->m_Text);
    send_message(CRT_PROCESS, (void*)toCRT); // for display
    
    if (!isRunning) {
        // start relaying messages to ourselves with a 1 second delay
        toWallClock->m_Type = DEFAULT;
        toWallClock->m_Text[0] = 'a';
        delayed_send(CLOCK_PROCESS, (void*)toWallClock, 1000);
        
        isRunning = 1;
    } else {
        ignoreNext = 1;
    }
}

void runClockProcess(void) {
	Letter* message; 
	int sender;
    
    Letter* registerCommandWS;
    Letter* registerCommandWT;
    Letter* registerCommandWR;
    
	// register the commands to kcd
	registerCommandWS = (Letter*)request_memory_block(); 	
	registerCommandWS->m_Type = KCD_REG;
	registerCommandWS->m_Text[0] = '%';
	registerCommandWS->m_Text[1] = 'W';
    registerCommandWS->m_Text[2] = 'S';
    registerCommandWS->m_Text[3] = '\0';
    send_message(KCD_PROCESS, (void *)registerCommandWS);
    
    registerCommandWT = (Letter*)request_memory_block(); 	
	registerCommandWT->m_Type = KCD_REG;
	registerCommandWT->m_Text[0] = '%';
	registerCommandWT->m_Text[1] = 'W';
    registerCommandWT->m_Text[2] = 'T';
    registerCommandWT->m_Text[3] = '\0';
    send_message(KCD_PROCESS, (void *)registerCommandWT);
    
    registerCommandWR = (Letter*)request_memory_block(); 	
	registerCommandWR->m_Type = KCD_REG;
	registerCommandWR->m_Text[0] = '%';
	registerCommandWR->m_Text[1] = 'W';
    registerCommandWR->m_Text[2] = 'R';
    registerCommandWR->m_Text[3] = '\0';
	send_message(KCD_PROCESS, (void *)registerCommandWR);
	
	while(1) {
		message = (Letter*)receive_message(&sender);
        //send_message(CRT_PROCESS, (void*)message);
        if (sender == KCD_PROCESS) {
            int commandLength;
            char command;
            
            // we just got a command
            // parse the command and set the clock accordingly
            commandLength = strlen(message->m_Text);
            if (commandLength >= 3) {
                command = message->m_Text[2];
                if (command == 'S') { // start
                    // for %WS, the command must contain a start time
                    if (commandLength >= 12) {
                        setClock(message->m_Text);
                        
                        // reuse the letter from KCD (overwrite the text)
                        startRelay(message, (Letter*)request_memory_block());
                    }
                } else if (command == 'R') { // reset
                    resetClock();
                    
                    // reuse the letter from KCD (overwrite the text)
                    startRelay(message, (Letter*)request_memory_block());
                } else if (command == 'T') { // terminate
                    isRunning = 0;
                    ignoreNext = 1;
                    release_memory_block((void*)message);
                }
            }
		} else if (sender == CLOCK_PROCESS) {
            if (isRunning && !ignoreNext) {
                Letter* toCRT;
                updateClock(); // increment clock
                
                // a second has passed since we last ran
                // relay the message back to ourselves with a delay of 1 second
                delayed_send(CLOCK_PROCESS, (void*)message, 1000);
                
                // send a new message to the CRT to display the time
                toCRT = (Letter*)request_memory_block();
                toCRT->m_Type = DEFAULT;
                strcpy(clockString, toCRT->m_Text);
                send_message(CRT_PROCESS, (void*)toCRT);
            } else {
                ignoreNext = 0;
                release_memory_block((void*)message); // release memory of ignored message
            }
        }
        release_processor();
	}
}

void updateClock() {
	uint32_t hour;
	uint32_t minute;
	uint32_t second;
	uint32_t duration;
	end = g_timer_count;
	duration = (end - start);	// convert to second
	
    baseTime += 1;
    // convert baseTime to HH:MM:SS formate
    hour = baseTime/3600;
    minute = (baseTime%3600)/60;
    second = baseTime%60;
    
    clockString[0] = hour/10 + '0';
    clockString[1] = hour%10 + '0';
    clockString[3] = minute/10 + '0';
    clockString[4] = minute%10 + '0';
    clockString[6] = second/10 + '0';
    clockString[7] = second%10 + '0';
    clockString[8] = '\0';
}
	
void resetClock(void) {
	int i;
	for (i = 0; i < CLOCK_STRING_LENGTH; i++) {
		if (i == 0 || i == 1 || i == 3 || i == 4 || i == 6 || i == 7 ) {
			clockString[i] = '0';
		} else {
			clockString[i] = ':';
		}
	}
    clockString[8] = '\0';
	baseTime = 0;
	start = g_timer_count;	//set start time 
}

void setClock(char command[]) {
	int i;
	// check if the set clock is valid time (should be between 00:00:00 and 23:59:59)
	if(command[3] == ' ' 
		&& command[6] == ':' 
		&& command[7] >= '0' && command[7] <= '5' 
		&& command[8] >= '0' && command[8] <= '9'
		&& command[9] == ':' 
		&& command[10] >= '0' && command[10] <= '5'
		&& command[11] >= '0' && command[11] <= '9'
		&& ((command[4] == '0' || command[4] == '1') && (command[5] >= '0' && command[5] <= '9')) || (command[4] == '2' && (command[5] >= '0' && command[5] <= '3'))) {

			baseTime = ((command[4] - '0')*10 + command[5] - '0')*3600 + ((command[7] - '0')*10 + command[8] - '0')*60 + ((command[10] - '0')*10 + command[11] - '0');
			
			for(i = 0; i < CLOCK_STRING_LENGTH; i++){
				clockString[i] = command[i+4];
			}
            clockString[8] = '\0';
	}
	start = g_timer_count;	// set start time
}

