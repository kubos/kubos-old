;/*************************************************************************//**
; * @file:    startup_efm32.s
; * @purpose: CMSIS Cortex-M3 Core Device Startup File 
; *           for the Energy Micro 'EFM32G' Device Series 
; * @version 1.0.2
; * @date:    10. September 2009
; *----------------------------------------------------------------------------
; *
; * Copyright (C) 2009 ARM Limited. All rights reserved.
; *
; * ARM Limited (ARM) is supplying this software for use with Cortex-Mx 
; * processor based microcontrollers.  This file can be freely distributed 
; * within development tools that are supporting such ARM based processors. 
; *
; * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
; * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
; * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
; * ARM SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
; * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
; *
; ******************************************************************************/


;
; The modules in this file are included in the libraries, and may be replaced
; by any user-defined modules that define the PUBLIC symbol _program_start or
; a user defined start symbol.
; To override the cstartup defined in the library, simply add your modified
; version to the workbench project.
;
; The vector table is normally located at address 0.
; When debugging in RAM, it can be located in RAM, aligned to at least 2^6.
; The name "__vector_table" has special meaning for C-SPY:
; it is where the SP start value is found, and the NVIC vector
; table register (VTOR) is initialized to this address if != 0.
;
; Cortex-M version
;
        MODULE  ?cstartup

        ;; Forward declaration of sections.
        SECTION CSTACK:DATA:NOROOT(3)

        SECTION .intvec:CODE:NOROOT(2)
            
        EXTERN  __iar_program_start
        EXTERN  SystemInit
        PUBLIC  __vector_table
        PUBLIC  __vector_table_0x1c
        PUBLIC  __Vectors
        PUBLIC  __Vectors_End
        PUBLIC  __Vectors_Size

        DATA

__vector_table
        DCD     sfe(CSTACK)
        DCD     Reset_Handler

        DCD     NMI_Handler
        DCD     HardFault_Handler
        DCD     MemManage_Handler
        DCD     BusFault_Handler
        DCD     UsageFault_Handler
__vector_table_0x1c
        DCD     0
        DCD     0
        DCD     0
        DCD     0
        DCD     vPortSVCHandler
        DCD     DebugMon_Handler
        DCD     0
        DCD     xPortPendSVHandler
        DCD     xPortSysTickHandler 

        ; External Interrupts
        DCD DMA_IRQHandler  ; 0: DMA Interrupt 
        DCD GPIO_EVEN_IRQHandler  ; 1: GPIO_EVEN Interrupt 
        DCD TIMER0_IRQHandler  ; 2: TIMER0 Interrupt 
        DCD USART0_RX_IRQHandler  ; 3: USART0_RX Interrupt 
        DCD USART0_TX_IRQHandler  ; 4: USART0_TX Interrupt 
        DCD ACMP0_IRQHandler  ; 5: ACMP0 Interrupt 
        DCD ADC0_IRQHandler  ; 6: ADC0 Interrupt 
        DCD DAC0_IRQHandler  ; 7: DAC0 Interrupt 
        DCD I2C0_IRQHandler  ; 8: I2C0 Interrupt 
        DCD GPIO_ODD_IRQHandler  ; 9: GPIO_ODD Interrupt 
        DCD TIMER1_IRQHandler  ; 10: TIMER1 Interrupt 
        DCD TIMER2_IRQHandler  ; 11: TIMER2 Interrupt 
        DCD USART1_RX_IRQHandler  ; 12: USART1_RX Interrupt 
        DCD USART1_TX_IRQHandler  ; 13: USART1_TX Interrupt 
        DCD USART2_RX_IRQHandler  ; 14: USART2_RX Interrupt 
        DCD USART2_TX_IRQHandler  ; 15: USART2_TX Interrupt 
        DCD UART0_RX_IRQHandler  ; 16: UART0_RX Interrupt 
        DCD UART0_TX_IRQHandler  ; 17: UART0_TX Interrupt 
        DCD LEUART0_IRQHandler  ; 18: LEUART0 Interrupt 
        DCD LEUART1_IRQHandler  ; 19: LEUART1 Interrupt 
        DCD LETIMER0_IRQHandler  ; 20: LETIMER0 Interrupt 
        DCD PCNT0_IRQHandler  ; 21: PCNT0 Interrupt 
        DCD PCNT1_IRQHandler  ; 22: PCNT1 Interrupt 
        DCD PCNT2_IRQHandler  ; 23: PCNT2 Interrupt 
        DCD SYSTICCK_IRQHandler;DCD RTC_IRQHandler  ; 24: RTC Interrupt 
        DCD CMU_IRQHandler  ; 25: CMU Interrupt 
        DCD VCMP_IRQHandler  ; 26: VCMP Interrupt 
        DCD LCD_IRQHandler  ; 27: LCD Interrupt 
        DCD MSC_IRQHandler  ; 28: MSC Interrupt 
        DCD AES_IRQHandler  ; 29: AES Interrupt 

__Vectors_End
__Vectors       EQU   __vector_table
__Vectors_Size  EQU     __Vectors_End - __Vectors


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Default interrupt handlers.
;;
        THUMB

        PUBWEAK Reset_Handler
        SECTION .text:CODE:REORDER(2)
Reset_Handler
        LDR     R0, =SystemInit
        BLX     R0
        LDR     R0, =__iar_program_start
        BX      R0

        PUBWEAK NMI_Handler
        SECTION .text:CODE:REORDER(1)
NMI_Handler
        B NMI_Handler

        PUBWEAK HardFault_Handler
        SECTION .text:CODE:REORDER(1)
HardFault_Handler
        B HardFault_Handler

        PUBWEAK MemManage_Handler
        SECTION .text:CODE:REORDER(1)
MemManage_Handler
        B MemManage_Handler

        PUBWEAK BusFault_Handler
        SECTION .text:CODE:REORDER(1)
BusFault_Handler
        B BusFault_Handler

        PUBWEAK UsageFault_Handler
        SECTION .text:CODE:REORDER(1)
UsageFault_Handler
        B UsageFault_Handler

        PUBWEAK vPortSVCHandler
        SECTION .text:CODE:REORDER(1)
vPortSVCHandler
        B vPortSVCHandler

        PUBWEAK DebugMon_Handler
        SECTION .text:CODE:REORDER(1)
DebugMon_Handler
        B DebugMon_Handler

        PUBWEAK xPortPendSVHandler
        SECTION .text:CODE:REORDER(1)
xPortPendSVHandler
        B xPortPendSVHandler

        PUBWEAK SYSTICCK_IRQHandler
        SECTION .text:CODE:REORDER(1)
SYSTICCK_IRQHandler
        B SYSTICCK_IRQHandler
        ; EFM32G specific interrupt handlers

        PUBWEAK DMA_IRQHandler
        SECTION .text:CODE:REORDER(1)
DMA_IRQHandler
        B DMA_IRQHandler
 
        PUBWEAK GPIO_EVEN_IRQHandler
        SECTION .text:CODE:REORDER(1)
GPIO_EVEN_IRQHandler
        B GPIO_EVEN_IRQHandler
 
        PUBWEAK TIMER0_IRQHandler
        SECTION .text:CODE:REORDER(1)
TIMER0_IRQHandler
        B TIMER0_IRQHandler
 
        PUBWEAK USART0_RX_IRQHandler
        SECTION .text:CODE:REORDER(1)
USART0_RX_IRQHandler
        B USART0_RX_IRQHandler
 
        PUBWEAK USART0_TX_IRQHandler
        SECTION .text:CODE:REORDER(1)
USART0_TX_IRQHandler
        B USART0_TX_IRQHandler
 
        PUBWEAK ACMP0_IRQHandler
        SECTION .text:CODE:REORDER(1)
ACMP0_IRQHandler
        B ACMP0_IRQHandler
 
        PUBWEAK ADC0_IRQHandler
        SECTION .text:CODE:REORDER(1)
ADC0_IRQHandler
        B ADC0_IRQHandler
 
        PUBWEAK DAC0_IRQHandler
        SECTION .text:CODE:REORDER(1)
DAC0_IRQHandler
        B DAC0_IRQHandler
 
        PUBWEAK I2C0_IRQHandler
        SECTION .text:CODE:REORDER(1)
I2C0_IRQHandler
        B I2C0_IRQHandler
 
        PUBWEAK GPIO_ODD_IRQHandler
        SECTION .text:CODE:REORDER(1)
GPIO_ODD_IRQHandler
        B GPIO_ODD_IRQHandler
 
        PUBWEAK TIMER1_IRQHandler
        SECTION .text:CODE:REORDER(1)
TIMER1_IRQHandler
        B TIMER1_IRQHandler
 
        PUBWEAK TIMER2_IRQHandler
        SECTION .text:CODE:REORDER(1)
TIMER2_IRQHandler
        B TIMER2_IRQHandler
 
        PUBWEAK USART1_RX_IRQHandler
        SECTION .text:CODE:REORDER(1)
USART1_RX_IRQHandler
        B USART1_RX_IRQHandler
 
        PUBWEAK USART1_TX_IRQHandler
        SECTION .text:CODE:REORDER(1)
USART1_TX_IRQHandler
        B USART1_TX_IRQHandler
 
        PUBWEAK USART2_RX_IRQHandler
        SECTION .text:CODE:REORDER(1)
USART2_RX_IRQHandler
        B USART2_RX_IRQHandler
 
        PUBWEAK USART2_TX_IRQHandler
        SECTION .text:CODE:REORDER(1)
USART2_TX_IRQHandler
        B USART2_TX_IRQHandler
 
        PUBWEAK UART0_RX_IRQHandler
        SECTION .text:CODE:REORDER(1)
UART0_RX_IRQHandler
        B UART0_RX_IRQHandler
 
        PUBWEAK UART0_TX_IRQHandler
        SECTION .text:CODE:REORDER(1)
UART0_TX_IRQHandler
        B UART0_TX_IRQHandler
 
        PUBWEAK LEUART0_IRQHandler
        SECTION .text:CODE:REORDER(1)
LEUART0_IRQHandler
        B LEUART0_IRQHandler
 
        PUBWEAK LEUART1_IRQHandler
        SECTION .text:CODE:REORDER(1)
LEUART1_IRQHandler
        B LEUART1_IRQHandler
 
        PUBWEAK LETIMER0_IRQHandler
        SECTION .text:CODE:REORDER(1)
LETIMER0_IRQHandler
        B LETIMER0_IRQHandler
 
        PUBWEAK PCNT0_IRQHandler
        SECTION .text:CODE:REORDER(1)
PCNT0_IRQHandler
        B PCNT0_IRQHandler
 
        PUBWEAK PCNT1_IRQHandler
        SECTION .text:CODE:REORDER(1)
PCNT1_IRQHandler
        B PCNT1_IRQHandler
 
        PUBWEAK PCNT2_IRQHandler
        SECTION .text:CODE:REORDER(1)
PCNT2_IRQHandler
        B PCNT2_IRQHandler
 
        PUBWEAK xPortSysTickHandler
        SECTION .text:CODE:REORDER(1)
xPortSysTickHandler
        B xPortSysTickHandler
 
        PUBWEAK CMU_IRQHandler
        SECTION .text:CODE:REORDER(1)
CMU_IRQHandler
        B CMU_IRQHandler
 
        PUBWEAK VCMP_IRQHandler
        SECTION .text:CODE:REORDER(1)
VCMP_IRQHandler
        B VCMP_IRQHandler
 
        PUBWEAK LCD_IRQHandler
        SECTION .text:CODE:REORDER(1)
LCD_IRQHandler
        B LCD_IRQHandler
 
        PUBWEAK MSC_IRQHandler
        SECTION .text:CODE:REORDER(1)
MSC_IRQHandler
        B MSC_IRQHandler
 
        PUBWEAK AES_IRQHandler
        SECTION .text:CODE:REORDER(1)
AES_IRQHandler
        B AES_IRQHandler
 
        END
