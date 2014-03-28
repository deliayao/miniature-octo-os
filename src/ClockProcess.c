/**
 * @file:   ClockProcess.c
 * @brief:  Wall clock process
 */
 
#include "ClockProcess.h"

#include "rtx.h"
#include "Utilities/String.h"

#include <LPC17xx.h>

#ifdef DEBUG_0
#include "printf.h"
#endif /* DEBUG_0 */

/**
 * Clock process initialization table item. Initialized with values on an
 * initializeClockProcess() call.
 */
PROC_INIT g_ClockProcess;

/**
 * String representation of the wall clock time, +3 for "\n\r\0".
 */
static char s_ClockDisplay[CLOCK_STRING_LENGTH + 3];

/**
 * Flag to indicate whether to ignore the next delayed message that comes in
 * from the wall clock (we want to do this if the time is changed by the user,
 * because the next message will be from before the time was changed).
 */
static int s_IgnoreNext;

/**
 * Flag to indicate whether the clock is currently running.
 */
static int s_IsRunning;

/**
 * Representation of the wall clock time in seconds.
 */
static U32 s_WallClock;

void initializeClockProcess(void) {
    g_ClockProcess.m_pid = (U32)CLOCK_PROCESS;
    g_ClockProcess.m_priority = PRIVILEGED;
    g_ClockProcess.m_stack_size = 0x100;
    g_ClockProcess.mpf_start_pc = &runClockProcess;
}

void resetClock(void) {
    strcpy("00:00:00\r\n", s_ClockDisplay);
    s_WallClock = 0;
}

void runClockProcess(void) {
    Letter* message; // message to be received into this pointer
    int sender;
    
    Letter* registerCommandWS;
    Letter* registerCommandWT;
    Letter* registerCommandWR;
    
    // initialize flags
    s_IgnoreNext = 0;
    s_IsRunning = 0;
    
    // initialize clock string
    resetClock();
    
    // register wall clock commands to KCD
    registerCommandWS = (Letter*)request_memory_block();    
    registerCommandWS->m_Type = KCD_REG;
    strcpy("%WS", registerCommandWS->m_Text);
    send_message(KCD_PROCESS, (void*)registerCommandWS);
    
    registerCommandWT = (Letter*)request_memory_block();    
    registerCommandWT->m_Type = KCD_REG;
    strcpy("%WT", registerCommandWT->m_Text);
    send_message(KCD_PROCESS, (void*)registerCommandWT);
    
    registerCommandWR = (Letter*)request_memory_block();    
    registerCommandWR->m_Type = KCD_REG;
    strcpy("%WR", registerCommandWR->m_Text);
    send_message(KCD_PROCESS, (void*)registerCommandWR);
    
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

                if (command == 'S') { // start command
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
                } else if (command == 'R') { // reset command
                    resetClock();
                    
                    // reuse the letter from KCD (overwrite the text)
                    startRelay(message);
                } else if (command == 'T') { // terminate command
                    // terminate only has an effect if the clock is actually running
                    if (s_IsRunning) {
                        s_IsRunning = 0;
                        s_IgnoreNext = 1;
                    }

                    strcpy("\r\n", message->m_Text);
                    send_message(CRT_PROCESS, (void*)message);
                }
            }
        } else if (sender == CLOCK_PROCESS) { // received a message from ourself
            if (s_IsRunning && !s_IgnoreNext) {
                Letter* toCRT;
                updateClock(); // increment clock
                
                // a second has passed since we last ran
                // relay the message back to ourselves with a delay of 1 second
                delayed_send(CLOCK_PROCESS, (void*)message, 1000);
                
                // send a new message to the CRT to display the time
                toCRT = (Letter*)request_memory_block();
                toCRT->m_Type = DEFAULT;
                strcpy(s_ClockDisplay, toCRT->m_Text);
                send_message(CRT_PROCESS, (void*)toCRT);
            } else {
                s_IgnoreNext = 0;
                release_memory_block((void*)message); // release memory of ignored message
            }
        }

        release_processor();
    }
}

int setClock(char command[]) {
    // check if the set clock command has a valid time (should be between 00:00:00 and 23:59:59)
    if (command[3] == ' '
        && command[6] == ':'
        && command[7] >= '0' && command[7] <= '5'
        && command[8] >= '0' && command[8] <= '9'
        && command[9] == ':'
        && command[10] >= '0' && command[10] <= '5'
        && command[11] >= '0' && command[11] <= '9'
        && ((command[4] == '0' || command[4] == '1') && (command[5] >= '0' && command[5] <= '9'))
                || (command[4] == '2' && (command[5] >= '0' && command[5] <= '3'))) {
                    
            s_WallClock = ((command[4] - '0') * 10 + command[5] - '0') * 3600 +
                    ((command[7] - '0') * 10 + command[8] - '0') * 60 +
                    ((command[10] - '0') * 10 + command[11] - '0');
            
            strcpy(&(command[4]), s_ClockDisplay);
            s_ClockDisplay[8] = '\n';
            s_ClockDisplay[9] = '\r';
            s_ClockDisplay[10] = '\0';
                    
            return 1; // success
    } else {
        return 0; // fail; user specified time had incorrect format or was invalid
    }
}

void startRelay(Letter* toCRT) {
    Letter* toWallClock; // create a message to send to ourself
    
    // print out the current time
    strcpy("\r\n", toCRT->m_Text);
    strcpy(s_ClockDisplay, &(toCRT->m_Text[2]));
    send_message(CRT_PROCESS, (void*)toCRT);
    
    toWallClock = (Letter*)request_memory_block();
    
    // start relaying messages to ourselves with a 1 second delay
    toWallClock->m_Type = DEFAULT;
    toWallClock->m_Text[0] = '\0';
    delayed_send(CLOCK_PROCESS, (void*)toWallClock, 1000);

    if (!s_IsRunning) {
        s_IsRunning = 1;
    } else {
        s_IgnoreNext = 1;
    }
}

void updateClock() {
    U32 hour;
    U32 minute;
    U32 second;
    
    if (s_WallClock < 86399) { // 86400 seconds in 24 hours
        s_WallClock += 1;
        
        // convert wallClock to hh:mm:ss format
        hour = s_WallClock / 3600;
        minute = (s_WallClock % 3600) / 60;
        second = s_WallClock % 60;
        
        s_ClockDisplay[0] = hour / 10 + '0';
        s_ClockDisplay[1] = hour % 10 + '0';
        s_ClockDisplay[3] = minute / 10 + '0';
        s_ClockDisplay[4] = minute % 10 + '0';
        s_ClockDisplay[6] = second / 10 + '0';
        s_ClockDisplay[7] = second % 10 + '0';
        s_ClockDisplay[8] = '\n';
        s_ClockDisplay[9] = '\r';
        s_ClockDisplay[10] = '\0';
    } else {
        // reset wall clock if we try to increment it from 23:59:59
        resetClock();
    }
}
