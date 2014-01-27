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
#define ARRAY_SIZE 10

/* initialization table item */
PROC_INIT g_test_procs[NUM_TEST_PROCS];

int test[6] = {0};

void set_test_procs() {
	int i;

	
	g_test_procs[0].m_pid = (U32) 0;
	g_test_procs[0].m_priority = LOWEST;
	g_test_procs[0].m_stack_size = 0x100;
	
	for( i = 1; i < NUM_TEST_PROCS; i++ ) {
		g_test_procs[i].m_pid = (U32)(i+1);
		g_test_procs[i].m_stack_size = 0x100;
	}
	
	g_test_procs[1].m_priority = HIGH;
	g_test_procs[2].m_priority = HIGH;
	g_test_procs[3].m_priority = MEDIUM;
	g_test_procs[4].m_priority = LOWEST;
	g_test_procs[5].m_priority = MEDIUM;
	g_test_procs[6].m_priority = LOW;
	
	g_test_procs[0].mpf_start_pc = &null_process;
 
	g_test_procs[1].mpf_start_pc = &proc1;
	g_test_procs[2].mpf_start_pc = &proc2;
	g_test_procs[3].mpf_start_pc = &proc3;
	g_test_procs[4].mpf_start_pc = &proc4;
	g_test_procs[5].mpf_start_pc = &proc5;
	g_test_procs[6].mpf_start_pc = &proc6;
}

void null_process() {
	while(1){
		/*uart0_put_string("null process");
		uart0_put_string("\n");*/
		release_processor();
	}
}

/**
 * @brief: a process that requests 10 memory blocks 
 *         and then yields the cpu.
 *         It release all the memory blocks that it has
 *         and then yields the cpu.
 */
void proc1(void) {
	int i;
	void* mem_ptr[10];
	uart0_put_string("\n\r");
	uart0_put_string("G006_test: START\n\r");
	uart0_put_string("G006_test: total 6 tests\n\r");
	while(1){
//		uart0_put_string("process 1 starts~\n");
		//start requesting 10 memory blocks for proc1 
		for(i = 0; i <= 9; i ++) {
			mem_ptr[i] = request_memory_block();
		//  uart0_put_string("proc 1 requesting memory ");
		//	uart0_put_char('0'+i);
		//	uart0_put_string("\n");
		}
		
		if(sizeof(mem_ptr) == 10){
			test[0] = 1;
		}
		release_processor();
		//release processor to run the next process
		if(test[0] == 2){
			uart0_put_string("\n\r");
			uart0_put_string("G006_test: test 1 OK\n\r");
		}else{
			uart0_put_string("\n\r");
			uart0_put_string("G006_test: test 1 FAIL\n\r");
		}
		
		//release all the memory blocks that proc1 has
		for(i = 0; i < 10; i ++) {
			//before releasing memory block, check if the pointer is not NULL
			if(mem_ptr[i] != NULL){
				release_memory_block(mem_ptr[i]);
			uart0_put_string("proc 1 releasing memory ");
			uart0_put_char('0'+i);
			uart0_put_string("\n");
			}
		}
		uart0_put_string("process 1 done~\n");
		//release processor to run the next process
		release_processor();
	}
}

/**
 * @brief: a process that requests 1 memory block 
 *         and then yields the cpu.
 */
void proc2(void){
	void* mem_ptr;
	while(1){
		test[0]++;
		//request for one memory blcok
		mem_ptr = request_memory_block();
		test[0]--;
		//if proc2 gets blocked the strings below won't be printed
		uart0_put_string("proc 2 requesting memory ");
		uart0_put_string("\n");
		
		//release processor to run the next process
		release_processor();
	}
}

/**
 * @brief: a process that requests 5 memory blocks 
 *         and then yields the cpu.
 *         It release all the memory blocks that it has
 *         and then yields the cpu.
 */
void proc3(void){
	void* mem_ptr[5];
	int i;
	while(1){
					uart0_put_string("proc 3 starts~\n");
		for(i = 0; i < 5; i++){
			//request 5 memory blcoks 
			mem_ptr[i] = request_memory_block();
			uart0_put_string("proc 3 requesting memory ");
			uart0_put_char('0'+i);
			uart0_put_string("\n");
			
		}
		release_processor();
		
		for(i = 0; i < 5; i++){
			// release all the memory blocks that proc3 has 
			if(mem_ptr[i] != NULL){
				release_memory_block(mem_ptr[i]);
			uart0_put_string("proc 3 releasing memory ");
			uart0_put_char('0'+i);
			uart0_put_string("\n");
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
  void* ret_val;
  while (1) {
		ret_val = request_memory_block();
		release_processor();
		release_memory_block(ret_val);
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
					uart0_put_string("proc 5 starts~\n");
		for(i = 0; i < 10; i ++) {
			//request 10 memory blocks
			mem_ptr[i] = request_memory_block();
			uart0_put_string("proc 5 requesting memory ");
			uart0_put_char('0'+i);
			uart0_put_string("\n");
		}
		release_processor();
	
		for(i = 0; i < 8; i ++) {
			// release all memory blocks that proc 5 has
			if(mem_ptr[i] != NULL) {
				release_memory_block(mem_ptr[i]);
			uart0_put_string("proc 5 requesting memory ");
			uart0_put_char('0'+i);
			uart0_put_string("\n");
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
	void* mem_ptr_one;
	void* mem_ptr_two;
	while(1) {
		// request 2 memory blocks
			uart0_put_string("proc 6 starts~\n");
		if (mem_ptr_one == NULL && mem_ptr_two == NULL) {
			mem_ptr_one = request_memory_block();
			mem_ptr_two = request_memory_block();
		}
		else if (mem_ptr_one == NULL) {
			mem_ptr_one = request_memory_block();
		}
		else {
			mem_ptr_two = request_memory_block();
		}
		uart0_put_string("proc 6 requesting memory ");
		uart0_put_string("\n");
		release_processor();
		if (mem_ptr_one != NULL) {
			// it only release one memory
			release_memory_block(mem_ptr_one);
		}
		else {
			release_memory_block(mem_ptr_two);
		}
		uart0_put_string("proc 6 requesting memory ");
		uart0_put_string("\n");
		release_processor();
	}
}


