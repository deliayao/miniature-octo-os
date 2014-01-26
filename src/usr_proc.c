/**
 * @file:   usr_proc.c
 * @brief:  Two user processes: proc1 and proc2
 * @author: Yiqing Huang
 * @date:   2014/01/17
 * NOTE: Each process is in an infinite loop. Processes never terminate.
 */

#include "rtx.h"
#include "uart_polling.h"
#include "usr_proc.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* DEBUG_0 */

/* --Definition-- */
#define ARRAY_SIZE 10

/* initialization table item */
PROC_INIT g_test_procs[NUM_TEST_PROCS];
/* initialization null process*/
PROC_INIT null_proc;

void set_test_procs() {
	int i;
	for( i = 0; i < NUM_TEST_PROCS; i++ ) {
		g_test_procs[i].m_pid = (U32)(i+1);
		g_test_procs[i].m_stack_size = 0x100;
	}
	
	null_proc.m_pid = (U32) 0;
	null_proc.m_priority = LOWEST;
	null_proc.m_stack_size = 0x100;
	
	g_test_procs[0].m_priority = HIGH;
	g_test_procs[1].m_priority = MEDIUM;
	g_test_procs[2].m_priority = LOW;
	g_test_procs[3].m_priority = LOWEST;
	g_test_procs[4].m_priority = MEDIUM;
	g_test_procs[5].m_priority = LOW;
	
	null_proc.mpf_start_pc = &null_process;
 
	//case one: 
	g_test_procs[0].mpf_start_pc = &proc1;
	g_test_procs[1].mpf_start_pc = &proc2;
	
	g_test_procs[2].mpf_start_pc = &proc3;
	g_test_procs[3].mpf_start_pc = &proc4;
	g_test_procs[4].mpf_start_pc = &proc5;
	g_test_procs[5].mpf_start_pc = &proc6;
}

/**
 * @brief: a process that prints five uppercase letters
 *         and then yields the cpu.
 */

void null_process() {
	while(1){
		release_processor();
	}
}

void proc1(void) {
	void* mem_ptr[10];
	int i;
	while(1){
		for(i = 0; i < 10; i ++) {
			mem_ptr[i] = request_memory_block();
			#ifdef DEBUG_0
			printf("proc1 has %d memory blocks\n", i+1);
			#endif /* DEBUG_0 */
		}
		release_processor(); // Proc2
		for(i = 0; i < 10; i ++) {
			if(mem_ptr[i] != NULL){
				release_memory_block(mem_ptr[i]);
			}
		}
		release_processor(); //Proc4
		
		for(i = 0; i < 10; i ++) {
			mem_ptr[i] = request_memory_block();
			#ifdef DEBUG_0
			printf("proc1: test3_proc1 has %d memory blocks\n", i+1);
			#endif /* DEBUG_0 */
		}
		
		#ifdef DEBUG_0
		printf("proc1: test4_This should not be printed as proc1 gets blocked.");
		#endif /* DEBUG_0 */
	}
}

void proc2(void){
	void* mem_ptr[2];
	int i;
	while(1){
		for(i = 0; i < 2; i ++) {
			mem_ptr[i] = request_memory_block();
			#ifdef DEBUG_0
				printf("proc2: test1_proc2 gets blocked\n"); //test one it should not print anything 
			#endif /* DEBUG_0 */
		}
		release_processor();
	}
}
void proc3(void){
	void* mem_ptr[5];
	int i;
	while(1){
		#ifdef DEBUG_0
		printf("proc4: test6_this is the last print.\n");
		#endif /* DEBUG_0 */
		for(i = 0; i < 5; i++){
			mem_ptr[i] = request_memory_block();
			#ifdef DEBUG_0
				printf("proc4: test2_proc4 gets %d memory blocks.\n", i+1); //prints 1 to 5
			#endif /* DEBUG_0 */
			
		}
		release_processor();
		
		for(i = 0; i < 5; i++){
			if(mem_ptr[i] != NULL){
				release_memory_block(mem_ptr[i]);
			}
		}
		release_processor();
		
		#ifdef DEBUG_0
		printf("proc4: test5_this gets printed.\n");
		#endif /* DEBUG_0 */
	}
}
void proc4(void){
	int i = 0;
        int ret_val = 10;
        while ( 1) {
                if ( i != 0 && i%5 == 0 ) {
                        uart0_put_string("\n\r");
                        ret_val = release_processor();
#ifdef DEBUG_0
                        printf("proc1: ret_val=%d\n", ret_val);
#endif /* DEBUG_0 */
                }
                uart0_put_char('A' + i%26);
                i++;
        }
}
void proc5(void){}
void proc6(void){}

