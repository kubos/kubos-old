/*
    FreeRTOS V7.4.2 - Copyright (C) 2013 Real Time Engineers Ltd.

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

    >>>>>>NOTE<<<<<< The modification to the GPL is included to allow you to
    distribute a combined work that includes FreeRTOS without being obliged to
    provide the source code for proprietary components outside of the FreeRTOS
    kernel.

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details. You should have received a copy of the GNU General Public License
    and the FreeRTOS license exception along with FreeRTOS; if not it can be
    viewed here: http://www.freertos.org/a00114.html and also obtained by
    writing to Real Time Engineers Ltd., contact details for whom are available
    on the FreeRTOS WEB site.

    1 tab == 4 spaces!

    ***************************************************************************
     *                                                                       *
     *    Having a problem?  Start by reading the FAQ "My application does   *
     *    not run, what could be wrong?"                                     *
     *                                                                       *
     *    http://www.FreeRTOS.org/FAQHelp.html                               *
     *                                                                       *
    ***************************************************************************


    http://www.FreeRTOS.org - Documentation, books, training, latest versions, 
    license and Real Time Engineers Ltd. contact details.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, and our new
    fully thread aware and reentrant UDP/IP stack.

    http://www.OpenRTOS.com - Real Time Engineers ltd license FreeRTOS to High 
    Integrity Systems, who sell the code with commercial support, 
    indemnification and middleware, under the OpenRTOS brand.
    
    http://www.SafeRTOS.com - High Integrity Systems also provide a safety 
    engineered and independently SIL3 certified version for use in safety and 
    mission critical applications that require provable dependability.
*/

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"

/*-----------------------------------------------------------
 * Implementation of functions defined in portable.h for the MSP430X port.
 *----------------------------------------------------------*/

/* Constants required for hardware setup.  The tick ISR runs off the ACLK,
not the MCLK. */
#define portACLK_FREQUENCY_HZ			( ( portTickType ) 32768 )
#define portINITIAL_CRITICAL_NESTING	( ( unsigned short ) 10 )
#define portFLAGS_INT_ENABLED			( ( portSTACK_TYPE ) 0x08 )

/* We require the address of the pxCurrentTCB variable, but don't want to know
any details of its type. */
typedef void tskTCB;
extern volatile tskTCB * volatile pxCurrentTCB;

/* Each task maintains a count of the critical section nesting depth.  Each
time a critical section is entered the count is incremented.  Each time a
critical section is exited the count is decremented - with interrupts only
being re-enabled if the count is zero.

usCriticalNesting will get set to zero when the scheduler starts, but must
not be initialised to zero as this will cause problems during the startup
sequence. */
volatile unsigned short usCriticalNesting = portINITIAL_CRITICAL_NESTING;
/*-----------------------------------------------------------*/


/*
 * Sets up the periodic ISR used for the RTOS tick.  This uses timer 0, but
 * could have alternatively used the watchdog timer or timer 1.
 */
void vPortSetupTimerInterrupt( void );
/*-----------------------------------------------------------*/

/*
 * Initialise the stack of a task to look exactly as if a call to
 * portSAVE_CONTEXT had been called.
 *
 * See the header file portable.h.
 */
portSTACK_TYPE *pxPortInitialiseStack( portSTACK_TYPE *pxTopOfStack, pdTASK_CODE pxCode, void *pvParameters )
{
unsigned short *pusTopOfStack;
unsigned long *pulTopOfStack, ulTemp;

	/*
		Place a few bytes of known values on the bottom of the stack.
		This is just useful for debugging and can be included if required.

		*pxTopOfStack = ( portSTACK_TYPE ) 0x1111;
		pxTopOfStack--;
		*pxTopOfStack = ( portSTACK_TYPE ) 0x2222;
		pxTopOfStack--;
		*pxTopOfStack = ( portSTACK_TYPE ) 0x3333;
		pxTopOfStack--;
	*/

	/* Data types are need either 16 bits or 32 bits depending on the data 
	and code model used. */
	if( sizeof( pxCode ) == sizeof( unsigned short ) )
	{
		pusTopOfStack = ( unsigned short * ) pxTopOfStack;
		ulTemp = ( unsigned long ) pxCode;
		*pusTopOfStack = ( unsigned short ) ulTemp;
	}
	else
	{
		/* Make room for a 20 bit value stored as a 32 bit value. */
		pusTopOfStack = ( unsigned short * ) pxTopOfStack;		
		pusTopOfStack--;
		pulTopOfStack = ( unsigned long * ) pusTopOfStack;
		*pulTopOfStack = ( unsigned long ) pxCode;
	}

	pusTopOfStack--;
	*pusTopOfStack = portFLAGS_INT_ENABLED;
	pusTopOfStack -= ( sizeof( portSTACK_TYPE ) / 2 );
	
	/* From here on the size of stacked items depends on the memory model. */
	pxTopOfStack = ( portSTACK_TYPE * ) pusTopOfStack;

	/* Next the general purpose registers. */
	#ifdef PRELOAD_REGISTER_VALUES
		*pxTopOfStack = ( portSTACK_TYPE ) 0xffff;
		pxTopOfStack--;
		*pxTopOfStack = ( portSTACK_TYPE ) 0xeeee;
		pxTopOfStack--;
		*pxTopOfStack = ( portSTACK_TYPE ) 0xdddd;
		pxTopOfStack--;
		*pxTopOfStack = ( portSTACK_TYPE ) pvParameters;
		pxTopOfStack--;
		*pxTopOfStack = ( portSTACK_TYPE ) 0xbbbb;
		pxTopOfStack--;
		*pxTopOfStack = ( portSTACK_TYPE ) 0xaaaa;
		pxTopOfStack--;
		*pxTopOfStack = ( portSTACK_TYPE ) 0x9999;
		pxTopOfStack--;
		*pxTopOfStack = ( portSTACK_TYPE ) 0x8888;
		pxTopOfStack--;	
		*pxTopOfStack = ( portSTACK_TYPE ) 0x5555;
		pxTopOfStack--;
		*pxTopOfStack = ( portSTACK_TYPE ) 0x6666;
		pxTopOfStack--;
		*pxTopOfStack = ( portSTACK_TYPE ) 0x5555;
		pxTopOfStack--;
		*pxTopOfStack = ( portSTACK_TYPE ) 0x4444;
		pxTopOfStack--;
	#else
		pxTopOfStack -= 3;
		*pxTopOfStack = ( portSTACK_TYPE ) pvParameters;
		pxTopOfStack -= 9;
	#endif

	/* A variable is used to keep track of the critical section nesting.
	This variable has to be stored as part of the task context and is
	initially set to zero. */
	*pxTopOfStack = ( portSTACK_TYPE ) portNO_CRITICAL_SECTION_NESTING;	

	/* Return a pointer to the top of the stack we have generated so this can
	be stored in the task control block for the task. */
	return pxTopOfStack;
}
/*-----------------------------------------------------------*/

void vPortEndScheduler( void )
{
	/* It is unlikely that the MSP430 port will get stopped.  If required simply
	disable the tick interrupt here. */
}
/*-----------------------------------------------------------*/

/*
 * Hardware initialisation to generate the RTOS tick.
 */
void vPortSetupTimerInterrupt( void )
{
	vApplicationSetupTimerInterrupt();
}
/*-----------------------------------------------------------*/

#pragma vector=configTICK_VECTOR
interrupt void vTickISREntry( void )
{
extern void vPortTickISR( void );

	__bic_SR_register_on_exit( SCG1 + SCG0 + OSCOFF + CPUOFF );
	#if configUSE_PREEMPTION == 1
		extern void vPortPreemptiveTickISR( void );
		vPortPreemptiveTickISR();
	#else
		extern void vPortCooperativeTickISR( void );
		vPortCooperativeTickISR();
	#endif
}

	
