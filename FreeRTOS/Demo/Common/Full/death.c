/*
    FreeRTOS V7.5.3 - Copyright (C) 2013 Real Time Engineers Ltd. 
    All rights reserved

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

/**
 * Create a single persistent task which periodically dynamically creates another 
 * four tasks.  The original task is called the creator task, the four tasks it 
 * creates are called suicidal tasks.
 *
 * Two of the created suicidal tasks kill one other suicidal task before killing 
 * themselves - leaving just the original task remaining.  
 *
 * The creator task must be spawned after all of the other demo application tasks 
 * as it keeps a check on the number of tasks under the scheduler control.  The 
 * number of tasks it expects to see running should never be greater than the 
 * number of tasks that were in existence when the creator task was spawned, plus 
 * one set of four suicidal tasks.  If this number is exceeded an error is flagged.
 *
 * \page DeathC death.c
 * \ingroup DemoFiles
 * <HR>
 */

/*
Changes from V2.0.0

	+ Delay periods are now specified using variables and constants of
	  portTickType rather than unsigned long.
*/

#include <stdlib.h>

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"

/* Demo program include files. */
#include "death.h"
#include "print.h"

#define deathSTACK_SIZE		( ( unsigned short ) 512 )

/* The task originally created which is responsible for periodically dynamically 
creating another four tasks. */
static void vCreateTasks( void *pvParameters );

/* The task function of the dynamically created tasks. */
static void vSuicidalTask( void *pvParameters );

/* A variable which is incremented every time the dynamic tasks are created.  This 
is used to check that the task is still running. */
static volatile short sCreationCount = 0;

/* Used to store the number of tasks that were originally running so the creator 
task can tell if any of the suicidal tasks have failed to die. */
static volatile unsigned portBASE_TYPE uxTasksRunningAtStart = 0;
static const unsigned portBASE_TYPE uxMaxNumberOfExtraTasksRunning = 5;

/* Used to store a handle to the tasks that should be killed by a suicidal task, 
before it kills itself. */
xTaskHandle xCreatedTask1, xCreatedTask2;

/*-----------------------------------------------------------*/

void vCreateSuicidalTasks( unsigned portBASE_TYPE uxPriority )
{
unsigned portBASE_TYPE *puxPriority;

	/* Create the Creator tasks - passing in as a parameter the priority at which 
	the suicidal tasks should be created. */
	puxPriority = ( unsigned portBASE_TYPE * ) pvPortMalloc( sizeof( unsigned portBASE_TYPE ) );
	*puxPriority = uxPriority;

	xTaskCreate( vCreateTasks, "CREATOR", deathSTACK_SIZE, ( void * ) puxPriority, uxPriority, NULL );

	/* Record the number of tasks that are running now so we know if any of the 
	suicidal tasks have failed to be killed. */
	uxTasksRunningAtStart = uxTaskGetNumberOfTasks();
}
/*-----------------------------------------------------------*/

static void vSuicidalTask( void *pvParameters )
{
portDOUBLE d1, d2;
xTaskHandle xTaskToKill;
const portTickType xDelay = ( portTickType ) 500 / portTICK_RATE_MS;

	if( pvParameters != NULL )
	{
		/* This task is periodically created four times.  Tow created tasks are 
		passed a handle to the other task so it can kill it before killing itself.  
		The other task is passed in null. */
		xTaskToKill = *( xTaskHandle* )pvParameters;
	}
	else
	{
		xTaskToKill = NULL;
	}

	for( ;; )
	{
		/* Do something random just to use some stack and registers. */
		d1 = 2.4;
		d2 = 89.2;
		d2 *= d1;
		vTaskDelay( xDelay );

		if( xTaskToKill != NULL )
		{
			/* Make sure the other task has a go before we delete it. */
			vTaskDelay( ( portTickType ) 0 );
			/* Kill the other task that was created by vCreateTasks(). */
			vTaskDelete( xTaskToKill );
			/* Kill ourselves. */
			vTaskDelete( NULL );
		}
	}
}/*lint !e818 !e550 Function prototype must be as per standard for task functions. */
/*-----------------------------------------------------------*/

static void vCreateTasks( void *pvParameters )
{
const portTickType xDelay = ( portTickType ) 1000 / portTICK_RATE_MS;
unsigned portBASE_TYPE uxPriority;
const char * const pcTaskStartMsg = "Create task started.\r\n";

	/* Queue a message for printing to say the task has started. */
	vPrintDisplayMessage( &pcTaskStartMsg );

	uxPriority = *( unsigned portBASE_TYPE * ) pvParameters;
	vPortFree( pvParameters );

	for( ;; )
	{
		/* Just loop round, delaying then creating the four suicidal tasks. */
		vTaskDelay( xDelay );

		xTaskCreate( vSuicidalTask, "SUICIDE1", deathSTACK_SIZE, NULL, uxPriority, &xCreatedTask1 );
		xTaskCreate( vSuicidalTask, "SUICIDE2", deathSTACK_SIZE, &xCreatedTask1, uxPriority, NULL );

		xTaskCreate( vSuicidalTask, "SUICIDE1", deathSTACK_SIZE, NULL, uxPriority, &xCreatedTask2 );
		xTaskCreate( vSuicidalTask, "SUICIDE2", deathSTACK_SIZE, &xCreatedTask2, uxPriority, NULL );

		++sCreationCount;
	}
}
/*-----------------------------------------------------------*/

/* This is called to check that the creator task is still running and that there 
are not any more than four extra tasks. */
portBASE_TYPE xIsCreateTaskStillRunning( void )
{
static short sLastCreationCount = 0;
short sReturn = pdTRUE;
unsigned portBASE_TYPE uxTasksRunningNow;

	if( sLastCreationCount == sCreationCount )
	{
		sReturn = pdFALSE;
	}
	
	uxTasksRunningNow = uxTaskGetNumberOfTasks();

	if( uxTasksRunningNow < uxTasksRunningAtStart )
	{
		sReturn = pdFALSE;
	}
	else if( ( uxTasksRunningNow - uxTasksRunningAtStart ) > uxMaxNumberOfExtraTasksRunning )
	{
		sReturn = pdFALSE;
	}
	else
	{
		/* Everything is okay. */
	}

	return sReturn;
}


