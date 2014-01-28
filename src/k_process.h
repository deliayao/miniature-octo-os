/**
 * @file:   k_process.h
 * @brief:  process management header file
 * NOTE: Assuming there are only two user processes in the system
 */

#ifndef K_PROCESS_H_
#define K_PROCESS_H_

#include "Utilities/Types.h"
#include "Utilities/k_rtx.h"


/* ----- Functions ----- */

void process_init(void);               /* initialize all procs in the system */
PCB *scheduler(void);                  /* pick the pid of the next to run process */
int k_release_processor(void);           /* kernel release_process function */
int k_set_process_priority(int process_id, int priority); // sets process priority
int k_get_process_priority(int process_id); // gets process priority

int handleMemoryRelease(void); // handles a release memory block event

extern U32 *alloc_stack(U32 size_b);   /* allocate stack for a process */
extern void __rte(void);               /* pop exception stack frame */
extern void set_test_procs(void);      /* test process initial set up */
extern void initializeNullProcess(void); // null process initialization

#endif /* ! K_PROCESS_H_ */
