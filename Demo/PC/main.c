/*
    FreeRTOS V7.1.1 - Copyright (C) 2012 Real Time Engineers Ltd.
	

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
     *    not run, what could be wrong?                                      *
     *                                                                       *
     *    http://www.FreeRTOS.org/FAQHelp.html                               *
     *                                                                       *
    ***************************************************************************

    
    http://www.FreeRTOS.org - Documentation, training, latest information, 
    license and contact details.
    
    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool.

    Real Time Engineers ltd license FreeRTOS to High Integrity Systems, who sell 
    the code with commercial support, indemnification, and middleware, under 
    the OpenRTOS brand: http://www.OpenRTOS.com.  High Integrity Systems also
    provide a safety engineered and independently SIL3 certified version under 
    the SafeRTOS brand: http://www.SafeRTOS.com.
*/

/**
 * Creates all the demo application tasks and co-routines, then starts the 
 * scheduler.
 *
 * Main. c also creates a task called "Print".  This only executes every 
 * five seconds but has the highest priority so is guaranteed to get 
 * processor time.  Its main function is to check that all the other tasks 
 * are still operational.  Nearly all the tasks in the demo application 
 * maintain a unique count that is incremented each time the task successfully 
 * completes its function.  Should any error occur within the task the count is 
 * permanently halted.  The print task checks the count of each task to ensure 
 * it has changed since the last time the print task executed.  If any count is 
 * found not to have changed the print task displays an appropriate message.  
 * If all the tasks are still incrementing their unique counts the print task 
 * displays an "OK" message.
 *
 * The LED flash tasks do not maintain a count as they already provide visual 
 * feedback of their status.
 *
 * The print task blocks on the queue into which messages that require 
 * displaying are posted.  It will therefore only block for the full 5 seconds
 * if no messages are posted onto the queue.
 *
 * Main. c also provides a demonstration of how the trace visualisation utility
 * can be used, and how the scheduler can be stopped.
 *
 * \page MainC main.c
 * \ingroup DemoFiles
 * <HR>
 */

#include <stdlib.h>
#include <conio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "croutine.h"
#include "partest.h"
#include "serial.h"

/* Demo file headers. */
#include "BlockQ.h"
#include "PollQ.h"
#include "death.h"
#include "crflash.h"
#include "flop.h"
#include "print.h"
#include "comtest.h"
#include "fileio.h"
#include "semtest.h"
#include "integer.h"
#include "dynamic.h"
#include "mevents.h"
#include "crhook.h"
#include "blocktim.h"
#include "AltBlock.h"
#include "GenQTest.h"
#include "QPeek.h"
#include "countsem.h"
#include "AltQTest.h"
#include "AltPollQ.h"
#include "AltBlckQ.h"
#include "RecMutex.h"

/* Priority definitions for the tasks in the demo application. */
#define mainLED_TASK_PRIORITY		( tskIDLE_PRIORITY + 1 )
#define mainCREATOR_TASK_PRIORITY	( tskIDLE_PRIORITY + 3 )
#define mainPRINT_TASK_PRIORITY		( tskIDLE_PRIORITY + 4 )
#define mainQUEUE_POLL_PRIORITY		( tskIDLE_PRIORITY + 2 )
#define mainQUEUE_BLOCK_PRIORITY	( tskIDLE_PRIORITY + 3 )
#define mainCOM_TEST_PRIORITY		( tskIDLE_PRIORITY + 2 )
#define mainSEMAPHORE_TASK_PRIORITY	( tskIDLE_PRIORITY + 1 )
#define mainGENERIC_QUEUE_PRIORITY	( tskIDLE_PRIORITY )

#define mainPRINT_STACK_SIZE		( ( unsigned short ) 512 )
#define mainDEBUG_LOG_BUFFER_SIZE	( ( unsigned short ) 20480 )

/* The number of flash co-routines to create. */
#define mainNUM_FLASH_CO_ROUTINES	( 8 )

/* Task function for the "Print" task as described at the top of the file. */
static void vErrorChecks( void *pvParameters );

/* Function that checks the unique count of all the other tasks as described at
the top of the file. */
static void prvCheckOtherTasksAreStillRunning( void );

/* Key presses can be used to start/stop the trace visualisation utility or stop
the scheduler. */
static void	prvCheckForKeyPresses( void );

/* Buffer used by the trace visualisation utility so only needed if the trace
being used. */
#if configUSE_TRACE_FACILITY == 1
	static char pcWriteBuffer[ mainDEBUG_LOG_BUFFER_SIZE ];
#endif

/* Constant definition used to turn on/off the pre-emptive scheduler. */
static const short sUsingPreemption = configUSE_PREEMPTION;

/* Start the math tasks appropriate to the build.  The Borland port does
not yet support floating point so uses the integer equivalent. */
static void prvStartMathTasks( void );

/* Check which ever tasks are relevant to this build. */
static portBASE_TYPE prvCheckMathTasksAreStillRunning( void );

/* Used to demonstrate the "task switched in" callback function. */
static portBASE_TYPE prvExampleTaskHook( void * pvParameter );

/* Just used to count the number of times the example task callback function is
called, and the number of times a queue send passes. */
static unsigned long long uxCheckTaskHookCallCount = 0;
static unsigned long long uxQueueSendPassedCount = 0;

/*-----------------------------------------------------------*/

short main( void )
{
	/* Initialise hardware and utilities. */
	vParTestInitialise();
	vPrintInitialise();
	
	/* CREATE ALL THE DEMO APPLICATION TASKS. */
	prvStartMathTasks();
	vStartComTestTasks( mainCOM_TEST_PRIORITY, serCOM1, ser115200 );
	vStartPolledQueueTasks( mainQUEUE_POLL_PRIORITY );
	vStartBlockingQueueTasks( mainQUEUE_BLOCK_PRIORITY );
	vCreateBlockTimeTasks();
	vStartGenericQueueTasks( mainGENERIC_QUEUE_PRIORITY );
	vStartSemaphoreTasks( mainSEMAPHORE_TASK_PRIORITY );
	vStartDynamicPriorityTasks();
	vStartMultiEventTasks();
	vStartQueuePeekTasks();
	vStartCountingSemaphoreTasks();
	vStartAltGenericQueueTasks( mainGENERIC_QUEUE_PRIORITY );
	vCreateAltBlockTimeTasks();
	vStartAltBlockingQueueTasks( mainQUEUE_BLOCK_PRIORITY );	
	vStartAltPolledQueueTasks( mainQUEUE_POLL_PRIORITY );
	vStartRecursiveMutexTasks();
		
	/* Create the "Print" task as described at the top of the file. */
	xTaskCreate( vErrorChecks, "Print", mainPRINT_STACK_SIZE, NULL, mainPRINT_TASK_PRIORITY, NULL );

	/* This task has to be created last as it keeps account of the number of tasks
	it expects to see running. */
	vCreateSuicidalTasks( mainCREATOR_TASK_PRIORITY );

	/* Create the co-routines that flash the LED's. */
	vStartFlashCoRoutines( mainNUM_FLASH_CO_ROUTINES );

	/* Create the co-routines that communicate with the tick hook. */
	vStartHookCoRoutines();

	/* Set the scheduler running.  This function will not return unless a task
	calls vTaskEndScheduler(). */
	vTaskStartScheduler();

	return 1;
}
/*-----------------------------------------------------------*/

static portBASE_TYPE prvExampleTaskHook( void * pvParameter )
{
	if( pvParameter != ( void * ) 0xabcd )
	{
		/* The parameter did not contain the value we expected, so cause an
		error to be detected by setting the call count back to zero. */
		uxCheckTaskHookCallCount = 0;
	}
	else
	{
		/* Simply increment a number so we know the callback has been executed. */
		uxCheckTaskHookCallCount++;
	}

	return 0;
}
/*-----------------------------------------------------------*/

void vMainQueueSendPassed( void )
{
	/* This is just an example implementation of the "queue send" trace hook. */
	uxQueueSendPassedCount++;
}
/*-----------------------------------------------------------*/

static void vErrorChecks( void *pvParameters )
{
portTickType xExpectedWakeTime;
const portTickType xPrintRate = ( portTickType ) 5000 / portTICK_RATE_MS;
const long lMaxAllowableTimeDifference = ( long ) 0;
portTickType xWakeTime;
long lTimeDifference;
const char *pcReceivedMessage;
const char * const pcTaskBlockedTooLongMsg = "Print task blocked too long!\r\n";
const char * const pcUnexpectedHookValueMsg = "Task hook has unexpected value!\r\n";

	( void ) pvParameters;

	/* Register our callback function. */
	vTaskSetApplicationTaskTag( NULL, prvExampleTaskHook );
	
	/* Just for test purposes. */
	if( xTaskGetApplicationTaskTag( NULL ) != prvExampleTaskHook )
	{
		vPrintDisplayMessage( &pcUnexpectedHookValueMsg );
	}

	/* Loop continuously, blocking, then checking all the other tasks are still
	running, before blocking once again.  This task blocks on the queue of
	messages that require displaying so will wake either by its time out expiring,
	or a message becoming available. */
	for( ;; )
	{
		/* Calculate the time we will unblock if no messages are received
		on the queue.  This is used to check that we have not blocked for too long. */
		xExpectedWakeTime = xTaskGetTickCount();
		xExpectedWakeTime += xPrintRate;

		/* Block waiting for either a time out or a message to be posted that
		required displaying. */
		pcReceivedMessage = pcPrintGetNextMessage( xPrintRate );

		/* Was a message received? */
		if( pcReceivedMessage == NULL )
		{
			/* A message was not received so we timed out, did we unblock at the
			expected time? */
			xWakeTime = xTaskGetTickCount();

			/* Calculate the difference between the time we unblocked and the
			time we should have unblocked. */
			if( xWakeTime > xExpectedWakeTime )
			{
				lTimeDifference = ( long ) ( xWakeTime - xExpectedWakeTime );
			}
			else
			{
				lTimeDifference = ( long ) ( xExpectedWakeTime - xWakeTime );
			}

			if( lTimeDifference > lMaxAllowableTimeDifference )
			{
				/* We blocked too long - create a message that will get
				printed out the next time around.  If we are not using
				preemption then we won't expect the timing to be so
				accurate. */
				if( sUsingPreemption == pdTRUE )
				{
					vPrintDisplayMessage( &pcTaskBlockedTooLongMsg );
				}
			}

			/* Check the other tasks are still running, just in case. */
			prvCheckOtherTasksAreStillRunning();
		}
		else
		{
			/* We unblocked due to a message becoming available.  Send the message
			for printing. */
			vDisplayMessage( pcReceivedMessage );
		}

		/* Key presses are used to invoke the trace visualisation utility, or end
		the program. */
		prvCheckForKeyPresses();
	}
}
/*-----------------------------------------------------------*/

static void	prvCheckForKeyPresses( void )
{
short sIn;

	taskENTER_CRITICAL();
		#ifdef DEBUG_BUILD
			/* kbhit can be used in .exe's that are executed from the command
			line, but not if executed through the debugger. */
			sIn = 0;
		#else
			sIn = kbhit();
		#endif
	taskEXIT_CRITICAL();

	if( sIn )
	{
		/* Key presses can be used to start/stop the trace utility, or end the 
		program. */
		sIn = getch();
		switch( sIn )
		{
			/* Only define keys for turning on and off the trace if the trace
			is being used. */
			#if configUSE_TRACE_FACILITY == 1
				case 't' :	vTaskList( pcWriteBuffer );
							vWriteMessageToDisk( pcWriteBuffer );
							break;			
				case 's' :	vTaskStartTrace( pcWriteBuffer, mainDEBUG_LOG_BUFFER_SIZE );
							break;

				case 'e' :	{
								unsigned long ulBufferLength;
								ulBufferLength = ulTaskEndTrace();
								vWriteBufferToDisk( pcWriteBuffer, ulBufferLength );
							}
							break;
			#endif

			default  :	vTaskEndScheduler();
						break;
		}
	}
}
/*-----------------------------------------------------------*/

static void prvCheckOtherTasksAreStillRunning( void )
{
static short sErrorHasOccurred = pdFALSE;
static unsigned long long uxLastHookCallCount = 0, uxLastQueueSendCount = 0;

	if( prvCheckMathTasksAreStillRunning() != pdTRUE )
	{
		vDisplayMessage( "Maths task count unchanged!\r\n" );
		sErrorHasOccurred = pdTRUE;
	}

	if( xAreComTestTasksStillRunning() != pdTRUE )
	{
		vDisplayMessage( "Com test count unchanged!\r\n" );
		sErrorHasOccurred = pdTRUE;
	}

	if( xAreBlockingQueuesStillRunning() != pdTRUE )
	{
		vDisplayMessage( "Blocking queues count unchanged!\r\n" );
		sErrorHasOccurred = pdTRUE;
	}

	if( xAreAltBlockingQueuesStillRunning() != pdTRUE )
	{
		vDisplayMessage( "Alt blocking queues count unchanged!\r\n" );
		sErrorHasOccurred = pdTRUE;
	}

	if( xArePollingQueuesStillRunning() != pdTRUE )
	{
		vDisplayMessage( "Polling queue count unchanged!\r\n" );
		sErrorHasOccurred = pdTRUE;
	}

	if( xAreAltPollingQueuesStillRunning() != pdTRUE )
	{
		vDisplayMessage( "Alt polling queue count unchanged!\r\n" );
		sErrorHasOccurred = pdTRUE;
	}

	if( xIsCreateTaskStillRunning() != pdTRUE )
	{
		vDisplayMessage( "Incorrect number of tasks running!\r\n" );
		sErrorHasOccurred = pdTRUE;
	}

	if( xAreSemaphoreTasksStillRunning() != pdTRUE )
	{
		vDisplayMessage( "Semaphore take count unchanged!\r\n" );
		sErrorHasOccurred = pdTRUE;
	}

	if( xAreDynamicPriorityTasksStillRunning() != pdTRUE )
	{
		vDisplayMessage( "Dynamic priority count unchanged!\r\n" );
		sErrorHasOccurred = pdTRUE;
	}
	
	if( xAreMultiEventTasksStillRunning() != pdTRUE )
	{
		vDisplayMessage( "Error in multi events tasks!\r\n" );
		sErrorHasOccurred = pdTRUE;
	}

	if( xAreFlashCoRoutinesStillRunning() != pdTRUE )
	{
		vDisplayMessage( "Error in co-routine flash tasks!\r\n" );
		sErrorHasOccurred = pdTRUE;
	}

	if( xAreHookCoRoutinesStillRunning() != pdTRUE )
	{
		vDisplayMessage( "Error in tick hook to co-routine communications!\r\n" );
		sErrorHasOccurred = pdTRUE;
	}

	if( xAreBlockTimeTestTasksStillRunning() != pdTRUE )
	{
		vDisplayMessage( "Error in block time test tasks!\r\n" );
		sErrorHasOccurred = pdTRUE;
	}

	if( xAreAltBlockTimeTestTasksStillRunning() != pdTRUE )
	{
		vDisplayMessage( "Error in fast block time test tasks!\r\n" );
		sErrorHasOccurred = pdTRUE;
	}

	if( xAreGenericQueueTasksStillRunning() != pdTRUE )
	{
		vDisplayMessage( "Error in generic queue test task!\r\n" );
		sErrorHasOccurred = pdTRUE;		
	}

	if( xAreAltGenericQueueTasksStillRunning() != pdTRUE )
	{
		vDisplayMessage( "Error in fast generic queue test task!\r\n" );
		sErrorHasOccurred = pdTRUE;		
	}

	if( xAreQueuePeekTasksStillRunning() != pdTRUE )
	{
		vDisplayMessage( "Error in queue peek test task!\r\n" );
		sErrorHasOccurred = pdTRUE;
	}

	if( xAreCountingSemaphoreTasksStillRunning() != pdTRUE )
	{
		vDisplayMessage( "Error in counting semaphore demo task!\r\n" );
		sErrorHasOccurred = pdTRUE;
	}

	if( xAreRecursiveMutexTasksStillRunning() != pdTRUE )
	{
		vDisplayMessage( "Error in recursive mutex tasks!\r\n" );
		sErrorHasOccurred = pdTRUE;
	}

	/* The hook function associated with this task is called each time the task
	is switched in.  We therefore expect the number of times the callback 
	function has been executed to have increrment since the last time this 
	function executed. */
	if( uxCheckTaskHookCallCount <= uxLastHookCallCount )
	{
		vDisplayMessage( "Error in task hook call count!\r\n" );
		sErrorHasOccurred = pdTRUE;
	}
	else
	{
		uxLastHookCallCount = uxCheckTaskHookCallCount;
	}

	/* We would expect some queue sending to occur between calls of this 
	function. */
	if( uxQueueSendPassedCount <= uxLastQueueSendCount )
	{
		vDisplayMessage( "Error in queue send hook call count!\r\n" );
		sErrorHasOccurred = pdTRUE;
	}
	else
	{
		uxLastQueueSendCount = uxQueueSendPassedCount;
	}

	if( sErrorHasOccurred == pdFALSE )
	{
		vDisplayMessage( "OK " );
	}
}
/*-----------------------------------------------------------*/

static void prvStartMathTasks( void )
{
	#ifdef BCC_INDUSTRIAL_PC_PORT
		/* The Borland project does not yet support floating point. */
		vStartIntegerMathTasks( tskIDLE_PRIORITY );
	#else
		vStartMathTasks( tskIDLE_PRIORITY );
	#endif
}
/*-----------------------------------------------------------*/

static portBASE_TYPE prvCheckMathTasksAreStillRunning( void )
{
	#ifdef BCC_INDUSTRIAL_PC_PORT
		/* The Borland project does not yet support floating point. */
		return xAreIntegerMathsTaskStillRunning();
	#else
		return xAreMathsTaskStillRunning();
	#endif
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
	/* The co-routines are executed in the idle task using the idle task 
	hook. */
	vCoRoutineSchedule();
}
/*-----------------------------------------------------------*/

