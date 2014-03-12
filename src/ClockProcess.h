/**
 * @file:   ClockProcess.c
 * @brief:  Wall Clock Process
 */
 
#ifndef _CLOCK_PROCESS_
#define _CLOCK_PROCESS_

#define CLOCK_STRING_LENGTH 8

void initializeClockProcess(void); // initializes wall clock process
void runClockProcess(void);

// wall clock modifiers
void resetClock(void); // reset the wall clock to "00:00:00"
int setClock(char[]); // set the wall clock, returns the success of the operation
void updateClock(void); // increments the wall clock by 1 second

#endif /*  _CLOCK_PROCESS_ */
