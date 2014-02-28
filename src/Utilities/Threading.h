/**
 * @file:   Threading.h
 * @brief:  Threading utilities
 */

#ifndef _THREADING_
#define _THREADING_

// turns interrupts on or off
// 1 -> atomic on (turn interrupts off)
// 0 -> atomic off (turn interrupts back on)
void atomic(int state);

#endif /*_THREADING_*/
