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

int test[7] = {0};	//test cases 0 to 6 if it goes to negative then it means TEST FAIL 
int testMode = 1;		// to print the result only once in infinite while loop
void set_test_procs() {
        int i;
        // initialization null process        
        g_test_procs[0].m_pid = (U32) 0;
        g_test_procs[0].m_priority = LOWEST;
        g_test_procs[0].m_stack_size = 0x100;
        
        // assign id and stack size for 6 test processes
        for( i = 1; i < NUM_TEST_PROCS; i++ ) {
                g_test_procs[i].m_pid = (U32)(i+1);
                g_test_procs[i].m_stack_size = 0x100;
        }
        
        // assign priority of each test process
        g_test_procs[1].m_priority = HIGH;
        g_test_procs[2].m_priority = HIGH;
        g_test_procs[3].m_priority = LOWEST;
        g_test_procs[4].m_priority = LOWEST;
        g_test_procs[5].m_priority = LOWEST;
        g_test_procs[6].m_priority = LOWEST;
        

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
		release_processor();
	}
}

/**
 * @brief: a process that requests 5 memory blocks 
 *         and then yields the cpu.
 *         It release 5 memory blocks
 *         and then yields the cpu.
 *         a process requests 3 memory blcoks
 *         and then yields the cpu.
 *         a process release 2 memory blocks
 *         and then yileds the cpu.
 *         a process release 1 memory block
 *         and then yileds the cpu.
 */
void proc1(void) {
	void* mem_ptr[8];
	int i;
	uart0_put_string("\n\r");
	uart0_put_string("G006_test: START\n\r");
	uart0_put_string("G006_test: total 6 tests\n\r");
	
	while(1){

		// requests 5 memory blocks
		for(i = 0; i < 5; i++){
			mem_ptr[i] = request_memory_block();
		}
		
		if(testMode == 1){
			test[1]++;	//test1 starts
		}

		if(testMode == 5){
			test[5]++;	//test5 is 2
		}
		
		release_processor();
		
		for(i = 0; i < 5; i++){
			release_memory_block(mem_ptr[i]);
		}

		if(testMode == 1){
			if(test[1] == 3){
				uart0_put_string("G006_test: test 1 OK\n\r");
				test[1] = 0;	//test1 PASS
			}else{
				uart0_put_string("G006_test: test 1 FAIL\n\r");
				test[1] = -1000;	//test1 FAIL
			}
			test[2]++;	//test2 starts and it increases to 1
			testMode = 2;
		}

		if(testMode == 5){
			if(test[5] == 2){
				uart0_put_string("G006_test: test 5 OK\n\r");
				test[5] = 0;
			}else{
				uart0_put_string("G006_test: test 5 FAIL\n\r");
				test[5] = -1000;
			}
			test[6]++;	//test6 starts
		}

		release_processor();
		
		for(i = 5; i < 8; i++){
			mem_ptr[i] = request_memory_block();
		}
		
		//2 free memory blocks left
		if(testMode == 2){
			test[2]++;	//test2 is 4
		}
		
		if(testMode == 6){
			test[6]++;	//test6 is 3
		}

		release_processor();

		// releases 2 memory blocks 
		release_memory_block(mem_ptr[5]);
		release_memory_block(mem_ptr[6]);
		
		if(testMode == 3){
			if(test[3] == 1){
				uart0_put_string("G006_test: test 3 OK\n\r");
				test[3] = 0;	//test3 PASS
			}else{
				uart0_put_string("G006_test: test 3 FAIL\n\r");
				test[3] = -1000;
			}
			test[4]++;	// test4 starts
			testMode = 4;
		}		
		//2 free memory blocks
		release_processor();

		// releases 1 memory block
		release_memory_block(mem_ptr[7]);
		if(testMode == 4){
			test[4]++;	//test4 is 4
		}
		//2 free memory blocks left
		release_processor();
	}

}

/**
 * @brief: a process that requests 3 memory blocks
 *         and then yields the cpu.
 *         a process does not request or release memory block, it counts numbers
 *         and then yields the cpu once it counts until i = 100
 *         a process that requests 3 memory blocks
 *         and then yields the cpu 
 *         a process does not request or release memory blcok, it counts numbers
 *         and then yileds the cpu once it counts until i = 100
 *         a proces that releases 2 memory blocks
 *         and then yields the cpu
 *         a process does not request or release memory blcok, it counts numbers
 *         and then yileds the cpu once it counts until i = 100
 *         a proces that requests 2 memory blocks
 *         and then yields the cpu
 *         
 */

void proc2(void){
	void* mem_ptr[8];
	int i;
	int x = 0;
    while(1){

		for(i = 0; i < 3; i++){
			mem_ptr[i] = request_memory_block();
		}
		
		if(testMode == 1){
			test[1]++;;	//test1 is 2
		}

		if(testMode == 5){
			test[5] = -1000;	//test5 FAIL
		}
		
		if(testMode == 6){
			test[6]++;	//test6 is 2
		} 
		release_processor();
		
		//delay 1000
		while(x < 1000){
			x++;
		}
		
		if(testMode == 2){
			test[2]++;	//test2 is 2
		}
		
		if(testMode == 6){
			test[6]++;	//test6 is 4 
		}
		//still have 6 memory blocks free
		release_processor();

		// requests 3 memory blocks
		mem_ptr[3] = request_memory_block();
		mem_ptr[4] = request_memory_block();
		
		//mem_ptr[5] causes blocking proc2 as there is no more memory block
		mem_ptr[5] = request_memory_block();
		if(testMode == 2){
			test[2] = -1000;
		}
		
		//delay 500
		x = 0;
		while(x < 500){
			x++;
		}
		if(testMode == 4){
			test[4]++;	// test4 is 2
		}
		release_processor();

		release_memory_block(mem_ptr[0]);
		release_memory_block(mem_ptr[1]);
		
		if(testMode == 4){
			test[4]++;	//test4 is 5
		}
		//4 free memory blocks
		release_processor();
		
		//delay 1000
		x = 0;
		while(x < 1000){
			++x;
		}
		release_processor();  

      	mem_ptr[6] = request_memory_block();
		mem_ptr[7] = request_memory_block();
		//3 free memory blocks 
		release_processor();
		}
	}
			
/**
 * @brief: a process that requests 1 memory block 
 *         and then yields the cpu.
 *         a process that requests 1 memory block
 *         and then yields the cpu.
 *         a process that requests 2 memory blocks
 *         and then yields the cpu.
 *         a process that release 2 memory blocks
 *         and then yields the cpu.
 */
			
void proc3(void){
	void* mem_ptr[4];
	int i;
	
	while(1){
		mem_ptr[0] = request_memory_block();
		if(testMode == 1){
			test[1]++;	//test1 is 3
		}
		if(testMode == 5){
			test[5] = -1000;	//test5 FAIL
		}

		//delay 1000
		i = 0;
		while(i < 1000){
			i++;
		}

		release_processor();
		
		mem_ptr[1] = request_memory_block();
		
		if(testMode == 2){
			test[2]++;	//test2 is 3
		}
		if(testMode == 6){
			test[6]++;	//test6 is 5
		}

		release_processor();
		
		if(testMode == 2){
			if(test[2] == 4){
				uart0_put_string("G006_test: test 2 OK\n\r");
				test[2] = 0;	//test2 PASS
			}else{
				uart0_put_string("G006_test: test 2 FAIL\n\r");
				test[2] = -1000;
			}
			test[3]++;	//test3 starts
			testMode = 3;
		}

		//the next line causes blocking proc3
		mem_ptr[2] = request_memory_block();
		if(testMode == 3){
			test[3] = -1000;	//test3 FAIL
		}
		
		if(testMode == 6){
			test[6] = -1000;	//test6 FAIL
		}

		mem_ptr[3] = request_memory_block(); 
		if(testMode == 4){
			test[4]++;	// test4 is 3
		}

		release_processor();

		release_memory_block(mem_ptr[0]);
		release_memory_block(mem_ptr[1]);
		//6 free memory blocks
		if(testMode == 4){
			if(test[4] == 5){
				uart0_put_string("G006_test: test 4 OK\n\r");
				test[4] = 0;	//test4 PASS
			}else{
				uart0_put_string("G006_test: test 4 FAIL\n\r");
				test[4] = -1000;	//test4 FAIL
			}
			test[5]++;	//test5 starts
			testMode = 5;
		}
		release_processor();          
	}
}
/**
 * @brief: a process that does not request or release memory but counting numbers
 *         and then yields the cpu until i reaches to 100.
 *         a process that does not request or release memory but counting numbers
 *         and then yields the cpu until i reaches to 100.
 *         a process that requests 1 memory block
 *         and then yields the cpu
 *         a process that releases 1 memory block
 *         and then yields the cpu
 */

void proc4(void){
    void* mem_ptr;
	int i;
	int x = 0;
  while (1) {

		i = 0;
		while(i < 1000){
			i++;
		}
		
		release_processor();

		i = 0;
		while(i < 500){
			i++;
		}
		
		release_processor();
		
		//gets blocked
		mem_ptr = request_memory_block();

		release_processor();

		release_memory_block(mem_ptr);

		release_processor();

    }
	}



/**
 * @brief: a process that requests 3 memory blocks 
 *         and then yields the cpu.
 *         a process that releases 3 memory blocks
 *         and then yields the cpu.
 */
	
void proc5(void){
 	void* mem_ptr[3];
	while(1) {
		//proc5 gets blocked
		mem_ptr[0] = request_memory_block();
		if(testMode == 6){
			test[6] = -1000;	//test6 FAIL
		}
		mem_ptr[1] = request_memory_block();
		mem_ptr[2] = request_memory_block();
		
		release_processor();
	
		release_memory_block(mem_ptr[0]); 
		release_memory_block(mem_ptr[1]); 
		release_memory_block(mem_ptr[2]); 
	
		release_processor();
	
	}

}

/**
 * @brief: a process that is the lowest and prints the wrap-up line for testing processes
 */
void proc6(void){
	void* mem_ptr;
	int i;
	int pass;
	int fail;
        while(1) {
		if(testMode == 6){
			if(test[6] == 2){
				uart0_put_string("G006_test: test 6 OK\n\r");
				test[6] = 0;
			}else{
				uart0_put_string("G006_test: test 6 FAIL\n\r");
				test[6] = -1000;
			}
	
			fail = 0;
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
			testMode = 0;	// no more testing
		}
	
		//gets blocked
		mem_ptr = request_memory_block();
		release_processor();

        }

}



