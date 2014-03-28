/**
 * @file:   ClockProcess.c
 * @brief:  Wall Clock Process
 */
 
#ifndef _CLOCK_PROCESS_
#define _CLOCK_PROCESS_

#include "Utilities/Types.h"

/**
 * Length of the wall clock time's string representation.
 */
#define CLOCK_STRING_LENGTH 8

/**
 * Initializes the clock process table item. Called during process
 * initialization.
 */
void initializeClockProcess(void);

/**
 * Wall clock modifier. Resets the wall clock to "00:00:00".
 */
void resetClock(void);

/**
 * The wall clock process. This is the function that is run when the wall clock
 * process is scheduled.
 */
void runClockProcess(void);

/**
 * Wall clock modifier. Sets the wall clock to a specified time.
 * 
 * @param   command The user command containing the specified time.
 * @return  The success or failure of the operation (fails if user input is
 *          invalid).
 */
int setClock(char[] command);

/**
 * Starts the loop of self-sent delayed messages, which will allow the wall
 * clock to update itself every second.
 * 
 * @param   toCRT The letter containing the string representation of the
 *                current time to be displayed (sent to the CRT to be printed).
 */
void startRelay(Letter* toCRT);

/**
 * Wall clock modifier. Increments the wall clock by 1 second.
 */
void updateClock(void);

#endif /* _CLOCK_PROCESS_ */
