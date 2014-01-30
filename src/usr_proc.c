/**
 * @file:   usr_proc.c
 * @brief:  six user processes
 * @author: Andrew Gillies, Delia Yao, Joohee Lee, Yiying Ho
 * @date:   2014/01/17
 * NOTE: Each process is in an infinite loop. Processes never terminate.
 */

#include "rtx.h"
#include "Polling/uart_polling.h"
#include "usr_proc.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* DEBUG_0 */

/* --Definition-- */
#define NUM_BLOCK 40

/* initialization table item */
PROC_INIT g_test_procs[NUM_TEST_PROCS];

int test[7] = {0};	//Flag for test cases 1 to 6
int testMode = 1;  //The testMode to indicate which test case is running

void set_test_procs() {
	int i;
	
	for( i = 0; i < NUM_TEST_PROCS; i++ ) {
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
* test cases:
* test1: proc2 should be blocked when it tried to request more memoey after proc1 has all the memory
* test2: proc2 unblocked here after releasing one memory from proc1 
* test3: proc 1 shuld have the lowest proirity 
* test4: since proc2 is blocked then proc3 should be ready to run
* test5: proc4 should get blocked then proc5 should be running
* test6: every processes should be blocked except proc6
*/

/**
 * @brief: a process that requests all the memory blocks 
 *         and then yields the cpu.
 *         It release one memory block
 *         and then yields the cpu.
 */


void proc1(void) {
	int i;
	void* mem_ptr;
	
	uart0_put_string("\n\r");
	uart0_put_string("G006_test: START\n\r");
	uart0_put_string("G006_test: total 6 tests\n\r");
		while(1){
		for(i = 0; i < NUM_BLOCK; i ++) {
			mem_ptr = request_memory_block();
		}	
		release_processor();

		if(testMode == 1){
			if(test[1] != -1000 ){
				uart0_put_string("G006_test: test 1 OK\n\r");
				test[1] = 0;	//test1 PASS
			}else{
				uart0_put_string("G006_test: test 1 FAIL\n\r");
			}
			test[2]++;	//test2 starts and it increases to 1
			testMode = 2;
		}

		release_memory_block(mem_ptr);
		set_process_priority(1,3);
		
		release_processor();
		
		if(testMode == 4){
			test[4] = -1000;	//test4 FAIL
		}
	}
}


/**
 * @brief: a process that requests all the memory blocks 
 *         and then yields the cpu.
 */

void proc2(void){
	void* mem_ptr;
	while(1){

		if(testMode == 2){
			test[2] = -1000;	//test2 FAIL
		}

		//request for one memory blcok
		mem_ptr = request_memory_block();

		if(testMode == 1){
			test[1] = -1000;	//test1 FAIL
		}
		
		//print result of test2: proc2 unblocked here after releasing all the memory from proc1
		if(testMode == 2){
			if(test[2] != -1000){
				uart0_put_string("G006_test: test 2 OK\n\r");
				test[2] = 0;	//test2 PASS
			}else{
				uart0_put_string("G006_test: test 2 FAIL\n\r");
			}
			test[3]++;	//test3 starts
			testMode = 3;
		}	
		
		release_processor();
	}
}


/**
 * @brief: a process that requests NUM_BLOCK - 5  memory blocks 
 *         and then yields the cpu.
 *         It release one memory block
 *         and then yields the cpu.
 */

void proc3(void){
	int array_size = NUM_BLOCK - 5;
	void* mem_ptr;
	int i;
	while(1){
		
		//test3: proc 1 has lowest proirity
		if(testMode == 3){
			if(get_process_priority(1) == 3){
				uart0_put_string("G006_test: test 3 OK\n\r");
				test[3] = 0;	//test3 PASS
			}else{
				uart0_put_string("G006_test: test 3 FAIL\n\r");
				test[3] = -1000;	//test3 FAIL
			}
			test[4]++;	//test3 starts
			testMode = 4;
		}
		
		//test4: the next priority proc3 is ready to run
		if(testMode == 4){
			if(test[4] != -1000){
				uart0_put_string("G006_test: test 4 OK\n\r");
				test[4] = 0;	//test4 PASS
			}else{
				uart0_put_string("G006_test: test 4 FAIL\n\r");
				test[4] = -1000;	//test4 FAIL
			}
			test[5]++;	//test5 starts
			testMode = 5;
		}
		
		for(i = 0; i < array_size; i++){
			mem_ptr = request_memory_block();
		}

		release_processor();
				release_memory_block(mem_ptr);

		release_processor();
	}
}


/**
 * @brief: a process that counts from 0 to 1000 
 *         and then yields the cpu.
 *         It request one memory block
 *         and then yields the cpu.
 */

void proc4(void){
	int i = 0;
	void* mem_ptr;
  	while (1) {
		
		//delay 1000
		while(i < 1000){
			i++;
		}
		
		release_processor();
		
		if(testMode == 5){
			test[5] = 2;
		}
		
		mem_ptr = request_memory_block();

		if(testMode == 5){
			test[5] = -1000;
		}
		
		release_processor();	
		
	}
}


/**
 * @brief: a process that requests 10 memory blocks 
 *         and then yields the cpu.
 *         It release 10 memory blocks
 *         and then yields the cpu.
 */

void proc5(void){
	int i;
	void* mem_ptr[10];
	while(1) {

		//test5: proc4 gets blocked so that the next priority proc5 runs
		if(testMode == 5){
			if(test[5] == 2){
				uart0_put_string("G006_test: test 5 OK\n\r");
				test[5] = 0;	//test5 PASS
			}else{
				uart0_put_string("G006_test: test 5 FAIL\n\r");
			}
			test[6]++;	//test6 starts
			testMode = 6;
		}

		for(i = 0; i < 10; i ++) {
			//request 10 memory blocks
			mem_ptr[i] = request_memory_block();
		}

		if(testMode == 6){
			test[6] = -1000;	//test6 FAIL
		}

		release_processor();
		
		for(i = 0; i < 10; i ++) {
			if(mem_ptr[i] != NULL) {
				release_memory_block(mem_ptr[i]);
			}
		}	 
	
		release_processor();
	}
}


/**
 * @brief: a process that only prints the result of test in the end
 */

void proc6(void){
	int fail = 0;
	int pass;
	int i;
	
	while(1) {
		if(testMode == 6){
			if(test[6] != -1000){
				uart0_put_string("G006_test: test 6 OK\n\r");
				test[6] = 0;	//test6 PASS
			}else{
				uart0_put_string("G006_test: test 6 FAIL\n\r");
			}
			
			for(i = 1; i < 7 ; ++i){
				if(test[i] != 0){
					fail++;
				}
			}
			pass = 6 - fail;
			
			uart0_put_string("G006_test: ");
			uart0_put_char('0' + pass);
			uart0_put_string("/6 tests OK\n\r");
			
			uart0_put_string("G006_test: ");
			uart0_put_char('0' + fail);
			uart0_put_string("/6 tests FAIL\n\r");
			
			uart0_put_string("G006_test: END\n\r");
			testMode = 0;  // no more testing	
		}

		release_processor();
	}
}
