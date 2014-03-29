/**
 * @brief timer.c - Timer example code. Tiemr IRQ is invoked every 1ms
 * @author T. Reidemeister
 * @author Y. Huang
 */

#include "PerformanceTimer.h"

#include "Utilities/Definitions.h"
#include "k_process.h"

#include <LPC17xx.h>

volatile uint32_t performanceTimerCount = 0; // increment every 1 ms

void performanceTimerStart() {
    performanceTimerCount = 0;
}

uint32_t performanceTimerEnd() {
    return performanceTimerCount;
}

/**
 * @brief: use CMSIS ISR for TIMER1 IRQ Handler
 * NOTE: This example shows how to save/restore all registers rather than just
 *       those backed up by the exception stack frame. We add extra
 *       push and pop instructions in the assembly routine. 
 *       The actual c_TIMER0_IRQHandler does the rest of irq handling
 */
__asm void TIMER1_IRQHandler(void)
{
	PRESERVE8
	IMPORT c_TIMER1_IRQHandler
	PUSH{r4-r11, lr}
	BL c_TIMER1_IRQHandler
	POP{r4-r11, pc}
} 
/**
 * @brief: c TIMER0 IRQ Handler
 */
void c_TIMER1_IRQHandler(void) {
    __disable_irq();
	/* acknowledge interrupt, see section  21.6.1 on pg 493 of LPC17XX_UM */
	LPC_TIM1->IR = BIT(0);  
	
    // increment timer
	performanceTimerCount++;
    __enable_irq();
}

