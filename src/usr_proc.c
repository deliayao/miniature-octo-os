/**
 * @file:   usr_proc.c
 * @brief:  Two user processes: proc1 and proc2
 * @author: Yiqing Huang
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
/* initialization null process*/

int free_mem_blk = NUM_BLOCK;
int test[7] = {0};
int testMode = 1;

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
 * @brief: a process that requests 10 memory blocks 
 *         and then yields the cpu.
 *         It release all the memory blocks that it has
 *         and then yields the cpu.
 */
void proc1(void) {
	int i;
	void* mem_ptr[NUM_BLOCK];
	
	uart0_put_string("\n\r");
	uart0_put_string("G006_test: START\n\r");
	uart0_put_string("G006_test: total 6 tests\n\r");
		while(1){
		uart0_put_string("process 1 starts~\n\r");
		//start requesting 10 memory blocks for proc1 
		for(i = 0; i < NUM_BLOCK; i ++) {
			mem_ptr[i] = request_memory_block();
			free_mem_blk--;
			uart0_put_string("proc 1 requesting memory\n\r");
			uart0_put_char('0'+i);
			uart0_put_string("\n\r");
		}
		
		//release processor to run the next process
		release_processor();
		
		//print result of test1: proc2 blocked so run proc1 
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
		
		//release all the memory blocks that proc1 has
		for(i = 0; i < NUM_BLOCK; i ++) {
			//before releasing memory block, check if the pointer is not NULL
			if(mem_ptr[i] != NULL){
				release_memory_block(mem_ptr[i]);
				free_mem_blk++;
			uart0_put_string("proc 1 releasing memory\n\r");
			}
		}
		//release processor to run the next process
		release_processor();
		
		//proc1 becomes lowest
		set_process_priority(1, 3);
		if(testMode == 4){
			test[4] = -1000;	//test4 FAIL
		}
	}
}

/**
 * @brief: a process that requests 1 memory block 
 *         and then yields the cpu.
 */
void proc2(void){
	void* mem_ptr;
	while(1){
				uart0_put_string("proc 2 starts~\n\r");
		
		if(testMode == 2){
			test[2] = -1000;	//test1 FAIL
		}
		//request for one memory blcok
		mem_ptr = request_memory_block();
		free_mem_blk--;
		
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
		
		//if proc2 gets blocked the strings below won't be printed
		uart0_put_string("proc 2 requesting memory\n\r");
		
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
#ifdef DEBUG_0 	
printf("set_priority for proc1: %d\n", get_process_priority(1));
#endif /* ! DEBUG_0 */
		release_processor();
	}
}


void proc3(void){
	int array_size = NUM_BLOCK - 5;
	void* mem_ptr[NUM_BLOCK - 5];
	int i;
	while(1){
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

			mem_ptr[i] = request_memory_block();
			free_mem_blk--;
			uart0_put_string("proc 3 requesting memory\n\r");
			uart0_put_char('0'+i);
			uart0_put_string("\n");
			
		}
		release_processor();
				uart0_put_string("proc 3 starts2~\n\r");
		for(i = 0; i < array_size; i++){

			if(mem_ptr[i] != NULL){
				release_memory_block(mem_ptr[i]);
				free_mem_blk++;
			uart0_put_string("proc 3 releasing memory\n\r");
			uart0_put_char('0'+i);
			uart0_put_string("\n\r");
			}
		}
		release_processor();
	}
}

/**
 * @brief: a process that only prints five numbers
 *         and then yields the cpu.
 */
void proc4(void){
	int i = 0;
	void* mem_ptr;
  while (1) {
		//set_process_priority(1, 0);
				uart0_put_string("proc 4 starts~\n\r");
		
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
 * @brief: a process that requests 8 memory blocks 
 *         and then yields the cpu.
 *         It release all the memory blocks that it has
 *         and then yields the cpu.
 */
void proc5(void){
	int i;
	void* mem_ptr[8];
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
		uart0_put_string("proc 5 starts~\n\r");
		for(i = 0; i < 10; i ++) {
			//request 10 memory blocks
			mem_ptr[i] = request_memory_block();
			free_mem_blk--;
			uart0_put_string("proc 5 requesting memory\n\r");
			uart0_put_char('0'+i);
			uart0_put_string("\n");
		}
		if(testMode == 6){
			test[6] = -1000;
		}
		release_processor();
		
				uart0_put_string("proc 5 start2~\n\r");
	
		for(i = 0; i < 8; i ++) {
			// release all memory blocks that proc 5 has
			if(mem_ptr[i] != NULL) {
				release_memory_block(mem_ptr[i]);
				free_mem_blk++;
			uart0_put_string("proc 5 requesting memory\n\r");
			uart0_put_char('0'+i);
			uart0_put_string("\n\r");
			}
		}	 
	
		release_processor();
	}
}

/**
 * @brief: a process that requests 2 memory blocks 
 *         and then yields the cpu.
 *         It release only one memory block
 *         and then yields the cpu.
 */
void proc6(void){
//	void* mem_ptr_one;
//	void* mem_ptr_two;
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
			uart0_put_char('0' + pass);
			uart0_put_string("G006_test: ");
			uart0_put_char('0' + fail);
			uart0_put_string("/6 tests FAIL\n\r");
			uart0_put_string("G006_test: END\n\r");
			testMode = 0;  // no more testing	
		}
		
		
		
		
		// request 2 memory blocks
	/*	uart0_put_string("proc 6 starts~\n\r");
		mem_ptr_one = request_memory_block();
		free_mem_blk--;
		mem_ptr_two = request_memory_block();
		free_mem_blk--;
		uart0_put_string("proc 6 requesting memory\n\r");
		release_processor();
	
		release_memory_block(mem_ptr_one);
		free_mem_blk++;
		
		uart0_put_string("proc 6 requesting memory\n\r");*/
		release_processor();
	}
}

