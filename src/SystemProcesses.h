/**
 * @file:   SystemProcesses.h
 * @brief:  System process initialization
 */
 
#ifndef _SYSTEM_PROCESSES_
#define _SYSTEM_PROCESSES_

void initializeSystemProcesses(void); // initializes all system processes

void initializeNullProcess(void); // initializes the null process
void initializeCRTProcess(void); // initializes the CRT process
void initializeKCDProcess(void); // initializes the KCD process

void runNullProcess(void);
void runCRTProcess(void);
void runKCDProcess(void);

#endif /* _SYSTEM_PROCESSES_ */
