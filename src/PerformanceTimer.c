/**
 * @file:   PerformanceTimer.c
 * @brief:  Performance timer implementation.
 */

#include "PerformanceTimer.h"

#include "k_process.h"
#include "Utilities/Definitions.h"

#include <LPC17xx.h>

/*
 * Counter for the performance timer. Increments every 1 ms.
 */
volatile uint32_t g_PerformanceTimerCount = 0;

uint32_t performanceTimerEnd() {
    return g_PerformanceTimerCount;
}

void performanceTimerStart() {
    g_PerformanceTimerCount = 0;
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

    // acknowledge interrupt, see section  21.6.1 on pg 493 of LPC17XX_UM
    LPC_TIM1->IR = BIT(0);  
    
    // increment timer
    g_PerformanceTimerCount++;
    __enable_irq();
}
