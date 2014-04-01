/** 
 * @file:   k_rtx.h
 * @brief:  Kernel process initialization data structure.
 */
 
#ifndef K_RTX_H_
#define K_RTX_H_

/**
 * Process table initialization item.
 */
typedef struct proc_init {
    int m_pid; // process ID
    int m_priority; // process priority
    int m_stack_size; // size of process stack in words
    void (*mpf_start_pc) (); // process entry point  
} PROC_INIT;

#endif // ! K_RTX_H_
