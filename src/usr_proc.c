/**
 * @file:   usr_proc.c
 * @brief:  six user processes
 * @author: Andrew Gillies, Delia Yao, Joohee Lee, Yiying Ho
 * @date:   2014/01/17
 * NOTE: Each process is in an infinite loop. Processes never terminate.
 */

#include "usr_proc.h"

#include "rtx.h"
#include "Polling/uart_polling.h"
#include "Utilities/String.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* ! DEBUG_0 */

/* --Definition-- */
#define NUM_BLOCK 40

/* initialization table item */
PROC_INIT g_test_procs[NUM_TEST_PROCS];

int test[7] = {0}; // Flag for test cases 1 to 6
int testMode = 1; // The testMode to indicate which test case is running. 1 is running and 0 is not

void set_test_procs() {
	int i;
	
	for (i = 0; i < NUM_TEST_PROCS; i++) {
		g_test_procs[i].m_pid = (U32)(i+1);
		g_test_procs[i].m_stack_size = 0x100;
	}

	g_test_procs[0].m_priority = HIGH;
	g_test_procs[1].m_priority = HIGH;
	g_test_procs[2].m_priority = MEDIUM;
	g_test_procs[3].m_priority = MEDIUM;
	g_test_procs[4].m_priority = LOW;
	g_test_procs[5].m_priority = LOW;
	
 
	g_test_procs[0].mpf_start_pc = &proc1;
	g_test_procs[1].mpf_start_pc = &proc2;
	g_test_procs[2].mpf_start_pc = &proc3;
	g_test_procs[3].mpf_start_pc = &proc4;
	g_test_procs[4].mpf_start_pc = &proc5;
	g_test_procs[5].mpf_start_pc = &proc6;
}

/**
* test cases (RTX I):
* test1: proc2 should be blocked when it tried to request more memoey after proc1 has all the memory
* test2: proc2 unblocked here after releasing one memory from proc1 
* test3: proc 1 shuld have the lowest proirity 
* test4: since proc2 is blocked then proc3 should be ready to run
* test5: proc4 should get blocked then proc5 should be running
* test6: every processes should be blocked except proc6
*/

/**
* test cases (RTX II):
* test1: Tests BLOCKED_RECEIVE.
* test2: Tests receive_message() sender and content.
* test3: Tests delayed_send().
* test4: Tests release_memory_block().
* test5: Tests context switching.
* test6: Tests get_process_priority() and set_process_priority() preemption. 
*/

/**
* test cases (RTX III):
* test1: Tests BLOCKED_RECEIVE.
* test2: Tests receive_message() sender and content.
* test3: Tests delayed_send().
* test4: Tests release_memory_block().
* test5: Tests context switching.
* test6: Tests get_process_priority() and set_process_priority() preemption by sending a message to PROCESS_SET_PRIORITY.  
*/

/**
 * @brief: Process waits for a message from Process 5, then sets its priority to LOWEST causing a preemption.
 */
void proc1(void) {
	Letter* received;
    
	while (1) {
		if (testMode) {
			test[1]++; // test[1] is 1
		}
		received = (Letter*)receive_message(NULL); // this will block until Process 5 sends a message
        
        // change the priority by sending a message to the set process priority process
        // reuse the message envelope that we just received
        strcpy("%C 1 4", received->m_Text);
		send_message(PROCESS_SET_PRIORITY, (void*)received); // this will preempt, Process 6 will always have a higher priority
		
        // if the process reaches this point, it has failed
		if (testMode) {
			test[6] = -1000;
		}
	}
}


/**
 * @brief: Process registers a command with the KCD, then waits for a message from Process 3.
 *         Upon receive, we do some message verification, then set this process' priority to LOWESST.
 */
void proc2(void) {
	Letter* command;
	Letter* received;
	int sender;
    
	while (1) {
		command = (Letter*)request_memory_block();
		command->m_Type = KCD_REG;
		command->m_Text[0] = '%';
		command->m_Text[1] = 'V';
		command->m_Text[2] = 'I';
		command->m_Text[3] = '\0';
		
		if (testMode) {
				test[2]++; // test[2] is 1
		}
        
		send_message(KCD_PROCESS, (void*)command); // register command
		release_processor();
		
        // this will block until Process 3 sends a message
		received = (Letter*)receive_message(&sender);
		
        // received a message
        // assert: sender is PROCESS_3
        //         message content is "Hi"
		if (testMode) {
			if (sender != PROCESS_3) { // verify sender
				test[2] = -1000; // test[2] fails
			} else if (received->m_Text[0] != 'H' || received->m_Text[1] != 'i' && received->m_Text[2] == '\0') { // verify content
                test[2] = -1000; // test[2] fails
			} else {
				test[2]++; // test[2] is 1	
			}
		}
        
        if (release_memory_block((void*)received) == RTX_ERR) {
            test[4] = -1000; // if this operation fails, the test fails
        } else {
            test[4]++;
        }
        
		command = (Letter*)request_memory_block();
		command->m_Type = DEFAULT;
        strcpy("%C 2 4", command->m_Text);
        
        send_message(PROCESS_SET_PRIORITY, (void*)command); // set Process 2 to LOWEST priority (should never run again)

		release_processor();
	}
}


/**
 * @brief: Process sends a message to Process 2, then receives a delayed message from Process 4.
 *         Upon receive, we do some message verification, then set this process' priority to LOWESST.
 */
void proc3(void) {
	Letter* message;
	Letter* received;
	int sender;
		
	while (1) {
        // write a message to Process 2
		message = (Letter*)request_memory_block();		
		message->m_Type = DEFAULT;
		message->m_Text[0] = 'H';
		message->m_Text[1] = 'i';
		message->m_Text[2] = '\0';
		
		if (testMode) {
            test[3]++; // test[3] is 2
        }
        
		send_message(PROCESS_2, (void*)message); // this will preempt
        
        // Once this Process 3 is preempted, Process 2 will run.
        // Process 3 gets added to the back of the Ready Queue.
        // Process 4 will run before Process 3 is rescheduled,
        // so there will already be a message waiting in Process 3's mailbox.
        
        // (Process 4 delay-sends and busy waits for 3 seconds)
		
        // i.e., this will not block
		// get a message from Process 4
		received = (Letter*)receive_message(&sender);
        
		test[3] = 100;
        
        // received a message
        // assert: sender is PROCESS_4
		if (testMode) {
			if (sender != PROCESS_4) { // verify sender
				test[3] = -1000; // test[3] fails
			}
		}
		
        // change the priority by sending a message to the set process priority process
        // reuse the message envelope that we just received
        strcpy("%C 3 4", received->m_Text);
		send_message(PROCESS_SET_PRIORITY, (void*)received); // set Process 3 to LOWEST priority (should never run again)
        
		release_processor();
	}
}


/**
 * @brief: Process sends a delayed message to Process 3, then busy waits until it is sent.
 *         Then sends the same message to Process 5 (this should be invalid and release_memory_block() should return an error).
 *         Then process tries to receive a message and blocks indefinitely.
 */
void proc4(void) {
	Letter* message;
	Letter* received;
	int i, j;
    
	while (1) {
        // write a message to Process 3
		message = (Letter*)request_memory_block();
		message->m_Type = DEFAULT;
        
		for (i = 0; i < 26; i ++) {
			message->m_Text[i] = 65 + i;
		}

		if (testMode) {
            test[5]++; // test[5] is 1
        }
        
        for (i = 0; i < 3; i++) {
            // count from i = 0 to 3
		}

        // delay for 3 seconds
		delayed_send(PROCESS_3, (void *)message, 3000);

        // after 3 seconds, this process will be preempted and Process 3 will be scheduled
        // at which time test[3] will be set to 100
        // poll test[3] until we are preempted
		j = 0;
		while (test[3] != 100) { // if test[3] didn't fail in proc3 then this loop is terminated when it comes back to proc4
			j++;
			if (test[3] == -1000) {
                break; // test[3] fails
            }
		}
		
		if (testMode == 1 && i == 3) {
			test[5]++;
		} else {
			test[5] = -1000;
		}
        
        message = (Letter*)request_memory_block();
		delayed_send(PROCESS_5, (void *)message, 1000);
        
        // this will block
		received = (Letter*)receive_message(NULL);
        release_memory_block((void*)received); // this line should never execute
	}
}


/**
 * @brief: Process receives a message from Process 4
 *         and asserts Process 1's priority is HIGH before forwarding the message to Process 1.
 *         Then asserts Process 1's priority has changed to LOWEST.
 *         Then tries to receive a message and blocks indefinitely.
 */
void proc5(void) {
	Letter* received;
    
	while (1) {
        // receive a message from Process 4
		received = (Letter*)receive_message(NULL);
        
        if (get_process_priority(PROCESS_1) != HIGH) { // Process 1 should have been blocked this entire time
                                                       // and not been able to change its priority
            test[1] = -1000;
        }
		
        // forward the message to Process 1
        send_message(PROCESS_1, (void*)received); // this will preempt
        
        if (get_process_priority(PROCESS_1) != LOWEST) {
            test[6] = -1000;
        } else {
            test[6]++; // if Process 1's priority was not HIGH at the beginning, test[6] will not be > 0
        }
		
        // this will block
		received = (Letter*)receive_message(NULL);
        
        // process should never reach this line
		test[1] = -1000;
	}
}


/**
 * @brief: Process prints test results then sets test flag to 0 (no more testing).
 */
void proc6(void) {
	int fail = 0;
	int pass;
	int i;
	
	while (1) { 
		while (testMode) {
			i++;
			if (test[6] != 0) {
				break; // break if Process 5 completed all of its tests
			}
			release_processor();
		}
		
        // print test results
		if (testMode) {
			uart1_put_string("\n\r");
			uart1_put_string("G006_test: START\n\r");
			uart1_put_string("G006_test: total 6 tests\n\r");
			
			for (i = 1; i < 7 ; ++i) {
				if (test[i] <= 0) {
					fail++;
					uart1_put_string("G006_test: test");
					uart1_put_char('0' + i);
					uart1_put_string(" FAIL\n\r");
				} else {
					uart1_put_string("G006_test: test");
					uart1_put_char('0' + i);
					uart1_put_string(" OK\n\r");
				}
			}
		
			pass = 6 - fail;
			uart1_put_string("G006_test: ");
			uart1_put_char('0' + pass);
			uart1_put_string("/6 tests OK\n\r");
		
			uart1_put_string("G006_test: ");
			uart1_put_char('0' + fail);
			uart1_put_string("/6 tests FAIL\n\r");
		
			uart1_put_string("G006_test: END\n\r");
			
			testMode = 0; // no more testing	
		}
			
		release_processor();
	}
}
