/**
 * @file:   ClockProcess.c
 * @brief:  Wall clock process
 */
 
#include "ClockProcess.h"

#include "rtx.h"
#include "Utilities/String.h"
#include "Polling/uart_polling.h"

#include <LPC17xx.h>

#ifdef DEBUG_0
#include "printf.h"
#endif /* DEBUG_0 */

PROC_INIT clockProcess;

char clockDisplay[CLOCK_STRING_LENGTH + 3]; // string representation of the wall clock time, +3 for "\n\r\0"

// if clock time is changed by user,
// ignore the next message that comes in from the wall clock
// because it will be from before the time was set
int ignoreNext;

// 1 (true) if the clock is running
int isRunning;

static U32 wallClock; // representation of the wall clock time in seconds

void initializeClockProcess(void) {
	clockProcess.m_pid = (U32)CLOCK_PROCESS;
	clockProcess.m_priority = PRIVILEGED;
	clockProcess.m_stack_size = 0x100;
	clockProcess.mpf_start_pc = &runClockProcess;
}

void startRelay(Letter* toCRT) {
    Letter* toWallClock;
    
    strcpy("\r\n", toCRT->m_Text);
    strcpy(clockDisplay, &(toCRT->m_Text[2]));
    send_message(CRT_PROCESS, (void*)toCRT); // for display
    
    toWallClock = (Letter*)request_memory_block();
    
    // start relaying messages to ourselves with a 1 second delay
    toWallClock->m_Type = DEFAULT;
    toWallClock->m_Text[0] = '\0';
    delayed_send(CLOCK_PROCESS, (void*)toWallClock, 1000);

    if (!isRunning) {
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
    
    // initialize flags
    ignoreNext = 0;
    isRunning = 0;
    
    // initialize clock string
    resetClock();
    
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
	
	while (1) {
		message = (Letter*)receive_message(&sender);
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
                        if (setClock(message->m_Text)) {
                            // the start command contained a valid time, start relay
                            // reuse the letter from KCD (overwrite the text)
                            startRelay(message);
                        } else {
                            strcpy("\r\n", message->m_Text);
                            send_message(CRT_PROCESS, (void*)message);
                        }
                    }
                } else if (command == 'R') { // reset
                    resetClock();
                    
                    // reuse the letter from KCD (overwrite the text)
                    startRelay(message);
                } else if (command == 'T') { // terminate
                    if (isRunning) { // terminate only has an effect if the clock is actually running
                        isRunning = 0;
                        ignoreNext = 1;
                    }
                    strcpy("\r\n", message->m_Text);
                    send_message(CRT_PROCESS, (void*)message);
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
                strcpy(clockDisplay, toCRT->m_Text);
                send_message(CRT_PROCESS, (void*)toCRT);
            } else {
                ignoreNext = 0;
                release_memory_block((void*)message); // release memory of ignored message
            }
        }
        release_processor();
	}
}

void resetClock(void) {
    strcpy("00:00:00\r\n", clockDisplay);
	wallClock = 0;
}

int setClock(char command[]) {
	// check if the set clock command is valid time (should be between 00:00:00 and 23:59:59)
	if (command[3] == ' '
		&& command[6] == ':'
		&& command[7] >= '0' && command[7] <= '5'
		&& command[8] >= '0' && command[8] <= '9'
		&& command[9] == ':'
		&& command[10] >= '0' && command[10] <= '5'
		&& command[11] >= '0' && command[11] <= '9'
		&& ((command[4] == '0' || command[4] == '1') && (command[5] >= '0' && command[5] <= '9'))
                || (command[4] == '2' && (command[5] >= '0' && command[5] <= '3'))) {
                    
            wallClock = ((command[4] - '0') * 10 + command[5] - '0') * 3600 +
                    ((command[7] - '0') * 10 + command[8] - '0') * 60 +
                    ((command[10] - '0') * 10 + command[11] - '0');
            
            strcpy(&(command[4]), clockDisplay);
            clockDisplay[8] = '\n';
            clockDisplay[9] = '\r';
            clockDisplay[10] = '\0';
                    
            return 1; // success
	} else {
        return 0;
    }
}

void updateClock() {
	U32 hour;
	U32 minute;
	U32 second;
	
    if (wallClock < 86399) { // 86400 seconds in 24 hours
        wallClock += 1;
        
        // convert wallClock to hh:mm:ss format
        hour = wallClock / 3600;
        minute = (wallClock % 3600) / 60;
        second = wallClock % 60;
        
        clockDisplay[0] = hour / 10 + '0';
        clockDisplay[1] = hour % 10 + '0';
        clockDisplay[3] = minute / 10 + '0';
        clockDisplay[4] = minute % 10 + '0';
        clockDisplay[6] = second / 10 + '0';
        clockDisplay[7] = second % 10 + '0';
        clockDisplay[8] = '\n';
        clockDisplay[9] = '\r';
        clockDisplay[10] = '\0';
    } else {
        // reset wall clock if we try to increment it from 23:59:59
        resetClock();
    }
}
