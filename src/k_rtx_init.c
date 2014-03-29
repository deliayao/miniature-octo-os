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

void k_rtx_init(void) {
    __disable_irq();
    
#ifndef DEBUG_PERFORMANCE // disable primary timer interrupts during performance testing
    timer_init(0); /* initialize timer 0 */
#endif /* !DEBUG_PERFORMANCE */
    timer_init(1); /* initialize timer 1 */
    uart0_irq_init(); // uart0 interrupt driven, for RTX console
    uart1_polling_init(); // uart1 polling, for debugging
    memory_init();
    process_init();
    __enable_irq();

	// start the first process
    k_release_processor();
}
