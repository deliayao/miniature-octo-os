/**
 * @file:   UARTProcess.c
 * @brief:  UART interrupt process implementation
 */

#include "UARTProcess.h"
#include "Polling/uart_polling.h"
#include "Utilities/k_rtx.h"
#include <LPC17xx.h>
#include "k_memory.h"
#include "k_process.h"

#include "printf.h"


void runUARTProcess() {
//     uint8_t IIR_IntId; // interrupt ID from IIR 		 
//     LPC_UART_TypeDef *pUart = (LPC_UART_TypeDef *)LPC_UART0;
//     
//     while (1) {
// 			// reading IIR automatically acknowledges the interrupt
//         IIR_IntId = (pUart->IIR) >> 1 ; // skip pending bit in IIR 
//         if (IIR_IntId & IIR_RDA) { // receive data available
//              uint8_t character;
//             
//             // read UART
//             // reading RBR will clear the interrupt
//              character = pUart->RBR;
//              printf("%c\r\n", character);
//        }
//     #ifdef DEBUG_0
//         uart1_put_string("Entering c_UART0_IRQHandler\n\r");
//     #endif // DEBUG_0
//         // reading IIR automatically acknowledges the interrupt
//         IIR_IntId = (pUart->IIR) >> 1 ; // skip pending bit in IIR 
//         if (IIR_IntId & IIR_RDA) { // receive data available
//             void* node;
//             Letter* newLetter;
//              uint8_t character;
//             
//             // read UART
//             // reading RBR will clear the interrupt
//              character = pUart->RBR;
//              //printf("%c\r\n", character);
//     #ifdef DEBUG_0
//             uart1_put_string("Reading a char = ");
//             uart1_put_char(g_char_in);
//             uart1_put_string("\n\r");
//     #endif // DEBUG_0
//             
//             // send a message to KCD containing the character
//             node = nonBlockingRequestMemory(); // request memory for message
//             if (node != NULL) {
//                 void * a;
//                 newLetter = (Letter*)node;
//                 newLetter->m_Type = DEFAULT;
//                 newLetter->m_Text = character;
//                 
//                 a = (void *)newLetter;
//                 
//                 // send the letter
//                 k_send_message(KCD_PROCESS, a);
//             }
//        } else if (IIR_IntId & IIR_THRE) { // transmission
//             Envelope* message;
//             Letter* newLetter;
//             
//             // THRE interrupt, transmit holding register becomes empty
//             message = nonBlockingReceiveMessage(UART_IPROCESS, NULL);
//             if (message != NULL) {
//                 newLetter = (Letter*)((U32)message + sizeof(Envelope));
//                 pUart->THR = newLetter->m_Text; // print character
//                 k_release_memory_block((void*)newLetter); // release message's memory
//             }
//             
//             pUart->IER ^= IER_THRE; // toggle IER_THRE bit 
//         }
//         __enable_irq();
//         k_release_processor();
//     }
}
