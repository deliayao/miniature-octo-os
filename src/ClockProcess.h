/**
 * @file:   ClockProcess.c
 * @brief:  Wall Clock Process
 */
 
#ifndef _CLOCK_PROCESS_
#define _CLOCK_PROCESS_

#define CLOCK_STRING_LENGTH 8

void initializeClockProcess(void); // initializes wall clock processes
void messageToWallClock(void);
void runClockProcess(void);
void messageToCRT(void);
void updateClock(void);
void resetClock(void); // reset the wall clock to "00:00:00"
void setClock(char* mtext); // set the wall clock to mtext

#endif /*  _CLOCK_PROCESS_ */
