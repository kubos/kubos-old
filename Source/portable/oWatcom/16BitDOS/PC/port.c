/*
    FreeRTOS V6.1.0 - Copyright (C) 2010 Real Time Engineers Ltd.

    ***************************************************************************
    *                                                                         *
    * If you are:                                                             *
    *                                                                         *
    *    + New to FreeRTOS,                                                   *
    *    + Wanting to learn FreeRTOS or multitasking in general quickly       *
    *    + Looking for basic training,                                        *
    *    + Wanting to improve your FreeRTOS skills and productivity           *
    *                                                                         *
    * then take a look at the FreeRTOS books - available as PDF or paperback  *
    *                                                                         *
    *        "Using the FreeRTOS Real Time Kernel - a Practical Guide"        *
    *                  http://www.FreeRTOS.org/Documentation                  *
    *                                                                         *
    * A pdf reference manual is also available.  Both are usually delivered   *
    * to your inbox within 20 minutes to two hours when purchased between 8am *
    * and 8pm GMT (although please allow up to 24 hours in case of            *
    * exceptional circumstances).  Thank you for your support!                *
    *                                                                         *
    ***************************************************************************

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation AND MODIFIED BY the FreeRTOS exception.
    ***NOTE*** The exception to the GPL is included to allow you to distribute
    a combined work that includes FreeRTOS without being obliged to provide the
    source code for proprietary components outside of the FreeRTOS kernel.
    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
    more details. You should have received a copy of the GNU General Public 
    License and the FreeRTOS license exception along with FreeRTOS; if not it 
    can be viewed here: http://www.freertos.org/a00114.html and also obtained 
    by writing to Richard Barry, contact details for whom are available on the
    FreeRTOS WEB site.

    1 tab == 4 spaces!

    http://www.FreeRTOS.org - Documentation, latest information, license and
    contact details.

    http://www.SafeRTOS.com - A version that is certified for use in safety
    critical systems.

    http://www.OpenRTOS.com - Commercial support, development, porting,
    licensing and training services.
*/

/*
Changes from V1.00:
	
	+ Call to taskYIELD() from within tick ISR has been replaced by the more
	  efficient portSWITCH_CONTEXT().
	+ ISR function definitions renamed to include the prv prefix.

Changes from V1.2.0:

	+ prvPortResetPIC() is now called last thing before the end of the 
	  preemptive tick routine.

Changes from V2.6.1

	+ Replaced the sUsingPreemption variable with the configUSE_PREEMPTION
	  macro to be consistent with the later ports.

Changes from V4.0.1
	
	+ Add function prvSetTickFrequencyDefault() to set the DOS tick back to
	  its proper value when the scheduler exits. 
*/

#include <stdlib.h>
#include <stdio.h>
#include <i86.h>
#include <dos.h>
#include <setjmp.h>

#include "FreeRTOS.h"
#include "task.h"
#include "portasm.h"

/*-----------------------------------------------------------
 * Implementation of functions defined in portable.h for the industrial
 * PC port.
 *----------------------------------------------------------*/

/*lint -e950 Non ANSI reserved words okay in this file only. */

#define portTIMER_INT_NUMBER	0x08

/* Setup hardware for required tick interrupt rate. */
static void prvSetTickFrequency( unsigned long ulTickRateHz );

/* Restore hardware to as it was prior to starting the scheduler. */
static void prvExitFunction( void );

/* Either chain to the DOS tick (which itself clears the PIC) or clear the PIC
directly.  We chain to the DOS tick as close as possible to the standard DOS
tick rate. */
static void prvPortResetPIC( void );

/* The tick ISR used depends on whether the preemptive or cooperative scheduler
is being used. */
#if configUSE_PREEMPTION == 1
	/* Tick service routine used by the scheduler when preemptive scheduling is
	being used. */
	static void __interrupt __far prvPreemptiveTick( void );
#else
	/* Tick service routine used by the scheduler when cooperative scheduling is 
	being used. */
	static void __interrupt __far prvNonPreemptiveTick( void );
#endif
/* Trap routine used by taskYIELD() to manually cause a context switch. */
static void __interrupt __far prvYieldProcessor( void );

/* Set the tick frequency back so the floppy drive works correctly when the
scheduler exits. */
static void prvSetTickFrequencyDefault( void );

/*lint -e956 File scopes necessary here. */

/* Used to signal when to chain to the DOS tick, and when to just clear the PIC ourselves. */
static short sDOSTickCounter;							

/* Set true when the vectors are set so the scheduler will service the tick. */
static short sSchedulerRunning = pdFALSE;				

/* Points to the original routine installed on the vector we use for manual context switches.  This is then used to restore the original routine during prvExitFunction(). */
static void ( __interrupt __far *pxOldSwitchISR )();		

/* Points to the original routine installed on the vector we use to chain to the DOS tick.  This is then used to restore the original routine during prvExitFunction(). */
static void ( __interrupt __far *pxOldSwitchISRPlus1 )();	

/* Used to restore the original DOS context when the scheduler is ended. */
static jmp_buf xJumpBuf;

/*lint +e956 */

/*-----------------------------------------------------------*/
portBASE_TYPE xPortStartScheduler( void )
{
pxISR pxOriginalTickISR;
	
	/* This is called with interrupts already disabled. */

	/* Remember what was on the interrupts we are going to use
	so we can put them back later if required. */
	pxOldSwitchISR = _dos_getvect( portSWITCH_INT_NUMBER );
	pxOriginalTickISR = _dos_getvect( portTIMER_INT_NUMBER );
	pxOldSwitchISRPlus1 = _dos_getvect( portSWITCH_INT_NUMBER + 1 );

	prvSetTickFrequency( configTICK_RATE_HZ );

	/* Put our manual switch (yield) function on a known
	vector. */
	_dos_setvect( portSWITCH_INT_NUMBER, prvYieldProcessor );

	/* Put the old tick on a different interrupt number so we can
	call it when we want. */
	_dos_setvect( portSWITCH_INT_NUMBER + 1, pxOriginalTickISR );

	#if configUSE_PREEMPTION == 1
	{		
		/* Put our tick switch function on the timer interrupt. */
		_dos_setvect( portTIMER_INT_NUMBER, prvPreemptiveTick );
	}
	#else
	{
		/* We want the timer interrupt to just increment the tick count. */
		_dos_setvect( portTIMER_INT_NUMBER, prvNonPreemptiveTick );
	}
	#endif

	/* Setup a counter that is used to call the DOS interrupt as close
	to it's original frequency as can be achieved given our chosen tick
	frequency. */
	sDOSTickCounter = portTICKS_PER_DOS_TICK;

	/* Clean up function if we want to return to DOS. */
	if( setjmp( xJumpBuf ) != 0 )
	{
		prvExitFunction();
		sSchedulerRunning = pdFALSE;
	}
	else
	{
		sSchedulerRunning = pdTRUE;

		/* Kick off the scheduler by setting up the context of the first task. */
		portFIRST_CONTEXT();
	}

	return sSchedulerRunning;
}
/*-----------------------------------------------------------*/

/* The tick ISR used depends on whether the preemptive or cooperative scheduler
is being used. */
#if configUSE_PREEMPTION == 1
	/* Tick service routine used by the scheduler when preemptive scheduling is
	being used. */
	static void __interrupt __far prvPreemptiveTick( void )
	{
		/* Get the scheduler to update the task states following the tick. */
		vTaskIncrementTick();

		/* Switch in the context of the next task to be run. */
		portSWITCH_CONTEXT();

		/* Reset the PIC ready for the next time. */
		prvPortResetPIC();
	}
#else
	static void __interrupt __far prvNonPreemptiveTick( void )
	{
		/* Same as preemptive tick, but the cooperative scheduler is being used
		so we don't have to switch in the context of the next task. */
		vTaskIncrementTick();
		prvPortResetPIC();
	}
#endif
/*-----------------------------------------------------------*/


static void __interrupt __far prvYieldProcessor( void )
{
	/* Switch in the context of the next task to be run. */
	portSWITCH_CONTEXT();
}
/*-----------------------------------------------------------*/

static void prvPortResetPIC( void )
{
	/* We are going to call the DOS tick interrupt at as close a
	frequency to the normal DOS tick as possible. */

	/* WE SHOULD NOT DO THIS IF YIELD WAS CALLED. */
	--sDOSTickCounter;
	if( sDOSTickCounter <= 0 )
	{
		sDOSTickCounter = ( short ) portTICKS_PER_DOS_TICK;
		__asm{ int	portSWITCH_INT_NUMBER + 1 };		 
	}
	else
	{
		/* Reset the PIC as the DOS tick is not being called to
		do it. */
		__asm
		{
			mov	al, 20H
			out 20H, al
		};
	}
}
/*-----------------------------------------------------------*/

void vPortEndScheduler( void )
{
	/* Jump back to the processor state prior to starting the
	scheduler.  This means we are not going to be using a
	task stack frame so the task can be deleted. */
	longjmp( xJumpBuf, 1 );
}
/*-----------------------------------------------------------*/

static void prvExitFunction( void )
{
void ( __interrupt __far *pxOriginalTickISR )();

	/* Interrupts should be disabled here anyway - but no 
	harm in making sure. */
	portDISABLE_INTERRUPTS();
	if( sSchedulerRunning == pdTRUE )
	{
		/* Set the DOS tick back onto the timer ticker. */
		pxOriginalTickISR = _dos_getvect( portSWITCH_INT_NUMBER + 1 );
		_dos_setvect( portTIMER_INT_NUMBER, pxOriginalTickISR );
		prvSetTickFrequencyDefault();

		/* Put back the switch interrupt routines that was in place
		before the scheduler started. */
		_dos_setvect( portSWITCH_INT_NUMBER, pxOldSwitchISR );
		_dos_setvect( portSWITCH_INT_NUMBER + 1, pxOldSwitchISRPlus1 );
	}
	/* The tick timer is back how DOS wants it.  We can re-enable
	interrupts without the scheduler being called. */
	portENABLE_INTERRUPTS();

	/* This will free up all the memory used by the scheduler.
	exiting back to dos with INT21 AH=4CH will do this anyway so
	it is not necessary to call this. */
	vTaskCleanUpResources(); 
}
/*-----------------------------------------------------------*/

static void prvSetTickFrequency( unsigned long ulTickRateHz )
{
const unsigned short usPIT_MODE = ( unsigned short ) 0x43;
const unsigned short usPIT0 = ( unsigned short ) 0x40;
const unsigned long ulPIT_CONST = ( unsigned long ) 1193180;
const unsigned short us8254_CTR0_MODE3 = ( unsigned short ) 0x36;
unsigned long ulOutput;

	/* Setup the 8245 to tick at the wanted frequency. */
	portOUTPUT_BYTE( usPIT_MODE, us8254_CTR0_MODE3 );
	ulOutput = ulPIT_CONST / ulTickRateHz;
   
	portOUTPUT_BYTE( usPIT0, ( unsigned short )( ulOutput & ( unsigned long ) 0xff ) );
	ulOutput >>= 8;
	portOUTPUT_BYTE( usPIT0, ( unsigned short ) ( ulOutput & ( unsigned long ) 0xff ) );
}
/*-----------------------------------------------------------*/

static void prvSetTickFrequencyDefault( void )
{
const unsigned short usPIT_MODE = ( unsigned short ) 0x43;
const unsigned short usPIT0 = ( unsigned short ) 0x40;
const unsigned short us8254_CTR0_MODE3 = ( unsigned short ) 0x36;

	portOUTPUT_BYTE( usPIT_MODE, us8254_CTR0_MODE3 );
	portOUTPUT_BYTE( usPIT0,0 );
	portOUTPUT_BYTE( usPIT0,0 );
}


/*lint +e950 */

