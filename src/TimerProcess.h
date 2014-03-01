/**
 * @file:   TimerProcess.h
 * @brief:  Timer interrupt process and 'message hub'
 */

#ifndef _TIMER_PROCESS_
#define _TIMER_PROCESS_

#include "Utilities/MessageQueue.h"

int k_delayed_send(int process_id, void *message_envelope, int delay); // message sent to process_id after delay

void initializeTimerProcess(void); // initializes the timer i-process
void runTimerProcess(void);

#endif /*_TIMER_PROCESS_*/
