/**
 * @file:   TimerProcess.c
 * @brief:  Timer interrupt process implementation
 */

#include "TimerProcess.h"
#include "Utilities/MessageQueue.h"
#include "Utilities/MemoryQueue.h"
#include "Utilities/ProcessQueue.h"
#include "Utilities/Types.h"
#include "k_process.h"
#include <LPC17xx.h>
#include "Polling/uart_polling.h"

#include "printf.h"

//static MessageQueue centralMailbox;
extern volatile uint32_t g_timer_count;

//PROC_INIT timerProcess;
