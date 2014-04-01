/**
 * @file:   k_process.h
 * @brief:  process management header file
 */

#ifndef K_PROCESS_H_
#define K_PROCESS_H_

#include "Utilities/k_rtx.h"
#include "Utilities/PriorityQueue.h"

/**
 * Adds an envelope to the message queue of the specified process. This is a
 * helper function for IPC.
 * 
 * @param   sourceProcess The ID of the sending process.
 * @param   envelopeDestinationProcess The ID of the final receiving process.
 * @param   destinationProcess The ID of the receiving process that will
 *                             temporarily hold the message if a delay is
 *                             specified.
 * @param   message The message to send.
 * @param   delay The amount of time in milliseconds to wait before sending the
 *                message.
 * @return  The success (RTX_OK) or failure (RTX_ERR) of the operation.
 */
int deliverMessage(int sourceProcess, int envelopeDestinationProcess, int destinationProcess, void* message, int delay);

/**
 * Handles a release memory block event. This function preempts if specified.
 * 
 * @param   preempt 1 if this function should be allowed to preempt the current
 *                  process.
 * @return  The success (RTX_OK) or failure (RTX_ERR) of the operation.
 */
int handleMemoryRelease(int preempt);

/**
 * Sends a message to the specified process after a delay.
 * 
 * @param   process_id The ID of the receiving process.
 * @param   message_envelope The message to send.
 * @param   delay The amount of time in milliseconds to wait before sending the
 *                message.
 * @return  The success (RTX_OK) or failure (RTX_ERR) of the operation.
 */
int k_delayed_send(int process_id, void* message_envelope, int delay);

/**
 * Gets the priority of the specified process.
 * 
 * @param   process_id The ID of the process of interest.
 * @return  The priority of the specified process.
 */
int k_get_process_priority(int process_id);

/**
 * Gets a message from the calling process' mailbox if a message is waiting.
 * This primitive is blocking.
 * 
 * @param   sender_id The ID of the sender is written into this address.
 * @return  A pointer to the message.
 */
void* k_receive_message(int* sender_id);

/**
 * Releases the processor to give the kernel a chance to schedule another
 * process.
 * 
 * @return  The success (RTX_OK) or failure (RTX_ERR) of the operation.
 */
int k_release_processor(void);

/**
 * Sends a message to the specified process. This primitive is preemptive.
 * 
 * @param   process_id The ID of the receiving process.
 * @param   message_envelope The message to send.
 * @return  The success (RTX_OK) or failure (RTX_ERR) of the operation.
 */
int k_send_message(int process_id, void* message_envelope);

/**
 * Sets the priority of the specified process.
 * 
 * @param   process_id The ID of the process to change.
 * @param   priority The new priority to change to.
 * @return  The success (RTX_OK) or failure (RTX_ERR) of the operation.
 */
int k_set_process_priority(int process_id, int priority);

/**
 * Gets a message from the calling process' mailbox if a message is waiting.
 * This primitive is non-blocking.
 * 
 * @param   receiverID The ID of the calling process.
 * @param   senderIDOutput The ID of the sender is written into this address.
 * @return  A pointer to the message, or NULL if there are no messages waiting.
 */
void* nonBlockingReceiveMessage(int receiverID, int* senderIDOutput);

/**
 * Sends a message to the specified process. This primitive is non-preemptive.
 * 
 * @param   sourceID The ID of the sending process.
 * @param   destinationID The ID of the receiving process.
 * @param   message The message to send.
 * @return  The success (RTX_OK) or failure (RTX_ERR) of the operation.
 */
int nonPreemptiveSendMessage(int sourceID, int destinationID, void* message);

/**
 * Initializes all processes in the system.
 */
void process_init(void);

/**
 * Switches the current process with another process picked by the scheduler.
 * 
 * @return  The success (RTX_OK) or failure (RTX_ERR) of the operation.
 */
int process_switch(void);

/**
 * Picks the next to run process based on process priority.
 * 
 * @return  A pointer to the next to run process.
 */
PCB* scheduler(void);

/**
 * Fills debug info with all processes within queueNumber.
 * 
 * @param   debugInfo Memory to write to.
 * @param   start First index to write to.
 * @param   queueNumber 0=>ready, 1=>blocked on memory, 2=>blocked on receive.
 */
void serializeQueue(char debugInfo[], int start, int queueNumber);

#define serializeReadyQueue(debugInfo, start) serializeQueue(debugInfo, start, 0)
#define serializeBlockedOnMemoryQueue(debugInfo, start) serializeQueue(debugInfo, start, 1)
#define serializeBlockedOnReceive(debugInfo, start) serializeQueue(debugInfo, start, 2)

extern U32* alloc_stack(U32 size_b); // allocate stack for a process
extern void __rte(void); // pop exception stack frame
extern void set_test_procs(void); // test process initial set up
extern void setStressTestProcesses(void); // stress test processes initialization
extern void initializeSetPriorityProcess(void); // set priority process initialization
extern void initializeClockProcess(void); // clock process initialization
extern void initializeSystemProcesses(void); // null process initialization
extern void initializeTimerProcess(void); // timer i-process initialization
extern void initializeUARTProcess(void); // UART i-process initialization

#endif /* ! K_PROCESS_H_ */
