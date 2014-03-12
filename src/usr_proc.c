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

#include "printf.h"


/* --Definition-- */
#define NUM_BLOCK 40

/* initialization table item */
PROC_INIT g_test_procs[NUM_TEST_PROCS];

int test[7] = {0};	//Flag for test cases 1 to 6
int testMode = 1;  //The testMode to indicate which test case is running. 1 is running and 0 is not


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
* test1: proc1 should be blocked when it tried to received the message but there is no message to be sent to proc1
* test2: 
* test3: 
* test4: 
* test5: 
* test6: 
*/

/**
 * @brief: the proc1 tried to received the message. 
 */


void proc1(void) {
	Letter* received;

	while(1) {
		
		if(testMode){
			test[1]++;	//test1 is 1
		}
		
		received = (Letter*)receive_message(NULL);
		//this should be blocked here

		if(testMode && test[1] != 0){
			test[1] = -1000;	//test1 fails
		}
	}
}


/**
 * @brief: the proc2 sent the command registration to KCD 
 */

void proc2(void){
	Letter* message;
	Letter* received;
	int sender;
	int i;
		
	while(1) {
		
		if(testMode && test[4] != 0){
			test[4]++;	//test4 is 2
		}

		message = (Letter*)request_memory_block();
		message->m_Type = KCD_REG;
		message->m_Text[0] = '%';
		message->m_Text[1] = 'V';
		message->m_Text[2] = 'I';
		message->m_Text[3] = '\0';
		
		if(testMode)	test[2]++;	//test2 is 1
		send_message(KCD_PROCESS, (void*)message);
		release_processor();
		
		received = (Letter*)receive_message(&sender); // get block at first time when proc2 tried to receive the message
																									// will get unblocked if the proc3 send the message to the proc2
		
		if(testMode){
			if(sender != PROCESS_3) {
				test[2] = -1000;	//test2 fails
			}
			
			if(received->m_Text[0] == 'H' && received->m_Text[1] == 'i' && received->m_Text[2] == '\0') {
				test[3]++;			//test3 is 1	
			}else{
				test[3] = -1000;	//test3 fails
			}
		}
		
		set_process_priority(PROCESS_2,LOWEST);	//set the process 2 to the medium priority so the process 3 will start running
		
		release_processor();

	}
}


/**
 * @brief:
 */

void proc3(void){
	Letter* message;
	Letter* received;
	int sender;
		
	while(1) {
		
		message = (Letter*)request_memory_block();
		message->m_Type = DEFAULT;
		message->m_Text[0] = 'H';
		message->m_Text[1] = 'i';
		message->m_Text[2] = '\0';
		
		if(testMode)	test[3]++;	//test3 is 2
		send_message(PROCESS_2, (void*)message);	
		release_processor();	//goes to proc2
		
		//get a message from proc4
		received = (Letter*)receive_message(&sender);	
		test[3] = 100;	
		//printf("got block\n");
		
		//printf("delayed for 3 seconds\n");
		if(testMode){
			if(sender != PROCESS_4){
				test[3] = -1000;	//test3 fails
			}
			
			if(testMode){
				test[4]++;	//test4 is 
			} else {
				test[4] = -1000;	//test4 fails
			}
		}
		
		release_memory_block((void*)message);
		set_process_priority(PROCESS_3, LOWEST);
		release_processor();
	}
}


/**
 * @brief: 
 */

void proc4(void){
	Letter* message;
	Letter* received;
	int i, j;
	char c = 'a';
	
	while(1) {
		
		message = (Letter*)request_memory_block();
		message->m_Type = DEFAULT;
		for(i = 0; i < 26; i ++){
				message->m_Text[i] = 65 + i;
				printf("%c\n\r", message->m_Text[i]);
		}

		if(testMode)	test[5]++;	//test5 is 1
		
		for(i = 0; i < 3; i++){
			printf("%c\n\r", c+i);	//printing abc
		}

		delayed_send(PROCESS_3, (void *)message, 3000);
		//testMode is 1
		j=0;
		while(test[3] != 100){	//if test3 didn't fail in proc3 then this loop is terminated when it comes back to proc4
			j++;
			if(test[3] == -1000)	break;	//test3 fails
		}
		
		printf("%c\n\r", c+i);	//printing d
		if(testMode == 1 && i == 3){
			test[5]++;
		}else{
			test[5] = -1000;
		}
		
		delayed_send(PROCESS_5, (void *)message, 1000);
		//block proc4
		received = (Letter*)receive_message(NULL);
	}
}


/**
 * @brief: 
 */

void proc5(void){
	Letter* received;
	void* mem_ptr;
	
	while(1) {
		received = (Letter*)receive_message(NULL);
		
		if(testMode){
			test[6]++;	//test6 is initiated and increased to 1
		}
		
		release_memory_block((void*)received);
		
		//block proc5
		while(testMode){
			mem_ptr = request_memory_block();
		}
		test[6] = -1000;
	}
}


/**
 * @brief: 
 */

void proc6(void){
	int fail = 0;
	int pass;
	int i;
	
	while(1) {
		
		while(testMode){
			i++;
			if(test[6] > 0)	break;	//if delay send to proc5 succeeded then test[6] is 1 so that it breaks the loop
		}
		
		if(testMode) {
			uart0_put_string("\n\r");
			uart0_put_string("G006_test: START\n\r");
			uart0_put_string("G006_test: total 6 tests\n\r");
			
			for(i = 1; i < 7 ; ++i){
				if(test[i] <= 0){
					fail++;
					uart0_put_string("G006_test: test");
					uart0_put_char('0' + i);
					uart0_put_string(" FAIL\n\r");
				}else{
					uart0_put_string("G006_test: test");
					uart0_put_char('0' + i);
					uart0_put_string(" OK\n\r");
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
