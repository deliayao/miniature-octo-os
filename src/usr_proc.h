/**
 * @file:   usr_proc.h
 * @brief:  Two user processes header file
 * @author: Yiqing Huang
 * @date:   2014/01/17
 */
 
#ifndef USR_PROC_H_
#define USR_PROC_H

void set_test_procs(void);

void proc1(void);
void proc2(void);
void proc3(void);
void proc4(void);
void proc5(void);
void proc6(void);

#ifdef DEBUG_PERFORMANCE
// user processes for performance testing mode
void requestMemoryBlockPerformance(void);
void sendMessagePerformance(void);
void receiveMessagePerformance(void);
void performanceDummy1(void);
void performanceDummy2(void);
void performanceDummy3(void);
#endif /*DEBUG_PERFORMANCE */

#endif /* USR_PROC_H_ */
