/*
	FreeRTOS.org V4.0.3 - Copyright (C) 2003-2006 Richard Barry.

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
	See http://www.FreeRTOS.org for documentation, latest information, license 
	and contact details.  Please ensure to read the configuration and relevant 
	port sections of the online documentation.
	***************************************************************************
*/

/*
	Changes between V4.0.0 and V4.0.1

	+ Reduced the code used to setup the initial stack frame.
	+ The kernel no longer has to install or handle the fault interrupt.
*/


/*-----------------------------------------------------------
 * Implementation of functions defined in portable.h for the ARM CM3 port.
 *----------------------------------------------------------*/

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Constants required to manipulate the NVIC. */
#define portNVIC_SYSTICK_CTRL		( ( volatile unsigned portLONG *) 0xe000e010 )
#define portNVIC_SYSTICK_LOAD		( ( volatile unsigned portLONG *) 0xe000e014 )
#define portNVIC_INT_CTRL			( ( volatile unsigned portLONG *) 0xe000ed04 )
#define portNVIC_SYSPRI2			( ( volatile unsigned portLONG *) 0xe000ed20 )
#define portNVIC_SYSPRI1			( ( volatile unsigned portLONG *) 0xe000ed1c )
#define portNVIC_SYSTICK_CLK		0x00000004
#define portNVIC_SYSTICK_INT		0x00000002
#define portNVIC_SYSTICK_ENABLE		0x00000001
#define portNVIC_PENDSVSET			0x10000000
#define portNVIC_PENDSV_PRI			0x00ff0000
#define portNVIC_SVCALL_PRI			0xff000000
#define portNVIC_SYSTICK_PRI		0xff000000

/* Constants required to set up the initial stack. */
#define portINITIAL_XPSR			( 0x01000000 )

/* Each task maintains its own interrupt status in the critical nesting
variable. */
unsigned portBASE_TYPE uxCriticalNesting = 0xaaaaaaaa;

/* 
 * Setup the timer to generate the tick interrupts.
 */
static void prvSetupTimerInterrupt( void );

/*
 * Exception handlers.
 */
void xPortPendSVHandler( void ) __attribute__ (( naked ));
void xPortSysTickHandler( void ) __attribute__ (( naked ));

/*
 * Set the MSP/PSP to a known value.
 */
void prvSetMSP( unsigned long ulValue ) __attribute__ (( naked ));
void prvSetPSP( unsigned long ulValue ) __attribute__ (( naked )); 

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
	*pxTopOfStack = 0xfffffffd;	/* LR */
	pxTopOfStack -= 5;	/* R12, R3, R2 and R1. */
	*pxTopOfStack = ( portSTACK_TYPE ) pvParameters;	/* R0 */
	pxTopOfStack -= 9;	/* R11, R10, R9, R8, R7, R6, R5 and R4. */
	*pxTopOfStack = 0x00000000; /* uxCriticalNesting. */

	return pxTopOfStack;
}
/*-----------------------------------------------------------*/

void prvSetPSP( unsigned long ulValue )
{
	asm volatile( "msr psp, r0" );
	asm volatile( "bx lr" );
}
/*-----------------------------------------------------------*/

void prvSetMSP( unsigned long ulValue )
{
	asm volatile( "msr msp, r0" );
	asm volatile( "bx lr" );
}
/*-----------------------------------------------------------*/

/* 
 * See header file for description. 
 */
portBASE_TYPE xPortStartScheduler( void )
{
	/* Make PendSV, CallSV and SysTick the lowest priority interrupts. */
	*(portNVIC_SYSPRI2) |= portNVIC_PENDSV_PRI;
	*(portNVIC_SYSPRI2) |= portNVIC_SYSTICK_PRI;

	/* Start the timer that generates the tick ISR.  Interrupts are disabled
	here already. */
	prvSetupTimerInterrupt();
	
	/* Start the first task. */
	prvSetPSP( 0 );
	prvSetMSP( *((unsigned portLONG *) 0 ) );
	*(portNVIC_INT_CTRL) |= portNVIC_PENDSVSET;

	/* Enable interrupts */
	portENABLE_INTERRUPTS();

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
	*(portNVIC_INT_CTRL) |= portNVIC_PENDSVSET;

	/* This function is also called in response to a Yield(), so we want
	the yield to occur immediately. */
	portENABLE_INTERRUPTS();
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

void xPortPendSVHandler( void )
{
	/* Start first task if the stack has not yet been setup. */
	__asm volatile
	( 
	"	mrs r0, psp						\n"
	"	cbz r0, no_save					\n"
	"									\n"	/* Save the context into the TCB. */					
	"	sub r0, #0x20					\n"
	"	stm r0, {r4-r11}				\n"
	"	nop								\n"
	"	sub r0, #0x04					\n"
	"	ldr r1, uxCriticalNestingConst	\n"
	"	ldr r1, [r1]					\n"
	"	stm r0, {r1}					\n"
	"	ldr r1, pxCurrentTCBConst		\n"
	"	ldr r1, [r1]					\n"
	"	str r0, [r1]					\n"
	"									\n"
	"no_save:\n"	
	"	ldr r0, vTaskSwitchContextConst	\n"	/* Find the task to execute. */
	"	push {r14}						\n"
	"	cpsid i							\n"
	"	blx r0							\n"
	"	cpsie i							\n"
	"	pop {r14}						\n"
	"									\n"	/* Restore the context. */	
	"	ldr r1, pxCurrentTCBConst		\n"
	"	ldr r1, [r1]					\n"
	"	ldr r0, [r1]					\n"
	"	ldm r0, {r1, r4-r11}			\n"
	"	nop								\n"
	"	ldr r2, uxCriticalNestingConst	\n"
	"	str r1, [r2]					\n"
	"	add r0, #0x24					\n"
	"	msr psp, r0						\n"
	"	orr r14, #0xd					\n"
	"									\n"	/* Exit with interrupts in the state required by the task. */	
	"	cbnz r1, sv_disable_interrupts	\n"
	"	bx r14							\n"
	"									\n"
	"sv_disable_interrupts:				\n"
	"	cpsid i							\n"
	"	bx r14							\n"
	"									\n"
	"	.align 2						\n"
	"vTaskSwitchContextConst: .word vTaskSwitchContext	\n"
	"pxCurrentTCBConst: .word pxCurrentTCB				\n"
	"uxCriticalNestingConst: .word uxCriticalNesting	\n"
	);
}
/*-----------------------------------------------------------*/

void xPortSysTickHandler( void )
{
	extern void vTaskIncrementTick( void );
	extern void vPortYieldFromISR( void );

	/* Call the scheduler tick function. */
	__asm volatile
	( 
	"	ldr r0, vTaskIncrementTickConst		\n"
	"	push {r14}							\n"
	"	cpsid i								\n"
	"	blx r0								\n"
	"	cpsie i								\n"
	"	pop {r14}" 
	);

	/* If using preemption, also force a context switch. */
	#if configUSE_PREEMPTION == 1
	__asm volatile
	( 
	"	push {r14}							\n"
	"	ldr r0, vPortYieldFromISRConst2		\n"
	"	blx r0								\n"
	"	pop {r14}" 
	);
	#endif

	/* Exit with interrupts in the correct state. */
	__asm volatile
	(
	"    ldr r2, uxCriticalNestingConst2	\n" 
	"    ldr r2, [r2]						\n"
	"    cbnz r2, tick_disable_interrupts	\n"
	"    bx r14" 
	);

   __asm volatile
   (
	"tick_disable_interrupts:				\n"
	"    cpsid i							\n"
	"    bx r14								\n"
	"										\n"
	"	.align 2							\n"
	"vPortYieldFromISRConst2: .word vPortYieldFromISR\n"
	"vTaskIncrementTickConst: .word vTaskIncrementTick\n" 
	"uxCriticalNestingConst2: .word uxCriticalNesting"
	);
}
/*-----------------------------------------------------------*/

/*
 * Setup the systick timer to generate the tick interrupts at the required
 * frequency.
 */
void prvSetupTimerInterrupt( void )
{
	/* Configure SysTick to interrupt at the requested rate. */
	*(portNVIC_SYSTICK_LOAD) = configCPU_CLOCK_HZ / configTICK_RATE_HZ;
	*(portNVIC_SYSTICK_CTRL) = portNVIC_SYSTICK_CLK | portNVIC_SYSTICK_INT | portNVIC_SYSTICK_ENABLE;
}


