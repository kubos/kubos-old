/*
	FreeRTOS.org V5.1.2 - Copyright (C) 2003-2009 Richard Barry.

	This file is part of the FreeRTOS.org distribution.

	FreeRTOS.org is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	FreeRTOS.org is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with FreeRTOS.org; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

	A special exception to the GPL can be applied should you wish to distribute
	a combined work that includes FreeRTOS.org, without being obliged to provide
	the source code for any proprietary components.  See the licensing section 
	of http://www.FreeRTOS.org for full details of how and when the exception
	can be applied.

    ***************************************************************************
    ***************************************************************************
    *                                                                         *
    * Get the FreeRTOS eBook!  See http://www.FreeRTOS.org/Documentation      *
	*                                                                         *
	* This is a concise, step by step, 'hands on' guide that describes both   *
	* general multitasking concepts and FreeRTOS specifics. It presents and   *
	* explains numerous examples that are written using the FreeRTOS API.     *
	* Full source code for all the examples is provided in an accompanying    *
	* .zip file.                                                              *
    *                                                                         *
    ***************************************************************************
    ***************************************************************************

	Please ensure to read the configuration and relevant port sections of the
	online documentation.

	http://www.FreeRTOS.org - Documentation, latest information, license and 
	contact details.

	http://www.SafeRTOS.com - A version that is certified for use in safety 
	critical systems.

	http://www.OpenRTOS.com - Commercial support, development, porting, 
	licensing and training services.
*/

/*-----------------------------------------------------------
 * Implementation of functions defined in portable.h for the ARM CM3 port.
 *----------------------------------------------------------*/

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"

#ifndef configKERNEL_INTERRUPT_PRIORITY
	#define configKERNEL_INTERRUPT_PRIORITY 255
#endif

/* Constants required to manipulate the NVIC. */
#define portNVIC_SYSTICK_CTRL		( ( volatile unsigned portLONG *) 0xe000e010 )
#define portNVIC_SYSTICK_LOAD		( ( volatile unsigned portLONG *) 0xe000e014 )
#define portNVIC_INT_CTRL			( ( volatile unsigned portLONG *) 0xe000ed04 )
#define portNVIC_SYSPRI2			( ( volatile unsigned portLONG *) 0xe000ed20 )
#define portNVIC_SYSTICK_CLK		0x00000004
#define portNVIC_SYSTICK_INT		0x00000002
#define portNVIC_SYSTICK_ENABLE		0x00000001
#define portNVIC_PENDSVSET			0x10000000
#define portNVIC_PENDSV_PRI			( ( ( unsigned portLONG ) configKERNEL_INTERRUPT_PRIORITY ) << 16 )
#define portNVIC_SYSTICK_PRI		( ( ( unsigned portLONG ) configKERNEL_INTERRUPT_PRIORITY ) << 24 )

/* Constants required to set up the initial stack. */
#define portINITIAL_XPSR			( 0x01000000 )

/* Each task maintains its own interrupt status in the critical nesting
variable. */
static unsigned portBASE_TYPE uxCriticalNesting = 0xaaaaaaaa;

/* 
 * Setup the timer to generate the tick interrupts.
 */
static void prvSetupTimerInterrupt( void );

/*
 * Exception handlers.
 */
void xPortPendSVHandler( void );
void xPortSysTickHandler( void );
void vPortSVCHandler( void );

/*
 * Start first task is a separate function so it can be tested in isolation.
 */
void vPortStartFirstTask( void );

/*-----------------------------------------------------------*/

/* 
 * See header file for description. 
 */
portSTACK_TYPE *pxPortInitialiseStack( portSTACK_TYPE *pxTopOfStack, pdTASK_CODE pxCode, void *pvParameters )
{
	/* Simulate the stack frame as it would be created by a context switch
	interrupt. */
	*pxTopOfStack = portINITIAL_XPSR;	/* xPSR */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) pxCode;	/* PC */
	pxTopOfStack--;
	*pxTopOfStack = 0;	/* LR */
	pxTopOfStack -= 5;	/* R12, R3, R2 and R1. */
	*pxTopOfStack = ( portSTACK_TYPE ) pvParameters;	/* R0 */
	pxTopOfStack -= 8;	/* R11, R10, R9, R8, R7, R6, R5 and R4. */

	return pxTopOfStack;
}
/*-----------------------------------------------------------*/

__asm void vPortSVCHandler( void )
{
	PRESERVE8

	ldr	r3, =pxCurrentTCB		/* Restore the context. */
	ldr r1, [r3]				/* Use pxCurrentTCBConst to get the pxCurrentTCB address. */
	ldr r0, [r1]				/* The first item in pxCurrentTCB is the task top of stack. */
	ldmia r0!, {r4-r11}		/* Pop the registers that are not automatically saved on exception entry and the critical nesting count. */
	msr psp, r0					/* Restore the task stack pointer. */
	mov r0, #0
	msr	basepri, r0
	orr r14, #0xd				
	bx r14						
}
/*-----------------------------------------------------------*/

__asm void vPortStartFirstTask( void )
{
	PRESERVE8

	/* Use the NVIC offset register to locate the stack. */
	ldr r0, =0xE000ED08
	ldr r0, [r0]
	ldr r0, [r0]
	/* Set the msp back to the start of the stack. */
	msr msp, r0
	/* Call SVC to start the first task. */
	svc 0
}
/*-----------------------------------------------------------*/

/* 
 * See header file for description. 
 */
portBASE_TYPE xPortStartScheduler( void )
{
	/* Make PendSV, CallSV and SysTick the same priroity as the kernel. */
	*(portNVIC_SYSPRI2) |= portNVIC_PENDSV_PRI;
	*(portNVIC_SYSPRI2) |= portNVIC_SYSTICK_PRI;

	/* Start the timer that generates the tick ISR.  Interrupts are disabled
	here already. */
	prvSetupTimerInterrupt();
	
	/* Initialise the critical nesting count ready for the first task. */
	uxCriticalNesting = 0;

	/* Start the first task. */
	vPortStartFirstTask();

	/* Should not get here! */
	return 0;
}
/*-----------------------------------------------------------*/

void vPortEndScheduler( void )
{
	/* It is unlikely that the CM3 port will require this function as there
	is nothing to return to.  */
}
/*-----------------------------------------------------------*/

void vPortYieldFromISR( void )
{
	/* Set a PendSV to request a context switch. */
	*(portNVIC_INT_CTRL) = portNVIC_PENDSVSET;
}
/*-----------------------------------------------------------*/

void vPortEnterCritical( void )
{
	portDISABLE_INTERRUPTS();
	uxCriticalNesting++;
}
/*-----------------------------------------------------------*/

void vPortExitCritical( void )
{
	uxCriticalNesting--;
	if( uxCriticalNesting == 0 )
	{
		portENABLE_INTERRUPTS();
	}
}
/*-----------------------------------------------------------*/

__asm void xPortPendSVHandler( void )
{
	extern uxCriticalNesting;
	extern pxCurrentTCB;
	extern vTaskSwitchContext;

	PRESERVE8

	mrs r0, psp						 

	ldr	r3, =pxCurrentTCB		 	 /* Get the location of the current TCB. */
	ldr	r2, [r3]						

	stmdb r0!, {r4-r11}				 /* Save the remaining registers. */
	str r0, [r2]					 /* Save the new top of stack into the first member of the TCB. */

	stmdb sp!, {r3, r14}		
	mov r0, #configMAX_SYSCALL_INTERRUPT_PRIORITY
	msr basepri, r0		 
	bl vTaskSwitchContext
	mov r0, #0
	msr basepri, r0
	ldmia sp!, {r3, r14}			

	ldr r1, [r3]					 
	ldr r0, [r1]					 /* The first item in pxCurrentTCB is the task top of stack. */
	ldmia r0!, {r4-r11}			 /* Pop the registers and the critical nesting count. */
	msr psp, r0						 
	bx r14
	nop
}
/*-----------------------------------------------------------*/

void xPortSysTickHandler( void )
{
unsigned portLONG ulDummy;

	/* If using preemption, also force a context switch. */
	#if configUSE_PREEMPTION == 1
		*(portNVIC_INT_CTRL) = portNVIC_PENDSVSET;	
	#endif

	ulDummy = portSET_INTERRUPT_MASK_FROM_ISR();
	{
		vTaskIncrementTick();
	}
	portCLEAR_INTERRUPT_MASK_FROM_ISR( ulDummy );
}
/*-----------------------------------------------------------*/

/*
 * Setup the systick timer to generate the tick interrupts at the required
 * frequency.
 */
void prvSetupTimerInterrupt( void )
{
	/* Configure SysTick to interrupt at the requested rate. */
	*(portNVIC_SYSTICK_LOAD) = ( configCPU_CLOCK_HZ / configTICK_RATE_HZ ) - 1UL;
	*(portNVIC_SYSTICK_CTRL) = portNVIC_SYSTICK_CLK | portNVIC_SYSTICK_INT | portNVIC_SYSTICK_ENABLE;
}
/*-----------------------------------------------------------*/

__asm void vPortSetInterruptMask( void )
{
	PRESERVE8

	push { r0 }
	mov r0, #configMAX_SYSCALL_INTERRUPT_PRIORITY
	msr basepri, r0
	pop { r0 }
	bx r14
}

/*-----------------------------------------------------------*/

__asm void vPortClearInterruptMask( void )
{
	PRESERVE8

	push { r0 }
	mov r0, #0
	msr basepri, r0
	pop { r0 }
	bx r14
}
