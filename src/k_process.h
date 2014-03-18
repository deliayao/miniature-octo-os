/**
 * @file:   k_process.h
 * @brief:  process management header file
 */

#ifndef K_PROCESS_H_
#define K_PROCESS_H_

#include "Utilities/k_rtx.h"
#include "Utilities/PriorityQueue.h"


/* ----- Functions ----- */

void process_init(void);               /* initialize all procs in the system */
PCB *scheduler(void);                  /* pick the pid of the next to run process */
int k_release_processor(void);           /* kernel release_process function */
int k_set_process_priority(int process_id, int priority); // sets process priority
int k_get_process_priority(int process_id); // gets process priority
void *k_receive_message(int *sender_id); // blocking receive, if message waiting return envelope to caller
int k_send_message(int process_id, void *message_envelope); // send message to specified process
int k_delayed_send(int process_id, void *message_envelope, int delay); // message sent to process_id after delay

// non-blocking and non-preemptive primitives
void* nonBlockingReceiveMessage(int, int*); // non-blocking receive, returns NULL if there are no messages waiting
int nonPreemptiveSendMessage(int, int, void*); // sends the message to the specified process, does not preempt the sending process

int deliverMessage(int, int, int, void*, int); // adds the envelope to the message queue of the specified destination process
int handleMemoryRelease(int); // handles a release memory block event, prempts if specified
int process_switch(void); // takes in process id to allow iprocesses to override the scheduler
void serializeQueue(char debugInfo[], int start, int queueNumber); //fills debug info with all preocesses within queueNumber, 0=>ready, 1=>blocked on memory, 2=> blocked on receive

#define serializeReadyQueue(debugInfo, start) serializeQueue(debugInfo, start, 0)
#define serializeBlockedOnMemoryQueue(debugInfo, start) serializeQueue(debugInfo, start, 1)
#define serializeBlockedOnReceive(debugInfo, start) serializeQueue(debugInfo, start, 2)

extern U32 *alloc_stack(U32 size_b);   /* allocate stack for a process */
extern void __rte(void);               /* pop exception stack frame */
extern void set_test_procs(void);      /* test process initial set up */
extern void setStressTestProcesses(void); // stress test processes initialization
extern void initializeSetPriorityProcess(void); // set priority process initialization
extern void initializeClockProcess(void); // clock process initialization
extern void initializeSystemProcesses(void); // null process initialization
extern void initializeTimerProcess(void); // timer i-process initialization
extern void initializeUARTProcess(void); // UART i-process initialization

#endif /* ! K_PROCESS_H_ */
