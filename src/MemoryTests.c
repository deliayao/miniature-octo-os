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

/**
 * Test process initialization table items. Initialized with values on a
 * set_test_procs() call.
 */
PROC_INIT g_test_procs[NUM_TEST_PROCS];

// for testing
extern int g_AvailableCount;
extern int g_UsedCount;

/**
 * Array for storing memory blocks during testing.
 */
static U32 s_MemoryBlocks[NUM_BLOCKS];

/**
 * Array for storing test results.
 */
static int s_Passed[3] = { PASS };

/*
 * Tests:
 * 1) Deplete memory: request all memory blocks
 *        Pass conditions:
 *        - g_UsedCount = NUM_BLOCKS
 *        - g_AvailableCount = 0
 *        - request() is blocked
 * 2) Release memory success
 *        Pass conditions:
 *        - all release_memory_block() operations return sucess
 * 3) Release some memory
 *        Pass conditions:
 *        - g_UsedCount != NUM_BLOCKS
 *        - g_AvailableCount != 0
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
        s_Passed[i] = PASS;
    }
}

void request(void) {
    int i;
    
    while (1) {
        // assert g_UsedCount = 0 and g_AvailableCount = 40
        if (g_UsedCount != 0 || g_AvailableCount != NUM_BLOCKS) {
            s_Passed[0] = FAIL;
        }
        
        for (i = 0; i < NUM_BLOCKS + 1; i++) { // request the number of available blocks + 1
            // this will block when i = 40, until a memory block is released
            s_MemoryBlocks[i] = (U32)request_memory_block();
        }
        
        // assert g_UsedCount = 40 and g_AvailableCount = 0
        if (g_UsedCount != NUM_BLOCKS || g_AvailableCount != 0) {
            s_Passed[0] = FAIL;
        }
        
        // lower priority, effectively inactivates this process
        set_process_priority(PROCESS_1, LOWEST);
    }
}

void release(void) {
    while (1) {
        int i;
        int release = 10; // release 10 memory blocks
        
        // assert g_UsedCount = 40 and g_AvailableCount = 0
        if (g_UsedCount != NUM_BLOCKS || g_AvailableCount != 0) {
            s_Passed[0] = FAIL;
        }
        
        for (i = 0; i < release; i++) {
            // check success of release operation
            if (release_memory_block((void*)s_MemoryBlocks[i]) == RTX_ERR) {
                s_Passed[1] = FAIL;
            }
        }
        
        // assert g_UsedCount = 31 and g_AvailableCount = 9
        if (g_UsedCount != NUM_BLOCKS - release + 1 || g_AvailableCount != release - 1) {
            s_Passed[2] = FAIL;
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
            if (s_Passed[0] == PASS) {
                uart1_put_string("Request memory: OK");
            } else {
                uart1_put_string("Request memory: FAILED");
            }
            uart1_put_string("\r\n");
            if (s_Passed[1] == PASS) {
                uart1_put_string("Release memory: OK");
            } else {
                uart1_put_string("Release memory: FAILED");
            }
            uart1_put_string("\r\n");
            if (s_Passed[2] == PASS) {
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
