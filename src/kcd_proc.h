**
 * @file:   kcd_process.h
 * @brief:  Keyboard Command Decoder Process Management
 */
 
#ifndef KCD_PROCESS_H_
#define KCD_PROCESS_H_

#include "rtx.h"

/* initialization table item */
typedef struct register_table
{
   int register_pid;
   char mtext;
   struct register_table *next;
}REGISTER_TABLE;

/* ----- Variables -----  
extern unsigned int Image$$RW_IRAM1$$ZI$$Limit; 

extern PROC_INIT g_proc_table[NUM_PROCS];

extern PCB* processTable[NUM_PROCS]; // kernel process table
extern PCB* currentProcess; // points to the current RUNNING process

void memory_init(void);
U32 *alloc_stack(U32 size_b);
void *k_request_memory_block(void);
int k_release_memory_block(void *);
*/

void kcdMain(void);
void addItems(char mtext, int register_pid);
void initTable();
void resetUARTmsg();



#endif /* ! KCD_PROCESS_H_ */
