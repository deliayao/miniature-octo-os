/**
 * @brief: uart.h 
 * @author: Yiqing Huang
 * @date: 2014/02/08
 */

#ifndef UART_IRQ_H_
#define UART_IRQ_H_

#include <stdint.h>
#include "Utilities/Definitions.h"

#define uart0_irq_init() uart_irq_init(0)
#define uart1_irq_init() uart_irq_init(1)

#define uart0_polling_init() uart_polling_init(0)
#define uart1_polling_init() uart_polling_init(1)

/**
 * Initializes the UART i-process table item. Called during process
 * initialization.
 */
void initializeUARTProcess(void);

/**
 * Initializes n_uart to use interrupts.
 * 
 * @param   n_uart The UART to initialize.
 * @return  The success/failure of the operation.
 */
int uart_irq_init(int n_uart);

/**
 * Initializes n_uart to use polling.
 * 
 * @param   n_uart The UART to initialize.
 * @return  The success/failure of the operation.
 */
int uart_polling_init(int n_uart);

#ifdef _DEBUG_HOTKEYS
/**
 * Takes in a hotkey and prints debug information to UART1.
 * 
 * @param   hotkey The hotkey to respond to.
 */
void hotkeyHandler(char hotkey);
#endif /* _DEBUG_HOTKEYS */

#endif /* ! UART_IRQ_H_ */
