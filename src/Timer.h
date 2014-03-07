/**
 * @brief timer.h - Timer header file
 * @author Y. Huang
 * @date 2013/02/12
 */
#ifndef _TIMER_H_
#define _TIMER_H_

#include <stdint.h>

int k_delayed_send(int process_id, void *message_envelope, int delay); // message sent to process_id after delay
void initializeTimerProcess(void); // initializes the timer i-process

extern uint32_t timer_init ( uint8_t n_timer );  /* initialize timer n_timer */

#endif /* ! _TIMER_H_ */
