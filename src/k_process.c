/**
 * @file:   k_process.c
 * @brief:  process management C file
 */

#include "k_process.h"

#include "Polling/uart_polling.h"
#include "Utilities/MemoryQueue.h"
#include "Utilities/MessageQueue.h"

#include <LPC17xx.h>
#include <system_LPC17xx.h>

#ifdef DEBUG_0
#include "printf.h"
#endif /* ! DEBUG_0 */

/**
 * The currently running process.
 */
PCB* g_CurrentProcess;

/**
 * Process initialization table for populating the kernel process table.
 */
PROC_INIT g_proc_table[NUM_PROCS];

/**
 * Kernel process table.
 */
PCB* g_ProcessTable[NUM_PROCS];

/**
 * Array of all queued states. This array is iterated when the priority of a
 * process is changed and we must find the process to remove and re-enqueue it.

 */
static PriorityQueue* s_MasterPQs[QUEUED_STATES];

/**
 * Priority queue representing the ready state. All ready processes are queued
 * here.
 */
static PriorityQueue s_ReadyQueue;

/**
 * Priority queue representing the blocked on memory state. All processes that
 * are blocked on memory are queued here.
 */
static PriorityQueue s_BlockedOnMemoryQueue;

// for process initialization
extern PROC_INIT g_test_procs[NUM_TEST_PROCS];
extern PROC_INIT g_StressProcesses[NUM_STRESS_PROCS];
extern PROC_INIT g_NullProcess;
extern PROC_INIT g_ClockProcess;
extern PROC_INIT g_KCDProcess;
extern PROC_INIT g_CRTProcess;
extern PROC_INIT g_TimerProcess;
extern PROC_INIT g_UARTProcess;
extern PROC_INIT g_SetPriorityProcess;

extern MemoryQueue g_Heap;
extern volatile uint32_t g_timer_count;

int deliverMessage(int sourceProcess, int envelopeDestinationProcess, int destinationProcess, void* message, int delay) {
    Envelope* envelope;
    PCB* destination = g_ProcessTable[destinationProcess];
    U32 node = (U32)message - sizeof(Envelope) - sizeof(Node); // for error checking
    
    if (!isValidNode(&g_Heap, (Node*)node)) { // make sure it's a valid memory block
// during performance testing, we repeatedly try to send a message using a dummy memory block
// so the memory block will be invalid, but we want to send it anyway
// so do not return here
#ifndef DEBUG_PERFORMANCE
        return RTX_ERR;
#endif /* DEBUG_PERFORMANCE */
    }
    
    node += sizeof(Node);
    envelope = (Envelope*)node;
    envelope->m_DestinationPID = envelopeDestinationProcess;
    envelope->m_SenderPID = sourceProcess;
    envelope->m_Expiry = g_timer_count + delay;
    
    return enqueueEnvelope(&(destination->m_Mailbox), envelope);
}

int handleMemoryRelease(int preempt) {
    if (!isEmptyPriorityQueue(&s_BlockedOnMemoryQueue)) {
        // clear blocked on memory queue, move processes to ready
        PCB* process = dequeueHighest(&s_BlockedOnMemoryQueue);
        if (process != NULL) {
            process->m_State = READY;
            enqueueAtPriority(&s_ReadyQueue, process);
        }
        if (preempt) {
            return k_release_processor();
        }
    }
    return RTX_OK;
}

int k_delayed_send(int process_id, void *message_envelope, int delay) {
    return deliverMessage(g_CurrentProcess->m_PID, process_id, TIMER_IPROCESS, message_envelope, delay);
}

int k_get_process_priority(int process_id) {
    if (process_id <= 0 || process_id >= NUM_PROCS) { // cannot get priority of null process
        return RTX_ERR;
    }
    return g_ProcessTable[process_id]->m_Priority;
}

void* k_receive_message(int* sender_id) {
    Envelope * envelope;
    
    while (isEmptyMessageQueue(&(g_CurrentProcess->m_Mailbox))) {
        g_CurrentProcess->m_State = BLOCKED_RECEIVE;
        k_release_processor();
    }
    
    envelope = dequeueEnvelope(&(g_CurrentProcess->m_Mailbox));
    if (sender_id != NULL) {
        *sender_id = envelope->m_SenderPID; // return ID of sender
    }
    
    return (void*)((U32)envelope + sizeof(Envelope)); // return the envelope offset by the size of Envelope
}

int k_release_processor(void) {
    return process_switch(); 
}

int k_send_message(int process_id, void *message_envelope) {
    PCB* destination = g_ProcessTable[process_id];
    int result = deliverMessage(g_CurrentProcess->m_PID, process_id, process_id, message_envelope, 0);
    
    // if the destination process is blocked on receive, unblock it
    if (destination->m_State == BLOCKED_RECEIVE) {
        destination->m_State = READY;
        enqueueAtPriority(&s_ReadyQueue, destination);
        
        // preempt current process if destination process has higher priority and is blocked on receive
        if (destination->m_Priority < g_CurrentProcess->m_Priority) {
#ifndef DEBUG_PERFORMANCE // do not preempt if we're in performance testing mode
            return k_release_processor();
#endif /* !DEBUG_PERFORMANCE */
        }
    }
    
    return result;
}

int k_set_process_priority(int process_id, int priority) {
    int oldPriority;
    int i;

    if (process_id <= NULL_PROCESS || process_id > (NUM_TEST_PROCS + NUM_STRESS_PROCS)) { // cannot change priority of system processes
        return RTX_ERR;
    }
    if (priority < HIGH || priority > LOWEST) { // cannot change to PRIVILEGED or NULL_PRIORITY
        return RTX_ERR;
    }

    oldPriority = g_ProcessTable[process_id]->m_Priority;

    if (oldPriority == priority) {
        // expected behaviour:
        // if there is no change in priority, the current process is not preempted
        // and the specified process retains its position in the priority queue
        return RTX_OK;
    }

    // if the current process is the specified process,
    // change its priority and preempt it
    if (g_CurrentProcess->m_PID == process_id) {
        g_CurrentProcess->m_Priority = priority;
        return k_release_processor();
    }
    
    // handle changing priority of a blocked on receive process
    // (blocked on receive processes are not in any priority queue)
    if (g_ProcessTable[process_id]->m_State == BLOCKED_RECEIVE) {
        g_ProcessTable[process_id]->m_Priority = priority;
        return RTX_OK; // should this case preempt?
    } else { // if the specified process is currently queued
        for (i = 0; i < QUEUED_STATES; i++) {
            if (updateProcessPriority(s_MasterPQs[i], process_id, oldPriority, priority)) {
                k_release_processor(); // allow the processor to preempt the current process if it wants to
                return RTX_OK;
            }
        }
    }
    
    return RTX_ERR;
}

void* nonBlockingReceiveMessage(int receiverID, int* senderIDOutput) {
    Envelope* envelope = dequeueEnvelope(&(g_ProcessTable[receiverID]->m_Mailbox));
    if (senderIDOutput != NULL) {
        *senderIDOutput = (envelope == NULL) ? -1 : envelope->m_SenderPID;
    }
    return (envelope == NULL) ? (void*)envelope : (void*)((U32)envelope + sizeof(Envelope)); // return the envelope offset by the size of Envelope
}

int nonPreemptiveSendMessage(int sourceID, int destinationID, void* message) {
    PCB* destination = g_ProcessTable[destinationID];
    int result = deliverMessage(sourceID, destinationID, destinationID, message, 0);
    
    // if the destination process is blocked on receive, unblock it
    if (destination->m_State == BLOCKED_RECEIVE) {
        destination->m_State = READY;
        enqueueAtPriority(&s_ReadyQueue, destination);
    }
    
    return result;
}

void process_init() {
    int i;
    U32 *sp;

    // initialize processes
    initializeSetPriorityProcess();
    initializeClockProcess();
    initializeSystemProcesses();
    initializeTimerProcess();
    initializeUARTProcess();
    
    set_test_procs();
    setStressTestProcesses();

    // system processes
    g_proc_table[NULL_PROCESS].m_pid = g_NullProcess.m_pid;
    g_proc_table[NULL_PROCESS].m_priority = g_NullProcess.m_priority;
    g_proc_table[NULL_PROCESS].m_stack_size = g_NullProcess.m_stack_size;
    g_proc_table[NULL_PROCESS].mpf_start_pc = g_NullProcess.mpf_start_pc;
    
    g_proc_table[PROCESS_SET_PRIORITY].m_pid = g_SetPriorityProcess.m_pid;
    g_proc_table[PROCESS_SET_PRIORITY].m_priority = g_SetPriorityProcess.m_priority;
    g_proc_table[PROCESS_SET_PRIORITY].m_stack_size = g_SetPriorityProcess.m_stack_size;
    g_proc_table[PROCESS_SET_PRIORITY].mpf_start_pc = g_SetPriorityProcess.mpf_start_pc;
    
    g_proc_table[CLOCK_PROCESS].m_pid = g_ClockProcess.m_pid;
    g_proc_table[CLOCK_PROCESS].m_priority = g_ClockProcess.m_priority;
    g_proc_table[CLOCK_PROCESS].m_stack_size = g_ClockProcess.m_stack_size;
    g_proc_table[CLOCK_PROCESS].mpf_start_pc = g_ClockProcess.mpf_start_pc;
    
    g_proc_table[KCD_PROCESS].m_pid = g_KCDProcess.m_pid;
    g_proc_table[KCD_PROCESS].m_priority = g_KCDProcess.m_priority;
    g_proc_table[KCD_PROCESS].m_stack_size = g_KCDProcess.m_stack_size;
    g_proc_table[KCD_PROCESS].mpf_start_pc = g_KCDProcess.mpf_start_pc;
    
    g_proc_table[CRT_PROCESS].m_pid = g_CRTProcess.m_pid;
    g_proc_table[CRT_PROCESS].m_priority = g_CRTProcess.m_priority;
    g_proc_table[CRT_PROCESS].m_stack_size = g_CRTProcess.m_stack_size;
    g_proc_table[CRT_PROCESS].mpf_start_pc = g_CRTProcess.mpf_start_pc;
    
    g_proc_table[TIMER_IPROCESS].m_pid = g_TimerProcess.m_pid;
    g_proc_table[TIMER_IPROCESS].m_priority = g_TimerProcess.m_priority;
    g_proc_table[TIMER_IPROCESS].m_stack_size = g_TimerProcess.m_stack_size;
    g_proc_table[TIMER_IPROCESS].mpf_start_pc = g_TimerProcess.mpf_start_pc;
    
    g_proc_table[UART_IPROCESS].m_pid = g_UARTProcess.m_pid;
    g_proc_table[UART_IPROCESS].m_priority = g_UARTProcess.m_priority;
    g_proc_table[UART_IPROCESS].m_stack_size = g_UARTProcess.m_stack_size;
    g_proc_table[UART_IPROCESS].mpf_start_pc = g_UARTProcess.mpf_start_pc;

    // test processes
    for (i = 1; i <= NUM_TEST_PROCS; i++) {
        g_proc_table[i].m_pid = g_test_procs[i - 1].m_pid;
        g_proc_table[i].m_priority = g_test_procs[i - 1].m_priority;
        g_proc_table[i].m_stack_size = g_test_procs[i - 1].m_stack_size;
        g_proc_table[i].mpf_start_pc = g_test_procs[i - 1].mpf_start_pc;
    }
    
    // stress test processes
    for (i = 1; i <= NUM_STRESS_PROCS; i++) {
        g_proc_table[i + 6].m_pid = g_StressProcesses[i - 1].m_pid;
        g_proc_table[i + 6].m_priority = g_StressProcesses[i - 1].m_priority;
        g_proc_table[i + 6].m_stack_size = g_StressProcesses[i - 1].m_stack_size;
        g_proc_table[i + 6].mpf_start_pc = g_StressProcesses[i - 1].mpf_start_pc;
    }

    // initialize exception stack frame (i.e. initial context) for each process
    for ( i = 0; i < NUM_PROCS; i++ ) {
        int j;
        (g_ProcessTable[i])->m_PID = (g_proc_table[i]).m_pid;
        (g_ProcessTable[i])->m_Priority = (g_proc_table[i]).m_priority;
        (g_ProcessTable[i])->m_State = NEW;
        initializeMessageQueue(&((g_ProcessTable[i])->m_Mailbox));

        sp = alloc_stack((g_proc_table[i]).m_stack_size);
        *(--sp)  = INITIAL_xPSR; // user process initial xPSR
        *(--sp)  = (U32)((g_proc_table[i]).mpf_start_pc); // PC contains the entry point of the process
        for ( j = 0; j < 6; j++ ) { // R0-R3, R12 are cleared with 0
            *(--sp) = 0x0;
        }
        (g_ProcessTable[i])->m_ProcessSP = sp;
    }

    // initialize priority queues
    initializePriorityQueue(&s_ReadyQueue);
    initializePriorityQueue(&s_BlockedOnMemoryQueue);

    s_MasterPQs[0] = &s_ReadyQueue;
    s_MasterPQs[1] = &s_BlockedOnMemoryQueue;

    // all processes are currently new and ready
    for (i = 0; i < NUM_PROCS - NUM_IPROCS; i++) {
        enqueueAtPriority(&s_ReadyQueue, g_ProcessTable[i]);
    }

    // no running process
    g_CurrentProcess = NULL;
}

int process_switch() {
    PCB* oldProcess;

    if (g_CurrentProcess != NULL) {
        // if current process is an i-process, don't add it to any priority queue
        // else, add process to appropriate queue and save context
        if (g_CurrentProcess->m_State == BLOCKED_RECEIVE) {
        } else if (g_CurrentProcess->m_State == BLOCKED_MEM) { // blocked on memory
            enqueueAtPriority(&s_BlockedOnMemoryQueue, g_CurrentProcess);
        } else { // ready
            enqueueAtPriority(&s_ReadyQueue, g_CurrentProcess);
            g_CurrentProcess->m_State = READY;
        }
    }

    oldProcess = g_CurrentProcess;
    g_CurrentProcess = scheduler();

    // save context of old process if we're actually switching
    if (g_CurrentProcess != oldProcess) {
        if (oldProcess != NULL) {
            oldProcess->m_ProcessSP = (U32 *)__get_MSP();
        }
        
        if (g_CurrentProcess->m_State == READY || g_CurrentProcess->m_State == NEW) {
            ProcessState state = g_CurrentProcess->m_State;
            g_CurrentProcess->m_State = RUNNING;
            __set_MSP((U32) g_CurrentProcess->m_ProcessSP); // switch to the new processes's stack

            if (state == NEW) {
                __rte(); // pop exception stack frame from the stack for new processes
            }
        } else { // if the scheduler chose a process that isn't READY or NEW, something broke
            g_CurrentProcess = oldProcess;
            return RTX_ERR;
        }
    } else {
        g_CurrentProcess->m_State = RUNNING;
    }

    return RTX_OK;
}

PCB* scheduler(void) {
    return dequeueHighest(&s_ReadyQueue);
}

void serializeQueue(char debugInfo[], int start, int queueNumber){
    int j = start;
    if (queueNumber == 2) { // blocked on receive, special case
        int i;
        for ( i = 0; i < NUM_PROCS; i++ ) {
            if (g_ProcessTable[i]->m_State == BLOCKED_RECEIVE) {
                debugInfo[j] = '(';
                j++;
                if (g_ProcessTable[i]->m_PID / 10 > 0) {// assuming less than 99 processes
                    debugInfo[j] = (g_ProcessTable[i]->m_PID/10) + '0';
                    j++;
                }
                debugInfo[j] = (g_ProcessTable[i]->m_PID % 10) + '0';
                j++;
                debugInfo[j] = ',';
                j++;
                debugInfo[j] = g_ProcessTable[i]->m_Priority + '0';
                j++;
                debugInfo[j] = ')';
                j++;
                debugInfo[j] = '\r';
                j++;
                debugInfo[j] = '\n';
                j++;
            }
        }
        debugInfo[j] = '\0';
    } else { // any priority queued state
        if (queueNumber >= 0 && queueNumber < QUEUED_STATES) {
            serializePriorityQueue(s_MasterPQs[queueNumber], debugInfo, j);
        }
    }
}
