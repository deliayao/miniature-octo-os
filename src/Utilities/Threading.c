/**
 * @file:   Threading.c
 * @brief:  Threading utilities implementation
 */

#include "Threading.h"

void atomic(int state) {
    if (state == 1) { // turn interrupts off
		    __disable_irq();
		} else { // turn interrupts on
		    __enable_irq();
		}
}
