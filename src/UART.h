/**
 * @brief: uart.h 
 * @author: Yiqing Huang
 * @date: 2014/02/08
 */

#ifndef UART_IRQ_H_
#define UART_IRQ_H_

/* typedefs */
#include <stdint.h>
#include "Utilities/Definitions.h"

#define uart0_irq_init() uart_irq_init(0)
#define uart1_irq_init() uart_irq_init(1)

#define uart0_polling_init() uart_polling_init(0)
#define uart1_polling_init() uart_polling_init(1)

void initializeUARTProcess(void); // initializes the UART i-process
     
int uart_irq_init(int n_uart); // initializes n_uart to use interrupts
int uart_polling_init(int n_uart); // initializes n_uart to use polling

//debug functions
void hotkeyHandler(char hotkey); //takes in a hotkey and eventually prints debug information to uart1

#endif /* ! UART_IRQ_H_ */
