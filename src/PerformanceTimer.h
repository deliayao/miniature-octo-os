/**
 * @file:   PerformanceTimer.h
 * @brief:  Performance timer header file. Note that performance timer uses
 *          timer 1 and it is still initalized in Timer.c's timer_init
 *          function.
 */
 
#ifndef _PERFORMANCETIMER_H_
#define _PERFORMANCETIMER_H_

#include <stdint.h>

/**
 * Gets the elapsed time in milliseconds since the last performanceTimerStart()
 * call.
 * 
 * @return  The value of the timer.
 */
uint32_t performanceTimerEnd(void);

/**
 * Begins the performance counter (sets its global timer variable to 0).
 */
void performanceTimerStart(void); 

#endif /* ! _PERFORMANCETIMER_H_ */
