/**
 * @file:   SetPriorityProcess.h
 * @brief:  Set priority process
 */
 
#ifndef _SET_PRIORITY_PROCESS_
#define _SET_PRIORITY_PROCESS_

/**
 * Initializes the set priority process table item. Called during process
 * initialization.
 */
void initializeSetPriorityProcess(void);

/**
 * The set priority process. This is the function that is run when the set
 * priority process is scheduled.
 */
void runSetPriorityProcess(void);

#endif /*  _SET_PRIORITY_PROCESS_ */
