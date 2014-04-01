/**
 * @brief: uart_irq.c 
 * @author: NXP Semiconductors
 * @author: Y. Huang
 * @date: 2014/02/08
 */

#include "UART.h"

#include "k_memory.h"
#include "k_process.h"
#include "Polling/uart_polling.h"
#include "Utilities/String.h"

#include <LPC17xx.h>

#ifdef DEBUG_0
#include "printf.h"
#endif /* ! DEBUG_0 */

/**
 * UART i-process initialization table item. Initialized with values on an
 * initializeUARTProcess() call.
 */
PROC_INIT g_UARTProcess;

#ifdef _DEBUG_HOTKEYS
/**
 * Reserved space for printing hotkey debug information.
 */
static char s_DebugInfo[100];

/**
 * Hotkey characters.
 */
static char s_Hotkeys[3] = { '~', '!', '@' };

#endif /* _DEBUG_HOTKEYS */

int uart_irq_init(int n_uart) {
    LPC_UART_TypeDef* pUart;

    if (n_uart == 0) {
        /*
        Steps 1 & 2: system control configuration.
        Under CMSIS, system_LPC17xx.c does these two steps
         
        -----------------------------------------------------
        Step 1: Power control configuration. 
                See table 46 pg63 in LPC17xx_UM
        -----------------------------------------------------
        Enable UART0 power, this is the default setting
        done in system_LPC17xx.c under CMSIS.
        Enclose the code for your refrence
        //LPC_SC->PCONP |= BIT(3);
    
        -----------------------------------------------------
        Step2: Select the clock source. 
               Default PCLK=CCLK/4 , where CCLK = 100MHZ.
               See tables 40 & 42 on pg56-57 in LPC17xx_UM.
        -----------------------------------------------------
        Check the PLL0 configuration to see how XTAL=12.0MHZ 
        gets to CCLK=100MHZin system_LPC17xx.c file.
        PCLK = CCLK/4, default setting after reset.
        Enclose the code for your reference
        //LPC_SC->PCLKSEL0 &= ~(BIT(7)|BIT(6)); 
            
        -----------------------------------------------------
        Step 5: Pin Ctrl Block configuration for TXD and RXD
                See Table 79 on pg108 in LPC17xx_UM.
        -----------------------------------------------------
        Note this is done before Steps3-4 for coding purpose.
        */
        
        /* Pin P0.2 used as TXD0 (Com0) */
        LPC_PINCON->PINSEL0 |= (1 << 4);  
        
        /* Pin P0.3 used as RXD0 (Com0) */
        LPC_PINCON->PINSEL0 |= (1 << 6);  

        pUart = (LPC_UART_TypeDef *) LPC_UART0;  
        
    } else if (n_uart == 1) {
        
        /* see Table 79 on pg108 in LPC17xx_UM */ 
        /* Pin P2.0 used as TXD1 (Com1) */
        LPC_PINCON->PINSEL4 |= (2 << 0);

        /* Pin P2.1 used as RXD1 (Com1) */
        LPC_PINCON->PINSEL4 |= (2 << 2);          

        pUart = (LPC_UART_TypeDef *) LPC_UART1;
        
    } else {
        return -1; /* not supported yet */
    } 
    
    /*
    -----------------------------------------------------
    Step 3: Transmission Configuration.
            See section 14.4.12.1 pg313-315 in LPC17xx_UM 
            for baud rate calculation.
    -----------------------------------------------------
        */
    
    /* Step 3a: DLAB=1, 8N1 */
    pUart->LCR = UART_8N1; /* see uart.h file */ 

    /* Step 3b: 115200 baud rate @ 25.0 MHZ PCLK */
    pUart->DLM = 0; /* see table 274, pg302 in LPC17xx_UM */
    pUart->DLL = 9; /* see table 273, pg302 in LPC17xx_UM */
    
    /* FR = 1.507 ~ 1/2, DivAddVal = 1, MulVal = 2
       FR = 1.507 = 25MHZ/(16*9*115200)
       see table 285 on pg312 in LPC_17xxUM
    */
    pUart->FDR = 0x21;       
    
    /*
    ----------------------------------------------------- 
    Step 4: FIFO setup.
           see table 278 on pg305 in LPC17xx_UM
    -----------------------------------------------------
        enable Rx and Tx FIFOs, clear Rx and Tx FIFOs
    Trigger level 0 (1 char per interrupt)
    */
    
    pUart->FCR = 0x07;

    /* Step 5 was done between step 2 and step 4 a few lines above */

    /*
    ----------------------------------------------------- 
    Step 6 Interrupt setting and enabling
    -----------------------------------------------------
    */
    /* Step 6a: 
       Enable interrupt bit(s) wihtin the specific peripheral register.
           Interrupt Sources Setting: RBR, THRE or RX Line Stats
       See Table 50 on pg73 in LPC17xx_UM for all possible UART0 interrupt sources
       See Table 275 on pg 302 in LPC17xx_UM for IER setting 
    */
    /* disable the Divisior Latch Access Bit DLAB=0 */
    pUart->LCR &= ~(BIT(7)); 
    
    //pUart->IER = IER_RBR | IER_THRE | IER_RLS; 
    pUart->IER = IER_RBR | IER_RLS;

    /* Step 6b: enable the UART interrupt from the system level */
    
    if (n_uart == 0) {
        NVIC_EnableIRQ(UART0_IRQn); /* CMSIS function */
    } else if (n_uart == 1) {
        NVIC_EnableIRQ(UART1_IRQn); /* CMSIS function */
    } else {
        return 1; /* not supported yet */
    }
    pUart->THR = '\0';
    return 0;
}

int uart_polling_init(int n_uart) {

  LPC_UART_TypeDef *pUart;  /* ptr to memory mapped device UART, check         */
                            /* LPC17xx.h for UART register C structure overlay */

  if (n_uart == 0) {
    /*
    Step 1: system control configuration
     
    step 1a: power control configuration, table 46 pg63
    enable UART0 power, this is the default setting
    also already done in system_LPC17xx.c
    enclose the code below for reference 
    LPC_SC->PCONP |= BIT(3);
  
    
    step 1b: select the clock source, default PCLK=CCLK/4 , where CCLK = 100MHZ.
    tables 40 and 42  on pg56 and pg57
    Check the PLL0 configuration to see how XTAL=12.0MHZ gets to CCLK=100MHZ
    in system_LPC17xx.c file
    enclose code below for reference
    LPC_SC->PCLKSEL0 &= ~(BIT(7)|BIT(6));  // PCLK = CCLK/4, default setting after reset  

    Step 2: Pin Ctrl Block configuration for TXD and RXD
    Listed as item #5 in LPC_17xxum UART0/2/3 manual pag298
    */
    LPC_PINCON->PINSEL0 |= (1 << 4);    /* Pin P0.2 used as TXD0 (Com0) */
    LPC_PINCON->PINSEL0 |= (1 << 6);    /* Pin P0.3 used as RXD0 (Com0) */

    pUart = (LPC_UART_TypeDef *) LPC_UART0;
    
  } else if (n_uart == 1) {
    LPC_PINCON->PINSEL4 |= (2 << 0);    /* Pin P2.0 used as TXD1 (Com1) */
    LPC_PINCON->PINSEL4 |= (2 << 2);    /* Pin P2.1 used as RXD1 (Com1) */

    pUart = (LPC_UART_TypeDef *) LPC_UART1;

  } else {
    return -1;   /* not supported yet */
  } 

  /* Step 3: Transmission Configuration */
  
  /* step 3a: DLAB=1, 8N1 */
  pUart->LCR    = UART_8N1;

  /* step 3b: 115200 baud rate @ 25.0 MHZ PCLK */
  pUart->DLM = 0;
  pUart->DLL = 9;
  pUart->FDR = 0x21;        /* FR = 1.507 ~ 1/2, DivAddVal = 1, MulVal = 2  */
                            /* FR = 1.507 = 25MHZ/(16*9*115200)             */
  pUart->LCR &= ~(BIT(7));  /* disable the Divisior Latch Access Bit DLAB=0 */

  return 0;
}

void initializeUARTProcess() {
    g_UARTProcess.m_pid = (U32)UART_IPROCESS;
    g_UARTProcess.m_priority = NULL_PRIORITY;
    g_UARTProcess.m_stack_size = 0x100;
    g_UARTProcess.mpf_start_pc = NULL;
}

/**
 * @brief: use CMSIS ISR for UART0 IRQ Handler
 * NOTE: This example shows how to save/restore all registers rather than just
 *       those backed up by the exception stack frame. We add extra
 *       push and pop instructions in the assembly routine. 
 *       The actual c_UART0_IRQHandler does the rest of irq handling
 */
__asm void UART0_IRQHandler(void)
{
    PRESERVE8
    IMPORT c_UART0_IRQHandler
    PUSH{r4-r11, lr}
    BL c_UART0_IRQHandler
    POP{r4-r11, pc}
} 
/**
 * @brief: c UART0 IRQ Handler
 */
void c_UART0_IRQHandler(void)
{
    uint8_t IIR_IntId; // interrupt ID from IIR          
    LPC_UART_TypeDef *pUart;
    
    __disable_irq();
    pUart = (LPC_UART_TypeDef *)LPC_UART0;
    
#ifdef DEBUG_0
    uart1_put_string("Entering c_UART0_IRQHandler\n\r");
#endif // DEBUG_0
    // reading IIR automatically acknowledges the interrupt
    IIR_IntId = (pUart->IIR) >> 1 ; // skip pending bit in IIR 
    
    if (IIR_IntId & IIR_RDA) { // receive data available
        void* node;
        Letter* newLetter;
        uint8_t character;
        
        // read UART
        // reading RBR will clear the interrupt
        character = pUart->RBR;
#ifdef DEBUG_0
        uart1_put_string("Reading a char = ");
        uart1_put_char(character);
        uart1_put_string("\n\r");
#endif // DEBUG_0
        
#ifdef _DEBUG_HOTKEYS
        // check for hotkey
        if (strcont(s_Hotkeys, character)) { // placeholder for hotkeys
            hotkeyHandler(character);
        } else {
#endif // DEBUG_HOTKEYS
            // send a message to KCD containing the character
            node = nonBlockingRequestMemory(); // request memory for message
            if (node != NULL) {
                newLetter = (Letter*)node;
                newLetter->m_Type = DEFAULT;
                newLetter->m_Text[0] = character;
                newLetter->m_Text[1] = '\0';
                
                // send the letter
                nonPreemptiveSendMessage(UART_IPROCESS, KCD_PROCESS, (void*)newLetter);
            }
#ifdef _DEBUG_HOTKEYS   
        }
#endif // DEBUG_HOTKEYS
    } else if (IIR_IntId & IIR_THRE) { // transmission
        Letter* newLetter;
        
        // THRE interrupt, transmit holding register becomes empty
        newLetter = (Letter*)nonBlockingReceiveMessage(UART_IPROCESS, NULL);
        while (newLetter != NULL) {
            int i;
            for (i = 0; newLetter->m_Text[i] != '\0'; i++) {
                pUart->THR = newLetter->m_Text[i]; // print character
            }       
            nonPreemptiveReleaseMemory((void*)newLetter); // release message's memory
            newLetter = (Letter*)nonBlockingReceiveMessage(UART_IPROCESS, NULL);
        }
        pUart->IER ^= IER_THRE; // toggle IER_THRE bit
        pUart->THR = '\0';
    } else {
#ifdef DEBUG_0
            uart1_put_string("Should not get here!\n\r");
#endif // DEBUG_0
    }
    
    __enable_irq();
    k_release_processor();
}

#ifdef _DEBUG_HOTKEYS
void hotkeyHandler(char hotkey) {
    int i;
    int j = 0;
    
    if (s_DebugInfo != NULL) {
        if (hotkey == s_Hotkeys[0]) { // print ready queue processes and their priorities
            s_DebugInfo[0] = 'R';
            s_DebugInfo[1] = 'E';
            s_DebugInfo[2] = 'A';
            s_DebugInfo[3] = 'D';
            s_DebugInfo[4] = 'Y';
            s_DebugInfo[5] = ':';
            s_DebugInfo[6] = '\r';
            s_DebugInfo[7] = '\n';
            j = 8;
            serializeReadyQueue(s_DebugInfo, j);
        } else if (hotkey == s_Hotkeys[1]) { // print blocked on memory queue and their priorities
            s_DebugInfo[0] = 'B';
            s_DebugInfo[1] = 'L';
            s_DebugInfo[2] = 'O';
            s_DebugInfo[3] = 'C';
            s_DebugInfo[4] = 'K';
            s_DebugInfo[5] = 'E';
            s_DebugInfo[6] = 'D';
            s_DebugInfo[7] = ' ';
            s_DebugInfo[8] = 'M';
            s_DebugInfo[9] = 'E';
            s_DebugInfo[10] = 'M';
            s_DebugInfo[11] = ':';
            s_DebugInfo[12] = '\r';
            s_DebugInfo[13] = '\n';
            j = 14;
            serializeBlockedOnMemoryQueue(s_DebugInfo, j);
        } else if (hotkey == s_Hotkeys[2]) { // print blocked on receive processes and their priorities
            s_DebugInfo[0] = 'B';
            s_DebugInfo[1] = 'L';
            s_DebugInfo[2] = 'O';
            s_DebugInfo[3] = 'C';
            s_DebugInfo[4] = 'K';
            s_DebugInfo[5] = 'E';
            s_DebugInfo[6] = 'D';
            s_DebugInfo[7] = ' ';
            s_DebugInfo[8] = 'R';
            s_DebugInfo[9] = 'E';
            s_DebugInfo[10] = 'C';
            s_DebugInfo[11] = ' ';
            s_DebugInfo[12] = '(';
            s_DebugInfo[13] = 'P';
            s_DebugInfo[14] = 'R';
            s_DebugInfo[15] = 'O';
            s_DebugInfo[16] = 'C';
            s_DebugInfo[17] = ',';
            s_DebugInfo[18] = 'P';
            s_DebugInfo[19] = 'R';
            s_DebugInfo[20] = 'I';
            s_DebugInfo[21] = ')';
            s_DebugInfo[22] = ':';
            s_DebugInfo[23] = '\r';
            s_DebugInfo[24] = '\n';
            j = 25;
            serializeBlockedOnReceive(s_DebugInfo, j);
        }
        
        for (i=0; s_DebugInfo[i] != '\0'; i++) {
            uart1_put_char(s_DebugInfo[i]);
        }
    }
}

#endif /* _DEBUG_HOTKEYS */
