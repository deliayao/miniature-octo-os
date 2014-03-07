/**
 * @file:   rtx.h
 * @brief: 	User API
 */

#ifndef RTX_H_
#define RTX_H_

/* ----- Definitions ----- */
#define RTX_ERR -1
#define NULL 0
#define NUM_TEST_PROCS 6

/* Process Priority. The bigger the number is, the lower the priority is*/
#define PRIVILEGED       0
#define HIGH             1
#define MEDIUM           2
#define LOW              3
#define LOWEST           4
#define NULL_PRIORITY    5

// Process IDs
#define NULL_PROCESS 					0
#define PROCESS_1   					1
#define PROCESS_2   					2
#define PROCESS_3   					3
#define PROCESS_4   					4
#define PROCESS_5   					5
#define PROCESS_6   					6
#define PROCESS_A    					7
#define PROCESS_B    					8
#define PROCESS_C    					9
#define PROCESS_SET_PRIORITY 	10
#define CLOCK_PROCESS        	11
#define KCD_PROCESS          	7
#define CRT_PROCESS          	8
#define TIMER_IPROCESS  			9
#define UART_IPROCESS   			10

// Message Types
#define DEFAULT 0
#define KCD_REG 1

// UART flags
#define IER_RBR		0x01
#define IER_THRE	0x02
#define IER_RLS		0x04

#define IIR_PEND	0x01
#define IIR_RLS		0x03
#define IIR_RDA		0x02
#define IIR_CTI		0x06
#define IIR_THRE	0x01

#define LSR_RDR		0x01
#define LSR_OE		0x02
#define LSR_PE		0x04
#define LSR_FE		0x08
#define LSR_BI		0x10
#define LSR_THRE	0x20
#define LSR_TEMT	0x40
#define LSR_RXFE	0x80

#define BUFSIZE		0x40

/* ----- Types ----- */
typedef unsigned int U32;

/* initialization table item */
typedef struct proc_init
{
	int m_pid;	        /* process id */
	int m_priority;         /* initial priority, not used in this example. */
	int m_stack_size;       /* size of stack in words */
	void (*mpf_start_pc) ();/* entry point of the process */
} PROC_INIT;

// For IPC
typedef struct Letter
{
    int m_Type; // message type
    char m_Text; // message body
} Letter;

/* ----- RTX User API ----- */
#define __SVC_0  __svc_indirect(0)

extern void k_rtx_init(void);
#define rtx_init() _rtx_init((U32)k_rtx_init)
extern void __SVC_0 _rtx_init(U32 p_func);

extern int k_release_processor(void);
#define release_processor() _release_processor((U32)k_release_processor)
extern int __SVC_0 _release_processor(U32 p_func);

// Memory Management
extern void *k_request_memory_block(void);
#define request_memory_block() _request_memory_block((U32)k_request_memory_block)
extern void *_request_memory_block(U32 p_func) __SVC_0;
/* __SVC_0 can also be put at the end of the function declaration */

extern int k_release_memory_block(void *);
#define release_memory_block(p_mem_blk) _release_memory_block((U32)k_release_memory_block, p_mem_blk)
extern int _release_memory_block(U32 p_func, void *p_mem_blk) __SVC_0;

// Process Management
extern int k_set_process_priority(int process_id, int priority);
#define set_process_priority(process_id, priority) _set_process_priority((U32)k_set_process_priority, process_id, priority)
extern int _set_process_priority(U32 p_func, int process_id, int priority) __SVC_0;

extern int k_get_process_priority(int process_id);
#define get_process_priority(process_id) _get_process_priority((U32)k_get_process_priority, process_id)
extern int _get_process_priority(U32 p_func, int process_id) __SVC_0;

// IPC Management
extern void *k_receive_message(int *sender_id);
#define receive_message(sender_id) _receive_message((U32)k_receive_message, sender_id)
extern void *_receive_message(U32 p_func, int *sender_id) __SVC_0;

extern int k_delayed_send(int process_id, void *message_envelope, int delay);
#define delayed_send(process_id, message_envelope, delay) _delayed_send((U32)k_delayed_send, process_id, message_envelope, delay)
extern int _delayed_send(U32 p_func, int process_id, void *message_envelope, int delay) __SVC_0;

extern int k_send_message(int process_id, void *message_envelope);
#define send_message(process_id, message_envelope) _send_message((U32)k_send_message, process_id, message_envelope)
extern int _send_message(U32 p_func, int process_id, void *message_envelope) __SVC_0;

#endif /* !RTX_H_ */
