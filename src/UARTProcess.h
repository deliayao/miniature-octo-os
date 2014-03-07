/**
 * @file:   UARTProcess.h
 * @brief:  UART interrupt process
 */

#ifndef _UART_PROCESS_
#define _UART_PROCESS_

#include "Utilities/Types.h"

#define __SVC_0  __svc_indirect(0)



extern int k_release_processor(void);
#define release_processor() _release_processor((U32)k_release_processor)
extern int __SVC_0 _release_processor(U32 p_func);

#endif /*_UART_PROCESS_*/
