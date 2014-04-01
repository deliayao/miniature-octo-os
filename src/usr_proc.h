/**
 * @file:   usr_proc.h
 * @brief:  Six user processes header file
 * @author: Yiqing Huang
 * @date:   2014/01/17
 */
 
#ifndef USR_PROC_H_
#define USR_PROC_H

/*
 * Sets user test processes.
 */
void set_test_procs(void);

void proc1(void);
void proc2(void);
void proc3(void);
void proc4(void);
void proc5(void);
void proc6(void);

// user processes for performance testing mode
#ifdef DEBUG_PERFORMANCE
/*
 * Tests and times request memory block performance.
 */
void requestMemoryBlockPerformance(void);

/*
 * Tests and times send message performance.
 */
void sendMessagePerformance(void);

/*
 * Tests and times receive message performance.
 */
void receiveMessagePerformance(void);

/*
 * Filler process 1 (system must have 6 test processes).
 */
void performanceDummy1(void);

/*
 * Filler process 2 (system must have 6 test processes).
 */
void performanceDummy2(void);

/*
 * Filler process 3 (system must have 6 test processes).
 */
void performanceDummy3(void);

#endif /*DEBUG_PERFORMANCE */

#endif /* USR_PROC_H_ */
