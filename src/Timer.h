/**
 * @file:   Timer.h
 * @brief:  Timer header file.
 */

#ifndef _TIMER_H_
#define _TIMER_H_

#include <stdint.h>

/**
 * Initializes the timer i-process table item. Called during process
 * initialization.
 */
void initializeTimerProcess(void);

extern uint32_t timer_init(uint8_t n_timer); // initialize timer n_timer

#endif /* ! _TIMER_H_ */
