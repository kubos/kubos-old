/*
    FreeRTOS V7.5.1 - Copyright (C) 2013 Real Time Engineers Ltd.

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that has become a de facto standard.             *
     *                                                                       *
     *    Help yourself get started quickly and support the FreeRTOS         *
     *    project by purchasing a FreeRTOS tutorial book, reference          *
     *    manual, or both from: http://www.FreeRTOS.org/Documentation        *
     *                                                                       *
     *    Thank you!                                                         *
     *                                                                       *
    ***************************************************************************

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>!AND MODIFIED BY!<< the FreeRTOS exception.

    >>! NOTE: The modification to the GPL is included to allow you to distribute
    >>! a combined work that includes FreeRTOS without being obliged to provide
    >>! the source code for proprietary components outside of the FreeRTOS
    >>! kernel.

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available from the following
    link: http://www.freertos.org/a00114.html

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
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.OpenRTOS.com - Real Time Engineers ltd license FreeRTOS to High
    Integrity Systems to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
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
#include <xintc.h>
#include <xintc_i.h>
#include <xtmrctr.h>

/* Tasks are started with interrupts enabled. */
#define portINITIAL_MSR_STATE		( ( portSTACK_TYPE ) 0x02 )

/* Tasks are started with a critical section nesting of 0 - however prior
to the scheduler being commenced we don't want the critical nesting level
to reach zero, so it is initialised to a high value. */
#define portINITIAL_NESTING_VALUE	( 0xff )

/* Our hardware setup only uses one counter. */
#define portCOUNTER_0 				0

/* The stack used by the ISR is filled with a known value to assist in
debugging. */
#define portISR_STACK_FILL_VALUE	0x55555555

/* Counts the nesting depth of calls to portENTER_CRITICAL().  Each task 
maintains it's own count, so this variable is saved as part of the task
context. */
volatile unsigned portBASE_TYPE uxCriticalNesting = portINITIAL_NESTING_VALUE;

/* To limit the amount of stack required by each task, this port uses a
separate stack for interrupts. */
unsigned long *pulISRStack;

/*-----------------------------------------------------------*/

/*
 * Sets up the periodic ISR used for the RTOS tick.  This uses timer 0, but
 * could have alternatively used the watchdog timer or timer 1.
 */
static void prvSetupTimerInterrupt( void );
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

	/* First stack an initial value for the critical section nesting.  This
	is initialised to zero as tasks are started with interrupts enabled. */
	*pxTopOfStack = ( portSTACK_TYPE ) 0x00;	/* R0. */

	/* Place an initial value for all the general purpose registers. */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) ulR2;	/* R2 - small data area. */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x03;	/* R3. */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x04;	/* R4. */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) pvParameters;/* R5 contains the function call parameters. */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x06;	/* R6. */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x07;	/* R7. */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x08;	/* R8. */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x09;	/* R9. */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x0a;	/* R10. */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x0b;	/* R11. */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x0c;	/* R12. */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) ulR13;	/* R13 - small data read write area. */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) pxCode;	/* R14. */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x0f;	/* R15. */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x10;	/* R16. */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x11;	/* R17. */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x12;	/* R18. */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x13;	/* R19. */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x14;	/* R20. */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x15;	/* R21. */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x16;	/* R22. */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x17;	/* R23. */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x18;	/* R24. */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x19;	/* R25. */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x1a;	/* R26. */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x1b;	/* R27. */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x1c;	/* R28. */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x1d;	/* R29. */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x1e;	/* R30. */
	pxTopOfStack--;

	/* The MSR is stacked between R30 and R31. */
	*pxTopOfStack = portINITIAL_MSR_STATE;
	pxTopOfStack--;

	*pxTopOfStack = ( portSTACK_TYPE ) 0x1f;	/* R31. */
	pxTopOfStack--;

	/* Return a pointer to the top of the stack we have generated so this can
	be stored in the task control block for the task. */
	return pxTopOfStack;
}
/*-----------------------------------------------------------*/

portBASE_TYPE xPortStartScheduler( void )
{
extern void ( __FreeRTOS_interrupt_Handler )( void );
extern void ( vStartFirstTask )( void );


	/* Setup the FreeRTOS interrupt handler.  Code copied from crt0.s. */
	asm volatile ( 	"la	r6, r0, __FreeRTOS_interrupt_handler		\n\t" \
					"sw	r6, r1, r0									\n\t" \
					"lhu r7, r1, r0									\n\t" \
					"shi r7, r0, 0x12								\n\t" \
					"shi r6, r0, 0x16 " );

	/* Setup the hardware to generate the tick.  Interrupts are disabled when
	this function is called. */
	prvSetupTimerInterrupt();

	/* Allocate the stack to be used by the interrupt handler. */
	pulISRStack = ( unsigned long * ) pvPortMalloc( configMINIMAL_STACK_SIZE * sizeof( portSTACK_TYPE ) );

	/* Restore the context of the first task that is going to run. */
	if( pulISRStack != NULL )
	{
		/* Fill the ISR stack with a known value to facilitate debugging. */
		memset( pulISRStack, portISR_STACK_FILL_VALUE, configMINIMAL_STACK_SIZE * sizeof( portSTACK_TYPE ) );
		pulISRStack += ( configMINIMAL_STACK_SIZE - 1 );

		/* Kick off the first task. */
		vStartFirstTask();
	}

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
		/* Jump directly to the yield function to ensure there is no
		compiler generated prologue code. */
		asm volatile (	"bralid r14, VPortYieldASM		\n\t" \
						"or r0, r0, r0					\n\t" );
	portEXIT_CRITICAL();
}
/*-----------------------------------------------------------*/

/*
 * Hardware initialisation to generate the RTOS tick.   
 */
static void prvSetupTimerInterrupt( void )
{
XTmrCtr xTimer;
const unsigned long ulCounterValue = configCPU_CLOCK_HZ / configTICK_RATE_HZ;
unsigned portBASE_TYPE uxMask;

	/* The OPB timer1 is used to generate the tick.  Use the provided library
	functions to enable the timer and set the tick frequency. */
	XTmrCtr_mDisable( XPAR_OPB_TIMER_1_BASEADDR, XPAR_OPB_TIMER_1_DEVICE_ID );
	XTmrCtr_Initialize( &xTimer, XPAR_OPB_TIMER_1_DEVICE_ID );
   	XTmrCtr_mSetLoadReg( XPAR_OPB_TIMER_1_BASEADDR, portCOUNTER_0, ulCounterValue );
	XTmrCtr_mSetControlStatusReg( XPAR_OPB_TIMER_1_BASEADDR, portCOUNTER_0, XTC_CSR_LOAD_MASK | XTC_CSR_INT_OCCURED_MASK );

	/* Set the timer interrupt enable bit while maintaining the other bit 
	states. */
	uxMask = XIntc_In32( ( XPAR_OPB_INTC_0_BASEADDR + XIN_IER_OFFSET ) );
	uxMask |= XPAR_OPB_TIMER_1_INTERRUPT_MASK;
	XIntc_Out32( ( XPAR_OPB_INTC_0_BASEADDR + XIN_IER_OFFSET ), ( uxMask ) );	
	
	XTmrCtr_Start( &xTimer, XPAR_OPB_TIMER_1_DEVICE_ID );
	XTmrCtr_mSetControlStatusReg(XPAR_OPB_TIMER_1_BASEADDR, portCOUNTER_0, XTC_CSR_ENABLE_TMR_MASK | XTC_CSR_ENABLE_INT_MASK | XTC_CSR_AUTO_RELOAD_MASK | XTC_CSR_DOWN_COUNT_MASK | XTC_CSR_INT_OCCURED_MASK );
	XIntc_mAckIntr( XPAR_INTC_SINGLE_BASEADDR, 1 );
}
/*-----------------------------------------------------------*/

/*
 * The interrupt handler placed in the interrupt vector when the scheduler is
 * started.  The task context has already been saved when this is called.
 * This handler determines the interrupt source and calls the relevant 
 * peripheral handler.
 */
void vTaskISRHandler( void )
{
static unsigned long ulPending;    

	/* Which interrupts are pending? */
	ulPending = XIntc_In32( ( XPAR_INTC_SINGLE_BASEADDR + XIN_IVR_OFFSET ) );

	if( ulPending < XPAR_INTC_MAX_NUM_INTR_INPUTS )
	{
		static XIntc_VectorTableEntry *pxTablePtr;
		static XIntc_Config *pxConfig;
		static unsigned long ulInterruptMask;

		ulInterruptMask = ( unsigned long ) 1 << ulPending;

		/* Get the configuration data using the device ID */
		pxConfig = &XIntc_ConfigTable[ ( unsigned long ) XPAR_INTC_SINGLE_DEVICE_ID ];

		pxTablePtr = &( pxConfig->HandlerTable[ ulPending ] );
		if( pxConfig->AckBeforeService & ( ulInterruptMask  ) )
		{
			XIntc_mAckIntr( pxConfig->BaseAddress, ulInterruptMask );
			pxTablePtr->Handler( pxTablePtr->CallBackRef );
		}
		else
		{
			pxTablePtr->Handler( pxTablePtr->CallBackRef );
			XIntc_mAckIntr( pxConfig->BaseAddress, ulInterruptMask );
		}
	}
}
/*-----------------------------------------------------------*/

/* 
 * Handler for the timer interrupt.
 */
void vTickISR( void *pvBaseAddress )
{
unsigned long ulCSR;

	/* Increment the RTOS tick - this might cause a task to unblock. */
	if( xTaskIncrementTick() != pdFALSE )
	{
		vTaskSwitchContext();
	}

	/* Clear the timer interrupt */
	ulCSR = XTmrCtr_mGetControlStatusReg(XPAR_OPB_TIMER_1_BASEADDR, 0);	
	XTmrCtr_mSetControlStatusReg( XPAR_OPB_TIMER_1_BASEADDR, portCOUNTER_0, ulCSR );
}
/*-----------------------------------------------------------*/





