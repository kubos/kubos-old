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

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>

//FILE *pfTraceFile = NULL;
//#define vPortTrace( x ) if( pfTraceFile == NULL ) pfTraceFile = fopen( "c:/temp/trace.txt", "w" ); if( pfTraceFile != NULL ) fprintf( pfTraceFile, x )
#define vPortTrace( x ) ( void ) x

#define portMAX_INTERRUPTS				( ( unsigned long ) sizeof( unsigned long ) * 8UL ) /* The number of bits in an unsigned long. */
#define portNO_CRITICAL_NESTING 		( ( unsigned long ) 0 )

/*
 * Created as a high priority thread, this function uses a timer to simulate
 * a tick interrupt being generated on an embedded target.  In this Windows
 * environment the timer does not achieve anything approaching real time 
 * performance though.
 */
static DWORD WINAPI prvSimulatedPeripheralTimer( LPVOID lpParameter );

/* 
 * Process all the simulated interrupts - each represented by a bit in 
 * ulPendingInterrupts variable.
 */
static void prvProcessPseudoInterrupts( void );

/*-----------------------------------------------------------*/

/* The WIN32 simulator runs each task in a thread.  The context switching is
managed by the threads, so the task stack does not have to be managed directly,
although the task stack is still used to hold an xThreadState structure this is
the only thing it will ever hold.  The structure indirectly maps the task handle 
to a thread handle. */
typedef struct
{
	/* Set to true if the task run by the thread yielded control to the pseudo
	interrupt handler manually - either by yielding or when exiting a critical
	section while pseudo interrupts were pending. */
	long lWaitingInterruptAck;			

	/* Critical nesting count of the task - each task has its own. */
	portSTACK_TYPE ulCriticalNesting;

	/* Handle of the thread that executes the task. */
	void * pvThread;					
} xThreadState;

/* Pseudo interrupts waiting to be processed.  This is a bit mask where each
bit represents one interrupt, so a maximum of 32 interrupts can be simulated. */
static volatile unsigned long ulPendingInterrupts = 0UL;

/* An event used to inform the pseudo interrupt processing thread (a high 
priority thread that simulated interrupt processing) that an interrupt is
pending. */
static void *pvInterruptEvent = NULL;

/* Mutex used to protect all the pseudo interrupt variables that are accessed 
by multiple threads. */
static void *pvInterruptEventMutex = NULL;

/* Events used to manage sequencing. */
static void *pvTickAcknowledgeEvent = NULL, *pvInterruptAcknowledgeEvent = NULL;

/* The critical nesting count for the currently executing task.  This is 
initialised to a non-zero value so interrupts do not become enabled during 
the initialisation phase.  As each task has its own critical nesting value 
ulCriticalNesting will get set to zero when the first task runs.  This 
initialisation is probably not critical in this simulated environment as the
pseudo interrupt handlers do not get created until the FreeRTOS scheduler is 
started anyway. */
static unsigned long ulCriticalNesting = 9999UL;

/* Handlers for all the simulated software interrupts.  The first two positions
are used for the Yield and Tick interrupts so are handled slightly differently,
all the other interrupts can be user defined. */
static void (*vIsrHandler[ portMAX_INTERRUPTS ])( void ) = { 0 };

/* Pointer to the TCB of the currently executing task. */
extern void *pxCurrentTCB;

/*-----------------------------------------------------------*/

static DWORD WINAPI prvSimulatedPeripheralTimer( LPVOID lpParameter )
{
	/* Just to prevent compiler warnings. */
	( void ) lpParameter;

	for(;;)
	{
		vPortTrace( "prvSimulatedPeripheralTimer: Tick acked, re-Sleeping()\r\n" );

		/* Wait until the timer expires and we can access the pseudo interrupt 
		variables.  *NOTE* this is not a 'real time' way of generating tick 
		events as the next wake time should be relative to the previous wake 
		time, not the time that Sleep() is called.  It is done this way to 
		prevent overruns in this very non real time simulated/emulated 
		environment. */
		Sleep( portTICK_RATE_MS );

		vPortTrace( "prvSimulatedPeripheralTimer: Sleep expired, waiting interrupt event mutex\r\n" );
		WaitForSingleObject( pvInterruptEventMutex, INFINITE );
		vPortTrace( "prvSimulatedPeripheralTimer: Got interrupt event mutex\r\n" );
		
		/* The timer has expired, generate the simulated tick event. */
		ulPendingInterrupts |= ( 1 << portINTERRUPT_TICK );

		/* The interrupt is now pending - but should only be processed if
		interrupts are actually enabled. */
		if( ulCriticalNesting == 0UL )
		{
			vPortTrace( "prvSimulatedPeripheralTimer: Setting interrupt event to signal tick\r\n" );
			SetEvent( pvInterruptEvent );

			/* Give back the mutex so the pseudo interrupt handler unblocks 
			and can	access the interrupt handler variables.  This high priority
			task will then loop back round after waiting for the lower priority 
			pseudo interrupt handler thread to acknowledge the tick. */
			vPortTrace( "prvSimulatedPeripheralTimer: Releasing interrupt event mutex so tick can be processed\r\n" );
			SignalObjectAndWait( pvInterruptEventMutex, pvTickAcknowledgeEvent, INFINITE, FALSE );
		}
		else
		{
			ReleaseMutex( pvInterruptEventMutex );
		}
	}
}
/*-----------------------------------------------------------*/

portSTACK_TYPE *pxPortInitialiseStack( portSTACK_TYPE *pxTopOfStack, pdTASK_CODE pxCode, void *pvParameters )
{
xThreadState *pxThreadState = NULL;

	/* In this simulated case a stack is not initialised, but instead a thread
	is created that will execute the task being created.  The thread handles
	the context switching itself.  The xThreadState object is placed onto
	the stack that was created for the task - so the stack buffer is still
	used, just not in the conventional way.  It will not be used for anything
	other than holding this structure. */
	pxThreadState = ( xThreadState * ) ( pxTopOfStack - sizeof( xThreadState ) );

	/* Create the thread itself. */
	pxThreadState->pvThread = ( void * ) CreateThread( NULL, 0, ( LPTHREAD_START_ROUTINE ) pxCode, pvParameters, CREATE_SUSPENDED, NULL );
	SetThreadPriorityBoost( pxThreadState->pvThread, TRUE );
	pxThreadState->ulCriticalNesting = portNO_CRITICAL_NESTING;
	pxThreadState->lWaitingInterruptAck = pdFALSE;
	SetThreadPriority( pxThreadState->pvThread, THREAD_PRIORITY_IDLE );
	
	return ( portSTACK_TYPE * ) pxThreadState;
}
/*-----------------------------------------------------------*/

portBASE_TYPE xPortStartScheduler( void )
{
void *pvHandle;
long lSuccess = pdPASS;
xThreadState *pxThreadState;

	/* Create the events and mutexes that are used to synchronise all the
	threads. */
	pvInterruptEventMutex = CreateMutex( NULL, FALSE, NULL );
	pvInterruptEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
	pvTickAcknowledgeEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
	pvInterruptAcknowledgeEvent = CreateEvent( NULL, FALSE, FALSE, NULL );

	if( ( pvInterruptEventMutex == NULL ) || ( pvInterruptEvent == NULL ) || ( pvTickAcknowledgeEvent == NULL ) || ( pvInterruptAcknowledgeEvent == NULL ) )
	{
		lSuccess = pdFAIL;
	}

	/* Set the priority of this thread such that it is above the priority of 
	the threads that run tasks.  This higher priority is required to ensure
	pseudo interrupts take priority over tasks. */
	SetPriorityClass( GetCurrentProcess(), ABOVE_NORMAL_PRIORITY_CLASS );
	pvHandle = GetCurrentThread();
	if( pvHandle == NULL )
	{
		lSuccess = pdFAIL;
	}
	
	if( lSuccess == pdPASS )
	{
		if( SetThreadPriority( pvHandle, THREAD_PRIORITY_HIGHEST ) == 0 )
		{
			lSuccess = pdFAIL;
		}
		SetThreadPriorityBoost( pvHandle, TRUE );
	}

	if( lSuccess == pdPASS )
	{
		/* Start the thread that simulates the timer peripheral to generate
		tick interrupts. */
		pvHandle = CreateThread( NULL, 0, prvSimulatedPeripheralTimer, NULL, 0, NULL );
		if( pvHandle != NULL )
		{
			SetThreadPriority( pvHandle, THREAD_PRIORITY_HIGHEST );
			SetThreadPriorityBoost( pvHandle, TRUE );
		}
		
		/* Start the highest priority task by obtaining its associated thread 
		state structure, in which is stored the thread handle. */
		pxThreadState = ( xThreadState * ) *( ( unsigned long * ) pxCurrentTCB );
		ulCriticalNesting = portNO_CRITICAL_NESTING;

		vPortTrace( "Created system threads, starting task" );

		/* Bump up the priority of the thread that is going to run, in the
		hope that this will asist in getting the Windows thread scheduler to
		behave as an embedded engineer might expect. */
		SetThreadPriority( pxThreadState->pvThread, THREAD_PRIORITY_ABOVE_NORMAL );
		ResumeThread( pxThreadState->pvThread );

		/* Handle all pseudo interrupts - including yield requests and 
		simulated ticks. */
		prvProcessPseudoInterrupts();
	}	
	
	/* Would not expect to return from prvProcessPseudoInterrupts(), so should 
	not get here. */
	return 0;
}
/*-----------------------------------------------------------*/

static void prvProcessPseudoInterrupts( void )
{
long lSwitchRequired;
xThreadState *pxThreadState;
void *pvObjectList[ 2 ];
unsigned long i;
//char cTraceBuffer[ 256 ];

	vPortTrace( "Entering prvProcessPseudoInterrupts\r\n" );

	/* Going to block on the mutex that ensured exclusive access to the pseudo 
	interrupt objects, and the event that signals that a pseudo interrupt
	should be processed. */
	pvObjectList[ 0 ] = pvInterruptEventMutex;
	pvObjectList[ 1 ] = pvInterruptEvent;

	for(;;)
	{
		vPortTrace( "prvProcessPseudoInterrupts: Waiting for next interrupt event\r\n" );
		WaitForMultipleObjects( sizeof( pvObjectList ) / sizeof( void * ), pvObjectList, TRUE, INFINITE );
		vPortTrace( "prvProcessPseudoInterrupts: Got interrupt event and mutex\r\n" );

		/* Used to indicate whether the pseudo interrupt processing has
		necessitated a context switch to another task/thread. */
		lSwitchRequired = pdFALSE;

		/* For each interrupt we are interested in processing, each of which is
		represented by a bit in the 32bit ulPendingInterrupts variable. */
		for( i = 0; i < portMAX_INTERRUPTS; i++ )
		{
			/* Is the pseudo interrupt pending? */
			if( ulPendingInterrupts & ( 1UL << i ) )
			{
				switch( i )
				{
					case portINTERRUPT_YIELD:

						vPortTrace( "prvProcessPseudoInterrupts: Processing Yield\r\n" );
						lSwitchRequired = pdTRUE;

						/* Clear the interrupt pending bit. */
						ulPendingInterrupts &= ~( 1UL << portINTERRUPT_YIELD );
						break;

					case portINTERRUPT_TICK:
					
						/* Tick interrupts should only be processed if the 
						critical nesting count is zero.  The critical nesting 
						count represents the interrupt mask on real target 
						hardware.  The thread that genereates ticks will not
						actually ask for the tick to be processed unless the
						critical nesting count is zero anyway, but it is 
						possible that a tick is pending when a yield is 
						performed (depending on if the simulation/emulation is
						set up to process yields while within a critical 
						section. */
						vPortTrace( "prvProcessPseudoInterrupts: Processing tick event\r\n" );
						if( ulCriticalNesting == 0UL )
						{
							/* Process the tick itself. */
							vPortTrace( "prvProcessPseudoInterrupts: Incrementing tick\r\n" );
							vTaskIncrementTick();
							#if( configUSE_PREEMPTION != 0 )
							{
								/* A context switch is only automatically 
								performed from the tick	interrupt if the 
								pre-emptive scheduler is being used. */
								lSwitchRequired = pdTRUE;
							}
							#endif
							
							/* Clear the interrupt pending bit. */
							ulPendingInterrupts &= ~( 1UL << portINTERRUPT_TICK );

							vPortTrace( "prvProcessPseudoInterrupts: Acking tick\r\n" );
							SetEvent( pvTickAcknowledgeEvent );
						}
						else
						{
							/* The tick is held pending in ulCriticalNesting
							until such time that pseudo interrupts are enabled
							again. */
						}
						break;

					default:

						if( ulCriticalNesting == 0UL )
						{
							/* Is a handler installed? */
							if( vIsrHandler[ i ] != NULL )
							{
								lSwitchRequired = pdTRUE;

								/* Run the actual handler. */
								vIsrHandler[ i ]();

								/* Clear the interrupt pending bit. */
								ulPendingInterrupts &= ~( 1UL << i );

								/* TODO:  Need to have some sort of handshake 
								event here for non-tick and none yield 
								interrupts. */
							}
						}
						break;
				}
			}
		}

		if( lSwitchRequired != pdFALSE )
		{
			void *pvOldCurrentTCB;

			pvOldCurrentTCB = pxCurrentTCB;

			/* Save the state of the current thread before suspending it. */
			pxThreadState = ( xThreadState *) *( ( unsigned long * ) pxCurrentTCB );
			pxThreadState->ulCriticalNesting = ulCriticalNesting ;
			
			/* Select the next task to run. */
			vTaskSwitchContext();
			
			/* If the task selected to enter the running state is not the task
			that is already in the running state. */
			if( pvOldCurrentTCB != pxCurrentTCB )
			{
				/* Suspend the old thread. */
				SetThreadPriority( pxThreadState->pvThread, THREAD_PRIORITY_IDLE );
				SuspendThread( pxThreadState->pvThread );

				//sprintf( cTraceBuffer, "Event processor: suspending %s, resuming %s\r\n", ((xTCB*)pvOldCurrentTCB)->pcTaskName, ((xTCB*)pxCurrentTCB)->pcTaskName );
				//vPortTrace( cTraceBuffer );

				/* Obtain the state of the task now selected to enter the Running state. */
				pxThreadState = ( xThreadState * ) ( *( unsigned long *) pxCurrentTCB );
				ulCriticalNesting = pxThreadState->ulCriticalNesting;
				SetThreadPriority( pxThreadState->pvThread, THREAD_PRIORITY_ABOVE_NORMAL );
				ResumeThread( pxThreadState->pvThread );

				if( pxThreadState->lWaitingInterruptAck == pdTRUE )
				{
					pxThreadState->lWaitingInterruptAck = pdFALSE;
					vPortTrace( "prvProcessPseudoInterrupts: Acking interrupt\r\n" );
					SetEvent( pvInterruptAcknowledgeEvent );
				}
			}
		}
		else
		{
			/* On exiting a critical section a task may have blocked on the
			interrupt event when only a tick needed processing, in which case
			it will not have been released from waiting on the event yet. */
			pxThreadState = ( xThreadState * ) ( *( unsigned long *) pxCurrentTCB );
			if( pxThreadState->lWaitingInterruptAck == pdTRUE )
			{
				pxThreadState->lWaitingInterruptAck = pdFALSE;
				vPortTrace( "prvProcessPseudoInterrupts: Acking interrupt even though a yield has not been performed.\r\n" );
				SetEvent( pvInterruptAcknowledgeEvent );
			}
		}

		ReleaseMutex( pvInterruptEventMutex );
	}
}
/*-----------------------------------------------------------*/

void vPortEndScheduler( void )
{
}
/*-----------------------------------------------------------*/

void vPortGeneratePseudoInterrupt( unsigned long ulInterruptNumber )
{
xThreadState *pxThreadState;

	if( ( ulInterruptNumber < portMAX_INTERRUPTS ) && ( pvInterruptEventMutex != NULL ) )
	{
		/* Yield interrupts are processed even when critical nesting is non-zero. */
		WaitForSingleObject( pvInterruptEventMutex, INFINITE );
		ulPendingInterrupts |= ( 1 << ulInterruptNumber );

		if( ulCriticalNesting == 0 ) //|| ( ulInterruptNumber == portINTERRUPT_YIELD ) )
		{
			/* The event handler needs to know to signal the interrupt acknowledge event
			the next time this task runs. */
			pxThreadState = ( xThreadState * ) *( ( unsigned long * ) pxCurrentTCB );
			pxThreadState->lWaitingInterruptAck = pdTRUE;

			vPortTrace( "vPortGeneratePseudoInterrupt: Got interrupt mutex, about to signal interrupt event\r\n" );
			SetEvent( pvInterruptEvent );

			/* The interrupt ack event should not be signaled yet - if it is then there
			is an error in the logical simulation. */
			if( WaitForSingleObject( pvInterruptAcknowledgeEvent, 0 ) != WAIT_TIMEOUT )
			{
				/* This line is for a break point only. */
				__asm { NOP };
			}

			SignalObjectAndWait( pvInterruptEventMutex, pvInterruptAcknowledgeEvent, INFINITE, FALSE );
			vPortTrace( "vPortGeneratePseudoInterrupt: About to release interrupt event mutex\r\n" );
//			ReleaseMutex( pvInterruptEventMutex );
			vPortTrace( "vPortGeneratePseudoInterrupt: Interrupt event mutex released, going to wait for interrupt ack\r\n" );

//			WaitForSingleObject( pvInterruptAcknowledgeEvent, INFINITE );
			vPortTrace( "vPortGeneratePseudoInterrupt: Interrupt acknowledged, leaving vPortGeneratePseudoInterrupt()\r\n" );
		}
		else
		{
			ReleaseMutex( pvInterruptEventMutex );
		}
	}
}
/*-----------------------------------------------------------*/

void vPortSetInterruptHandler( unsigned long ulInterruptNumber, void (*pvHandler)( void ) )
{
	if( ulInterruptNumber < portMAX_INTERRUPTS )
	{
		if( pvInterruptEventMutex != NULL )
		{
			WaitForSingleObject( pvInterruptEventMutex, INFINITE );
			vIsrHandler[ ulInterruptNumber ] = pvHandler;
			ReleaseMutex( pvInterruptEventMutex );
		}
		else
		{
			vIsrHandler[ ulInterruptNumber ] = pvHandler;
		}
	}
}
/*-----------------------------------------------------------*/

void vPortEnterCritical( void )
{
	if( xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED )
	{
		WaitForSingleObject( pvInterruptEventMutex, INFINITE );
//		SuspendThread( pvSimulatedTimerThread );
		ulCriticalNesting++;
		ReleaseMutex( pvInterruptEventMutex );
	}
	else
	{
		ulCriticalNesting++;
	}	
}
/*-----------------------------------------------------------*/

void vPortExitCritical( void )
{
xThreadState *pxThreadState;

	if( ulCriticalNesting > portNO_CRITICAL_NESTING )
	{
		if( ulCriticalNesting == ( portNO_CRITICAL_NESTING + 1 ) )
		{
			/* Wait for the interrupt event mutex prior to manipulating or 
			testing the pseudo interrupt control variables. */
			WaitForSingleObject( pvInterruptEventMutex, INFINITE );
			vPortTrace( "vPortExitCritical:  Got interrupt event mutex\r\n" );

//			ResumeThread( pvSimulatedTimerThread );

			/* Now it is safe to decrement the critical nesting count as no
			tick events will be processed until the interrupt event mutex is
			given back. */
			ulCriticalNesting--;

			/* Were any interrupts set to pending while interrupts were 
			(pseudo) disabled? */
			if( ulPendingInterrupts != 0UL )
			{
				SetEvent( pvInterruptEvent );

				/* The interrupt ack event should not be signaled yet - if it 
				is then there is an error in the logical simulation. */
				if( WaitForSingleObject( pvInterruptAcknowledgeEvent, 0 ) != WAIT_TIMEOUT )
				{
					/* This line is for a break point only. */
					__asm { NOP };
				}

				/* The event handler needs to know to signal the interrupt 
				acknowledge event the next time this task runs. */
				pxThreadState = ( xThreadState * ) *( ( unsigned long * ) pxCurrentTCB );
				pxThreadState->lWaitingInterruptAck = pdTRUE;

				SignalObjectAndWait( pvInterruptEventMutex, pvInterruptAcknowledgeEvent, INFINITE, FALSE );
				/* Give back the interrupt event mutex so the event can be processed. */
//				ReleaseMutex( pvInterruptEventMutex );

//				vPortTrace( "vPortExitCritical:  Waiting interrupt ack\r\n" );
//				WaitForSingleObject( pvInterruptAcknowledgeEvent, INFINITE );
				vPortTrace( "vPortExitCritical: Interrupt acknowledged, leaving critical section code\r\n" );
			}
			else
			{
				/* Can't leave here without giving back the interrupt event
				mutex. */
				ReleaseMutex( pvInterruptEventMutex );
			}
		}
		else
		{
			/* Tick interrupts will still not be processed as the critical
			nesting depth will not be zero. */
			ulCriticalNesting--;
		}
	}
}
