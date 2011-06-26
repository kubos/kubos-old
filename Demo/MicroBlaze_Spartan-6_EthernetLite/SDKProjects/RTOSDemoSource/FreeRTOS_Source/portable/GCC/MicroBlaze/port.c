/*
    FreeRTOS V7.0.1 - Copyright (C) 2011 Real Time Engineers Ltd.
	

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

    http://www.FreeRTOS.org - Documentation, latest information, license and
    contact details.

    http://www.SafeRTOS.com - A version that is certified for use in safety
    critical systems.

    http://www.OpenRTOS.com - Commercial support, development, porting,
    licensing and training services.
*/

/*-----------------------------------------------------------
 * Implementation of functions defined in portable.h for the MicroBlaze port.
 *----------------------------------------------------------*/


/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Standard includes. */
#include <string.h>

/* Hardware includes. */
#include <xintc_i.h>
#include <xil_exception.h>
#include <microblaze_exceptions_g.h>

/* Tasks are started with a critical section nesting of 0 - however prior
to the scheduler being commenced we don't want the critical nesting level
to reach zero, so it is initialised to a high value. */
#define portINITIAL_NESTING_VALUE	( 0xff )

/* The bit within the MSR register that enabled/disables interrupts. */
#define portMSR_IE					( 0x02U )

#define portINITIAL_FSR				( 0U )
/*-----------------------------------------------------------*/

/*
 * Initialise the interrupt controller instance.
 */
static portBASE_TYPE prvInitialiseInterruptController( void );

/*
 * Call an application provided callback to set up the periodic interrupt used
 * for the RTOS tick.  Using an application callback allows the application
 * writer to decide
 */
extern void vApplicationSetupTimerInterrupt( void );
/*-----------------------------------------------------------*/

/* Counts the nesting depth of calls to portENTER_CRITICAL().  Each task 
maintains it's own count, so this variable is saved as part of the task
context. */
volatile unsigned portBASE_TYPE uxCriticalNesting = portINITIAL_NESTING_VALUE;

/* To limit the amount of stack required by each task, this port uses a
separate stack for interrupts. */
unsigned long *pulISRStack;

/* If an interrupt requests a context switch then ulTaskSwitchRequested will
get set to 1, which in turn will cause vTaskSwitchContext() to be called
prior to a task context getting restored on exit from the interrupt.  This
mechanism is used as a single interrupt can cause multiple peripherals handlers
to get called, and vTaskSwitchContext() should not get called in each handler. */
volatile unsigned long ulTaskSwitchRequested = 0UL;

/* The instance of the interrupt controller used by this port. */
static XIntc xInterruptControllerInstance;

/*-----------------------------------------------------------*/

/* 
 * Initialise the stack of a task to look exactly as if a call to 
 * portSAVE_CONTEXT had been made.
 * 
 * See the header file portable.h.
 */
portSTACK_TYPE *pxPortInitialiseStack( portSTACK_TYPE *pxTopOfStack, pdTASK_CODE pxCode, void *pvParameters )
{
extern void *_SDA2_BASE_, *_SDA_BASE_;
const unsigned long ulR2 = ( unsigned long ) &_SDA2_BASE_;
const unsigned long ulR13 = ( unsigned long ) &_SDA_BASE_;

	/* Place a few bytes of known values on the bottom of the stack. 
	This is essential for the Microblaze port and these lines must
	not be omitted.  The parameter value will overwrite the 
	0x22222222 value during the function prologue. */
	*pxTopOfStack = ( portSTACK_TYPE ) 0x11111111;
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x22222222;
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x33333333;
	pxTopOfStack--; 

	/* The debugger will look at the previous stack frame. */
	*pxTopOfStack = ( portSTACK_TYPE ) 0x00000000;
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x00000000;
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x00000000;
	pxTopOfStack--;

	#if XPAR_MICROBLAZE_0_USE_FPU == 1
		/* The FSR value placed in the initial task context is just 0. */
		*pxTopOfStack = portINITIAL_FSR;
		pxTopOfStack--;
	#endif

	/* The MSR value placed in the initial task context should have interrupts
	disabled.  Each task will enable interrupts automatically when it enters
	the running state for the first time. */
	*pxTopOfStack = mfmsr() & ~portMSR_IE;
	pxTopOfStack--;

	/* First stack an initial value for the critical section nesting.  This
	is initialised to zero. */
	*pxTopOfStack = ( portSTACK_TYPE ) 0x00;
	
	/* R0 is always zero. */
	/* R1 is the SP. */

	/* Place an initial value for all the general purpose registers. */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) ulR2;	/* R2 - read only small data area. */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x03;	/* R3 - return values and temporaries. */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x04;	/* R4 - return values and temporaries. */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) pvParameters;/* R5 contains the function call parameters. */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x06;	/* R6 - other parameters and temporaries.  Used as the return address from vPortTaskEntryPoint. */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x07;	/* R7 - other parameters and temporaries. */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x08;	/* R8 - other parameters and temporaries. */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x09;	/* R9 - other parameters and temporaries. */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x0a;	/* R10 - other parameters and temporaries. */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x0b;	/* R11 - temporaries. */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x0c;	/* R12 - temporaries. */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) ulR13;	/* R13 - read/write small data area. */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) pxCode;	/* R14 - return address for interrupt. */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) NULL;	/* R15 - return address for subroutine. */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x10;	/* R16 - return address for trap (debugger). */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x11;	/* R17 - return address for exceptions, if configured. */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x12;	/* R18 - reserved for assembler and compiler temporaries. */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x00;	/* R19 - must be saved across function calls. Callee-save.  Seems to be interpreted as the frame pointer. */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x14;	/* R20 - reserved for storing a pointer to the Global Offset Table (GOT) in Position Independent Code (PIC). Non-volatile in non-PIC code. Must be saved across function calls. Callee-save.  Not used by FreeRTOS. */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x15;	/* R21 - must be saved across function calls. Callee-save. */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x16;	/* R22 - must be saved across function calls. Callee-save. */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x17;	/* R23 - must be saved across function calls. Callee-save. */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x18;	/* R24 - must be saved across function calls. Callee-save. */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x19;	/* R25 - must be saved across function calls. Callee-save. */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x1a;	/* R26 - must be saved across function calls. Callee-save. */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x1b;	/* R27 - must be saved across function calls. Callee-save. */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x1c;	/* R28 - must be saved across function calls. Callee-save. */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x1d;	/* R29 - must be saved across function calls. Callee-save. */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x1e;	/* R30 - must be saved across function calls. Callee-save. */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x1f;	/* R31 - must be saved across function calls. Callee-save. */
	pxTopOfStack--;

	/* Return a pointer to the top of the stack we have generated so this can
	be stored in the task control block for the task. */
	return pxTopOfStack;
}
/*-----------------------------------------------------------*/

portBASE_TYPE xPortStartScheduler( void )
{
extern void ( vPortStartFirstTask )( void );
extern unsigned long _stack[];

	/* Setup the hardware to generate the tick.  Interrupts are disabled when
	this function is called. */
	vApplicationSetupTimerInterrupt();

	/* Reuse the stack from main as the stack for the interrupts/exceptions. */
	pulISRStack = ( unsigned long * ) _stack;

	/* Restore the context of the first task that is going to run.  From here
	on, the created tasks will be executing. */
	vPortStartFirstTask();

	/* Should not get here as the tasks are now running! */
	return pdFALSE;
}
/*-----------------------------------------------------------*/

void vPortEndScheduler( void )
{
	/* Not implemented. */
}
/*-----------------------------------------------------------*/

/*
 * Manual context switch called by portYIELD or taskYIELD.  
 */
void vPortYield( void )
{
extern void VPortYieldASM( void );

	/* Perform the context switch in a critical section to assure it is
	not interrupted by the tick ISR.  It is not a problem to do this as
	each task maintains it's own interrupt status. */
	portENTER_CRITICAL();
	{
		/* Jump directly to the yield function to ensure there is no
		compiler generated prologue code. */
		asm volatile (	"bralid r14, VPortYieldASM		\n\t" \
						"or r0, r0, r0					\n\t" );
	}
	portEXIT_CRITICAL();
}
/*-----------------------------------------------------------*/

void vPortEnableInterrupt( unsigned char ucInterruptID )
{
	XIntc_Enable( &xInterruptControllerInstance, ucInterruptID );
}
/*-----------------------------------------------------------*/

void vPortDisableInterrupt( unsigned char ucInterruptID )
{
	XIntc_Disable( &xInterruptControllerInstance, ucInterruptID );
}
/*-----------------------------------------------------------*/

portBASE_TYPE xPortInstallInterruptHandler( unsigned char ucInterruptID, XInterruptHandler pxHandler, void *pvCallBackRef )
{
static portBASE_TYPE xInterruptControllerInitialised = pdFALSE;
portBASE_TYPE xReturn = XST_SUCCESS;

	if( xInterruptControllerInitialised != pdTRUE )
	{
		xReturn = prvInitialiseInterruptController();
		xInterruptControllerInitialised = pdTRUE;
	}

	if( xReturn == XST_SUCCESS )
	{
		xReturn = XIntc_Connect( &xInterruptControllerInstance, ucInterruptID, pxHandler, pvCallBackRef );
	}

	if( xReturn == XST_SUCCESS )
	{
		xReturn = pdPASS;
	}

	return xReturn;
}
/*-----------------------------------------------------------*/

/* 
 * Handler for the timer interrupt.
 */
void vTickISR( void *pvUnused )
{
extern void vApplicationClearTimerInterrupt( void );

	/* Ensure the unused parameter does not generate a compiler warning. */
	( void ) pvUnused;

	vApplicationClearTimerInterrupt();

	/* Increment the RTOS tick - this might cause a task to unblock. */
	vTaskIncrementTick();

	/* If we are using the preemptive scheduler then we also need to determine
	if this tick should cause a context switch. */
	#if configUSE_PREEMPTION == 1
		/* Force vTaskSwitchContext() to be called as the interrupt exits. */
		ulTaskSwitchRequested = 1;
	#endif
}
/*-----------------------------------------------------------*/

static portBASE_TYPE prvInitialiseInterruptController( void )
{
portBASE_TYPE xStatus;

	xStatus = XIntc_Initialize( &xInterruptControllerInstance, configINTERRUPT_CONTROLLER_TO_USE );

	if( xStatus == XST_SUCCESS )
	{
		/* Initialise the exception table. */
		Xil_ExceptionInit();

	    /* Service all pending interrupts each time the handler is entered. */
	    XIntc_SetIntrSvcOption( xInterruptControllerInstance.BaseAddress, XIN_SVC_ALL_ISRS_OPTION );

	    /* Install exception handlers if the MicroBlaze is configured to handle
	    exceptions, and the application defined constant
	    configINSTALL_EXCEPTION_HANDLERS is set to 1. */
		#if ( MICROBLAZE_EXCEPTIONS_ENABLED == 1 ) && ( configINSTALL_EXCEPTION_HANDLERS == 1 )
	    {
	    	vPortExceptionsInstallHandlers();
	    }
		#endif /* MICROBLAZE_EXCEPTIONS_ENABLED */

		/* Start the interrupt controller.  Interrupts are enabled when the
		scheduler starts. */
		xStatus = XIntc_Start( &xInterruptControllerInstance, XIN_REAL_MODE );

		/* Ensure the compiler does not generate warnings for the unused
		iStatus valud if configASSERT() is not defined. */
		( void ) xStatus;
	}

	configASSERT( ( xStatus == ( portBASE_TYPE ) XST_SUCCESS ) )

/*_RB_ Exception test code. */
#if 0
This does not cause the bralid address to be in the r17 register.
__asm volatile (
					"bralid r15, 1234 \n"
					"or r0, r0, r0 \n"
				);
#endif
#if 0
	xStatus /= 0;
#endif

	return xStatus;
}
/*-----------------------------------------------------------*/


