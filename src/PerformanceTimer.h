/**
 * @ Performance Timer header file
 * Note that performance timer uses timer 1 and it is still initalized in Timer.c's timer_init function
 */
#ifndef _PERFORMANCETIMER_H_
#define _PERFORMANCETIMER_H_

#include <stdint.h>

void performanceTimerStart(void); // begins the performance counter (just sets its global timer variable to 0)
uint32_t performanceTimerEnd(void); // returns incremented value of the timer (doesn't modify it at all)


#endif /* ! _PERFORMANCETIMER_H_ */
