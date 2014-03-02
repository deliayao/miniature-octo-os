/**
 * @file:   TimerProcess.h
 * @brief:  Timer interrupt process and 'message hub'
 */

#ifndef _TIMER_PROCESS_
#define _TIMER_PROCESS_

#include "Utilities/MessageQueue.h"

#define __SVC_0  __svc_indirect(0)

int k_delayed_send(int process_id, void *message_envelope, int delay); // message sent to process_id after delay

void initializeTimerProcess(void); // initializes the timer i-process
void runTimerProcess(void);

extern int k_release_processor(void);
#define release_processor() _release_processor((U32)k_release_processor)
extern int __SVC_0 _release_processor(U32 p_func);

#endif /*_TIMER_PROCESS_*/
