/*
    FreeRTOS V7.3.0 - Copyright (C) 2012 Real Time Engineers Ltd.

    FEATURES AND PORTS ARE ADDED TO FREERTOS ALL THE TIME.  PLEASE VISIT
    http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS tutorial books are available in pdf and paperback.        *
     *    Complete, revised, and edited pdf reference manuals are also       *
     *    available.                                                         *
     *                                                                       *
     *    Purchasing FreeRTOS documentation will not only help you, by       *
     *    ensuring you get running as quickly as possible and with an        *
     *    in-depth knowledge of how to use FreeRTOS, it will also help       *
     *    the FreeRTOS project to continue with its mission of providing     *
     *    professional grade, cross platform, de facto standard solutions    *
     *    for microcontrollers - completely free of charge!                  *
     *                                                                       *
     *    >>> See http://www.FreeRTOS.org/Documentation for details. <<<     *
     *                                                                       *
     *    Thank you for using FreeRTOS, and thank you for your support!      *
     *                                                                       *
    ***************************************************************************


    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation AND MODIFIED BY the FreeRTOS exception.
    >>>NOTE<<< The modification to the GPL is included to allow you to
    distribute a combined work that includes FreeRTOS without being obliged to
    provide the source code for proprietary components outside of the FreeRTOS
    kernel.  FreeRTOS is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
    or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
    more details. You should have received a copy of the GNU General Public
    License and the FreeRTOS license exception along with FreeRTOS; if not it
    can be viewed here: http://www.freertos.org/a00114.html and also obtained
    by writing to Richard Barry, contact details for whom are available on the
    FreeRTOS WEB site.

    1 tab == 4 spaces!

    ***************************************************************************
     *                                                                       *
     *    Having a problem?  Start by reading the FAQ "My application does   *
     *    not run, what could be wrong?"                                     *
     *                                                                       *
     *    http://www.FreeRTOS.org/FAQHelp.html                               *
     *                                                                       *
    ***************************************************************************


    http://www.FreeRTOS.org - Documentation, training, latest versions, license
    and contact details.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool.

    Real Time Engineers ltd license FreeRTOS to High Integrity Systems, who sell
    the code with commercial support, indemnification, and middleware, under
    the OpenRTOS brand: http://www.OpenRTOS.com.  High Integrity Systems also
    provide a safety engineered and independently SIL3 certified version under
    the SafeRTOS brand: http://www.SafeRTOS.com.
*/

/*
	Changes from V4.2.1

	+ Introduced the configKERNEL_INTERRUPT_PRIORITY definition.
*/

/*-----------------------------------------------------------
 * Implementation of functions defined in portable.h for the PIC24 port.
 *----------------------------------------------------------*/

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"

/* Hardware specifics. */
#define portBIT_SET 1
#define portTIMER_PRESCALE 8
#define portINITIAL_SR	0

/* Defined for backward compatability with project created prior to
FreeRTOS.org V4.3.0. */
#ifndef configKERNEL_INTERRUPT_PRIORITY
	#define configKERNEL_INTERRUPT_PRIORITY 1
#endif

/* The program counter is only 23 bits. */
#define portUNUSED_PR_BITS	0x7f

/* Records the nesting depth of calls to portENTER_CRITICAL(). */
unsigned portBASE_TYPE uxCriticalNesting = 0xef;

#if configKERNEL_INTERRUPT_PRIORITY != 1
	#error If configKERNEL_INTERRUPT_PRIORITY is not 1 then the #32 in the following macros needs changing to equal the portINTERRUPT_BITS value, which is ( configKERNEL_INTERRUPT_PRIORITY << 5 )
#endif

#ifdef MPLAB_PIC24_PORT

    #ifdef __HAS_EDS__
		#define portRESTORE_CONTEXT()                                                                                                                   \
					asm volatile(	"MOV	_pxCurrentTCB, W0		\n"	/* Restore the stack pointer for the task. */				\
							"MOV	[W0], W15				\n"																\
							"POP	W0						\n"	/* Restore the critical nesting counter for the task. */	\
							"MOV	W0, _uxCriticalNesting	\n"																\
							"POP	DSWPAG					\n"																\
							"POP    DSRPAG                                  \n"                                             \
							"POP	CORCON					\n"																\
							"POP	TBLPAG					\n"																\
							"POP	RCOUNT					\n"	/* Restore the registers from the stack. */					\
							"POP	W14						\n"																\
							"POP.D	W12						\n"																\
							"POP.D	W10						\n"																\
							"POP.D	W8						\n"																\
							"POP.D	W6						\n"																\
							"POP.D	W4						\n"																\
							"POP.D	W2						\n"																\
							"POP.D	W0						\n"																\
							"POP	SR						  " );
	#else /* __HAS_EDS__ */
		#define portRESTORE_CONTEXT()																						\
			asm volatile(	"MOV	_pxCurrentTCB, W0		\n"	/* Restore the stack pointer for the task. */				\
							"MOV	[W0], W15				\n"																\
							"POP	W0						\n"	/* Restore the critical nesting counter for the task. */	\
							"MOV	W0, _uxCriticalNesting	\n"																\
							"POP	PSVPAG					\n"																\
							"POP	CORCON					\n"																\
							"POP	TBLPAG					\n"																\
							"POP	RCOUNT					\n"	/* Restore the registers from the stack. */					\
							"POP	W14						\n"																\
							"POP.D	W12						\n"																\
							"POP.D	W10						\n"																\
							"POP.D	W8						\n"																\
							"POP.D	W6						\n"																\
							"POP.D	W4						\n"																\
							"POP.D	W2						\n"																\
							"POP.D	W0						\n"																\
							"POP	SR						  " );
		#endif /* __HAS_EDS__ */
#endif /* MPLAB_PIC24_PORT */

#ifdef MPLAB_DSPIC_PORT

	#define portRESTORE_CONTEXT()																						\
		asm volatile(	"MOV	_pxCurrentTCB, W0		\n"	/* Restore the stack pointer for the task. */				\
						"MOV	[W0], W15				\n"																\
						"POP	W0						\n"	/* Restore the critical nesting counter for the task. */	\
						"MOV	W0, _uxCriticalNesting	\n"																\
						"POP	PSVPAG					\n"																\
						"POP	CORCON					\n"																\
						"POP	DOENDH					\n"																\
						"POP	DOENDL					\n"																\
						"POP	DOSTARTH				\n"																\
						"POP	DOSTARTL				\n"																\
						"POP	DCOUNT					\n"																\
						"POP	ACCBU					\n"																\
						"POP	ACCBH					\n"																\
						"POP	ACCBL					\n"																\
						"POP	ACCAU					\n"																\
						"POP	ACCAH					\n"																\
						"POP	ACCAL					\n"																\
						"POP	TBLPAG					\n"																\
						"POP	RCOUNT					\n"	/* Restore the registers from the stack. */					\
						"POP	W14						\n"																\
						"POP.D	W12						\n"																\
						"POP.D	W10						\n"																\
						"POP.D	W8						\n"																\
						"POP.D	W6						\n"																\
						"POP.D	W4						\n"																\
						"POP.D	W2						\n"																\
						"POP.D	W0						\n"																\
						"POP	SR						  " );

#endif /* MPLAB_DSPIC_PORT */

/*
 * Setup the timer used to generate the tick interrupt.
 */
static void prvSetupTimerInterrupt( void );

/*
 * See header file for description.
 */
portSTACK_TYPE *pxPortInitialiseStack( portSTACK_TYPE *pxTopOfStack, pdTASK_CODE pxCode, void *pvParameters )
{
unsigned short usCode;
portBASE_TYPE i;

const portSTACK_TYPE xInitialStack[] =
{
	0x1111,	/* W1 */
	0x2222, /* W2 */
	0x3333, /* W3 */
	0x4444, /* W4 */
	0x5555, /* W5 */
	0x6666, /* W6 */
	0x7777, /* W7 */
	0x8888, /* W8 */
	0x9999, /* W9 */
	0xaaaa, /* W10 */
	0xbbbb, /* W11 */
	0xcccc, /* W12 */
	0xdddd, /* W13 */
	0xeeee, /* W14 */
	0xcdce, /* RCOUNT */
	0xabac, /* TBLPAG */

	/* dsPIC specific registers. */
	#ifdef MPLAB_DSPIC_PORT
		0x0202, /* ACCAL */
		0x0303, /* ACCAH */
		0x0404, /* ACCAU */
		0x0505, /* ACCBL */
		0x0606, /* ACCBH */
		0x0707, /* ACCBU */
		0x0808, /* DCOUNT */
		0x090a, /* DOSTARTL */
		0x1010, /* DOSTARTH */
		0x1110, /* DOENDL */
		0x1212, /* DOENDH */
	#endif
};

	/* Setup the stack as if a yield had occurred.

	Save the low bytes of the program counter. */
	usCode = ( unsigned short ) pxCode;
	*pxTopOfStack = ( portSTACK_TYPE ) usCode;
	pxTopOfStack++;

	/* Save the high byte of the program counter.  This will always be zero
	here as it is passed in a 16bit pointer.  If the address is greater than
	16 bits then the pointer will point to a jump table. */
	*pxTopOfStack = ( portSTACK_TYPE ) 0;
	pxTopOfStack++;

	/* Status register with interrupts enabled. */
	*pxTopOfStack = portINITIAL_SR;
	pxTopOfStack++;

	/* Parameters are passed in W0. */
	*pxTopOfStack = ( portSTACK_TYPE ) pvParameters;
	pxTopOfStack++;

	for( i = 0; i < ( sizeof( xInitialStack ) / sizeof( portSTACK_TYPE ) ); i++ )
	{
		*pxTopOfStack = xInitialStack[ i ];
		pxTopOfStack++;
	}

	*pxTopOfStack = CORCON;
	pxTopOfStack++;

	#if defined(__HAS_EDS__)
		*pxTopOfStack = DSRPAG;
		pxTopOfStack++;
		*pxTopOfStack = DSWPAG;
		pxTopOfStack++;
	#else /* __HAS_EDS__ */
		*pxTopOfStack = PSVPAG;
		pxTopOfStack++;
	#endif /* __HAS_EDS__ */

	/* Finally the critical nesting depth. */
	*pxTopOfStack = 0x00;
	pxTopOfStack++;

	return pxTopOfStack;
}
/*-----------------------------------------------------------*/

portBASE_TYPE xPortStartScheduler( void )
{
	/* Setup a timer for the tick ISR. */
	prvSetupTimerInterrupt();

	/* Restore the context of the first task to run. */
	portRESTORE_CONTEXT();

	/* Simulate the end of the yield function. */
	asm volatile ( "return" );

	/* Should not reach here. */
	return pdTRUE;
}
/*-----------------------------------------------------------*/

void vPortEndScheduler( void )
{
	/* It is unlikely that the scheduler for the PIC port will get stopped
	once running.  If required disable the tick interrupt here, then return
	to xPortStartScheduler(). */
}
/*-----------------------------------------------------------*/

/*
 * Setup a timer for a regular tick.
 */
static void prvSetupTimerInterrupt( void )
{
const unsigned long ulCompareMatch = ( ( configCPU_CLOCK_HZ / portTIMER_PRESCALE ) / configTICK_RATE_HZ ) - 1;

	/* Prescale of 8. */
	T1CON = 0;
	TMR1 = 0;

	PR1 = ( unsigned short ) ulCompareMatch;

	/* Setup timer 1 interrupt priority. */
	IPC0bits.T1IP = configKERNEL_INTERRUPT_PRIORITY;

	/* Clear the interrupt as a starting condition. */
	IFS0bits.T1IF = 0;

	/* Enable the interrupt. */
	IEC0bits.T1IE = 1;

	/* Setup the prescale value. */
	T1CONbits.TCKPS0 = 1;
	T1CONbits.TCKPS1 = 0;

	/* Start the timer. */
	T1CONbits.TON = 1;
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

void __attribute__((__interrupt__, auto_psv)) _T1Interrupt( void )
{
	/* Clear the timer interrupt. */
	IFS0bits.T1IF = 0;

	vTaskIncrementTick();

	#if configUSE_PREEMPTION == 1
		portYIELD();
	#endif
}
