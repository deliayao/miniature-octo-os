/**
 * @file:   SystemProcesses.h
 * @brief:  System processes (null process, KCD, CRT)
 */
 
#ifndef _SYSTEM_PROCESSES_
#define _SYSTEM_PROCESSES_

#include "Utilities/Definitions.h"
#include "Utilities/Types.h"

/**
 * Command structure for storing commands and their associated process.
 */
typedef struct Command {
   int commandPID;
   char commandText[MAX_COMMAND_LENGTH + 2];
} Command;

/**
 * Adds (registers) a command to the KCD's command table.
 * 
 * @param   text The command that the process wishes to register.
 * @param   registerPID The PID of the registering process.
 */
void addCommand(char text[], int registerPID);

/**
 * Sets all characters of KCD's buffer to null.
 */
void clearBuffer(void);

/**
 * Deletes the last character from KCD's buffer if it is not empty.
 */
void deleteFromBuffer(void);

/**
 * Gets the process ID associated with the command contained in buffer.
 * 
 * @param   buffer The specified command.
 * @return  The process ID associated with the command if it exists, -1
 *          otherwise.
 */
int getCommandProcess(char buffer[]);

/**
 * Initializes the CRT process table item.
 */
void initializeCRTProcess(void);

/**
 * Initializes the KCD process table item.
 */
void initializeKCDProcess(void);

/**
 * Initializes the null process table item.
 */
void initializeNullProcess(void);

/**
 * Initializes all system process table items. Called during process
 * initialization.
 */
void initializeSystemProcesses(void);

/**
 * The CRT process. This is the function that is run when the CRT process is
 * scheduled.
 */
void runCRTProcess(void);

/**
 * The KCD process. This is the function that is run when the KCD process is
 * scheduled.
 */
void runKCDProcess(void);

/**
 * The null process. This is the function that is run when the null process is
 * scheduled.
 */
void runNullProcess(void);

/**
 * Appends a character to the end of KCD's buffer.
 * 
 * @param   character The character to append.
 */
void writeToBuffer(char character);

#endif /* _SYSTEM_PROCESSES_ */
