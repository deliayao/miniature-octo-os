/**
 * @file:   SystemProcesses.c
 * @brief:  Implementation of system processes (null process, KCD, CRT)
 */

#include "SystemProcesses.h"

#include "rtx.h"
#include "Utilities/String.h"

#include <LPC17xx.h>

#ifdef DEBUG_0
#include "printf.h"
#endif /* DEBUG_0 */


/**
 * CRT process initialization table item. Initialized with values on an
 * initializeCRTProcess() call.
 */
PROC_INIT g_CRTProcess;

/**
 * KCD process initialization table item. Initialized with values on an
 * initializeKCDProcess() call.
 */
PROC_INIT g_KCDProcess;

/**
 * Null process initialization table item. Initialized with values on an
 * initializeNullProcess() call.
 */
PROC_INIT g_NullProcess;

/**
 * Index of the buffer position into which the KCD will write the next time it
 * receives input.
 */
static int s_BufferIndex;

/**
 * KCD command table. The command table can hold up to 10 commands.
 */
static Command s_CommandTable[COMMAND_TABLE_SIZE];

/**
 * KCD buffer to hold input until it is sent to a process (or cleared). The
 * buffer can hold up to 35 characters.
 */
static char s_InputBuffer[MAX_LETTER_LENGTH];

void addCommand(char text[], int registerPID) {
    int i;

    for (i = 0; i < COMMAND_TABLE_SIZE; i++) {
        if (s_CommandTable[i].commandPID == -1) {
            s_CommandTable[i].commandPID = registerPID;
            strcpy(text, s_CommandTable[i].commandText);
            break;
        }
    }
}

void clearBuffer(void) {
    int i;

    for (i = 0; i < MAX_LETTER_LENGTH; i++) {
        s_InputBuffer[i] = '\0';
    }

    s_BufferIndex = 0; // reset buffer index
}

void deleteFromBuffer(void) {
    if (s_BufferIndex > 0) {
        s_BufferIndex--;
        s_InputBuffer[s_BufferIndex] = '\0';
    }
}

int getCommandProcess(char buffer[]) {
    int i;
    int j;

    if (buffer[0] == '%') {
        for (i = 0; s_CommandTable[i].commandPID != -1 && i < COMMAND_TABLE_SIZE; i++) {
            for (j = 0; s_CommandTable[i].commandText[j] != '\0' && buffer[j] != '\0'; j++) {
                if (s_CommandTable[i].commandText[j] != buffer[j]) {
                    j = -1;
                    break;
                }
            }

            if (j != -1) { // command must be valid
                return s_CommandTable[i].commandPID;
            }
        }
    }

    return -1; 
}

void initializeCRTProcess(void) {
    g_CRTProcess.m_pid = (U32)CRT_PROCESS;
    g_CRTProcess.m_priority = PRIVILEGED;
    g_CRTProcess.m_stack_size = 0x100;
    g_CRTProcess.mpf_start_pc = &runCRTProcess;
}

void initializeKCDProcess(void) {
    g_KCDProcess.m_pid = (U32)KCD_PROCESS;
    g_KCDProcess.m_priority = PRIVILEGED;
    g_KCDProcess.m_stack_size = 0x100;
    g_KCDProcess.mpf_start_pc = &runKCDProcess;
}

void initializeNullProcess() {
    g_NullProcess.m_pid = (U32)NULL;
    g_NullProcess.m_priority = NULL_PRIORITY;
    g_NullProcess.m_stack_size = 0x100;
    g_NullProcess.mpf_start_pc = &runNullProcess;
}

void initializeSystemProcesses() {
    initializeNullProcess();
    initializeCRTProcess();
    initializeKCDProcess();
}

void runCRTProcess(void) {
    LPC_UART_TypeDef* pUart;
    void* message;
    pUart = (LPC_UART_TypeDef*) LPC_UART0;
    
    while (1) {
        message = receive_message(NULL);
        send_message(UART_IPROCESS, message); // forward message to UART
        pUart->IER = IER_THRE | IER_RLS | IER_RBR; // trigger interrupt
        release_processor();
    }
}

void runKCDProcess(void) { 
    Letter* message;     
    int sender;
    int i;
    
    // initialize input buffer
    clearBuffer();
    
    // initialize command table
    for (i = 0; i < COMMAND_TABLE_SIZE; i++) {
        s_CommandTable[i].commandPID = -1;
    }
    
    while(1) {
        message = (Letter*)receive_message(&sender);

        if (message->m_Type == DEFAULT && sender == UART_IPROCESS) { // message type is regular
            if (message->m_Text[0] == '\r') { // return character: check for command, clear buffer
                int process = getCommandProcess(s_InputBuffer);
                if (process != -1) {
                    // input is a command, send the command to the corresponding process
                    strcpy(s_InputBuffer, message->m_Text);
                    send_message(process, (void*)message);
                } else {
                    strcpy("\r\n", message->m_Text); // append newline
                    send_message(CRT_PROCESS, (void*)message);
                }
                clearBuffer();
            } else if (message->m_Text[0] == 0x7F) { // backspace
                deleteFromBuffer();
                send_message(CRT_PROCESS, (void*)message);
            } else { // normal character
                if (s_InputBuffer[MAX_LETTER_LENGTH - 2] == '\0') { // we use -2 because we need to leave room for the null character
                    writeToBuffer(message->m_Text[0]);
                    send_message(CRT_PROCESS, (void*)message);
                } else {
                    // if buffer is full, we do not print the character
                    // so release memory block
                    release_memory_block((void*)message);
                }
            }
        } else if (message->m_Type == KCD_REG) { // register command
            int n = strlen(message->m_Text);

            // command can't have size of 1
            // because there should be a character after '%'
            if (n > 1 && n <= MAX_COMMAND_LENGTH && message->m_Text[0] == '%' && message->m_Text[1] != ' ') {
                message->m_Text[n] = ' '; // add an extra space at the end of the command 
                message->m_Text[n + 1] = '\0';

                // if command doesn't already exist, register it
                if (getCommandProcess(message->m_Text) == -1) {
                    addCommand(message->m_Text, sender);
                }
            }

            release_memory_block((void*)message);
        }
        release_processor();
    }
}

void runNullProcess() {
    while(1) {
        release_processor();
    }
}

void writeToBuffer(char character) {
    if (s_BufferIndex <= MAX_LETTER_LENGTH - 2) { // leave room for null character
        s_InputBuffer[s_BufferIndex] = character;
        s_BufferIndex++;
    }
}
