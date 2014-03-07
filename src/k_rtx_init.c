/** 
 * @file:   k_rtx_init.c
 * @brief:  Kernel initialization C file
 * @auther: Yiqing Huang
 * @date:   2014/01/17
 */

#include "k_rtx_init.h"
#include "k_memory.h"
#include "k_process.h"
#include "Timer.h"
#include "UART.h"

void k_rtx_init(void)
{
				
		
        __disable_irq();
		timer_init(0); /* initialize timer 0 */
        uart0_irq_init(); // uart0 interrupt driven, for RTX console
        uart1_polling_init(); // uart1 polling, for debugging
        memory_init();
        process_init();
        __enable_irq();

	/* start the first process */
	
        k_release_processor();
}
