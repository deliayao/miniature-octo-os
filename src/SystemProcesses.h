/**
 * @file:   SystemProcesses.h
 * @brief:  System process initialization
 */
 
#ifndef _SYSTEM_PROCESSES_
#define _SYSTEM_PROCESSES_

#include "rtx.h"

void initializeSystemProcesses(void); // initializes all system processes

void initializeNullProcess(void); // initializes the null process
void initializeCRTProcess(void); // initializes the CRT process
void initializeKCDProcess(void); // initializes the KCD process

void runNullProcess(void);
void runCRTProcess(void);
void runKCDProcess(void);


//KCD helper functions and structures

/* initialization table item */
typedef struct Command {
   int commandPID;
   char commandText[MAX_COMMAND_LENGTH + 2];
} Command;

void initializeKCDProcess(void);
void kcd_proc(void);
void addCommand(char* mtext, int register_pid);
void checkMsgInfo(Letter* get_msginfo);
int getCommandProcess(char* buffer); // check if buffer is a command, if it is return the process id, otherwise it returns -1

// buffer helper functions
void clearBuffer(void); // sets all characters to null
void writeToBuffer(char character); // appends a letter to the end of the buffer
void deleteFromBuffer(void); // deletes the last character from the buffer

#endif /* _SYSTEM_PROCESSES_ */
