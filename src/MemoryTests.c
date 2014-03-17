/**
 * @file:   MemoryTests.c
 * @brief:  Unit tests for memory request and release.
 */

#include "MemoryTests.h"

#include "Polling/uart_polling.h"
#include "rtx.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* DEBUG_0 */

#define FAIL 0
#define PASS 1

// test processes initialization table
PROC_INIT g_test_procs[NUM_TEST_PROCS];

// store all memory blocks
U32 memoryBlocks[NUM_BLOCKS];

// for testing
extern int usedCount;
extern int availableCount;

int passed[3] = { PASS };

/*
 * Tests:
 * 1) Deplete memory: request all memory blocks
 *        Pass conditions:
 *        - usedCount = NUM_BLOCKS
 *        - availableCount = 0
 *        - request() is blocked
 * 2) Release memory success
 *        Pass conditions:
 *        - all release_memory_block() operations return sucess
 * 3) Release some memory
 *        Pass conditions:
 *        - usedCount != NUM_BLOCKS
 *        - availableCount != 0
 *        - request() is unblocked
 *         
*/

void set_test_procs() {
	int i;
	
	for (i = 0; i < NUM_TEST_PROCS; i++) {
		g_test_procs[i].m_pid = (U32)(i + 1);
		g_test_procs[i].m_stack_size = 0x100;
	}

	g_test_procs[0].m_priority = HIGH;
	g_test_procs[1].m_priority = HIGH;
	g_test_procs[2].m_priority = MEDIUM;
	g_test_procs[3].m_priority = LOW;
	g_test_procs[4].m_priority = LOW;
	g_test_procs[5].m_priority = LOW;
	
	g_test_procs[0].mpf_start_pc = &request;
	g_test_procs[1].mpf_start_pc = &release;
	g_test_procs[2].mpf_start_pc = &results;
	g_test_procs[3].mpf_start_pc = &dummy1;
	g_test_procs[4].mpf_start_pc = &dummy2;
	g_test_procs[5].mpf_start_pc = &dummy3;
    
    for (i = 0; i < 3; i++) {
        passed[i] = PASS;
    }
}

void request(void) {
    int i;
    
    while (1) {
        // assert usedCount = 0 and availableCount = 40
        if (usedCount != 0 || availableCount != NUM_BLOCKS) {
            passed[0] = FAIL;
        }
        
        for (i = 0; i < NUM_BLOCKS + 1; i++) { // request the number of available blocks + 1
            // this will block when i = 40, until a memory block is released
            memoryBlocks[i] = (U32)request_memory_block();
        }
        
        // assert usedCount = 40 and availableCount = 0
        if (usedCount != NUM_BLOCKS || availableCount != 0) {
            passed[0] = FAIL;
        }
        
        // lower priority, effectively inactivates this process
        set_process_priority(PROCESS_1, LOWEST);
    }
}

void release(void) {
    while (1) {
        int i;
        int release = 10; // release 10 memory blocks
        
        // assert usedCount = 40 and availableCount = 0
        if (usedCount != NUM_BLOCKS || availableCount != 0) {
            passed[0] = FAIL;
        }
        
        for (i = 0; i < release; i++) {
            // check success of release operation
            if (release_memory_block((void*)memoryBlocks[i]) == RTX_ERR) {
                passed[1] = FAIL;
            }
        }
        
        // assert usedCount = 31 and availableCount = 9
        if (usedCount != NUM_BLOCKS - release + 1 || availableCount != release - 1) {
            passed[2] = FAIL;
        }
        
        // lower priority, effectively inactivates this process
        set_process_priority(PROCESS_2, LOWEST);
    }
}

void results(void) {
    int completed = 0;
    
    while (1) {
        // print results once request() and release() have complete
        if (!completed && get_process_priority(PROCESS_1) == LOWEST && get_process_priority(PROCESS_2) == LOWEST) {
            if (passed[0] == PASS) {
                uart1_put_string("Request memory: OK");
            } else {
                uart1_put_string("Request memory: FAILED");
            }
            uart1_put_string("\r\n");
            if (passed[1] == PASS) {
                uart1_put_string("Release memory: OK");
            } else {
                uart1_put_string("Release memory: FAILED");
            }
            uart1_put_string("\r\n");
            if (passed[2] == PASS) {
                uart1_put_string("Remaining memory: OK");
            } else {
                uart1_put_string("Remaining memory: FAILED");
            }
            
            completed = 1;
        }
        release_processor();
    }
}

void dummy1(void) {
    while (1) {
        release_processor();
    }
}

void dummy2(void) {
    while (1) {
        release_processor();
    }
}

void dummy3(void) {
    while (1) {
        release_processor();
    }
}

