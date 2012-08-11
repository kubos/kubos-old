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

/*
 * This demo file demonstrates how to send data between an ISR and a 
 * co-routine.  A tick hook function is used to periodically pass data between
 * the RTOS tick and a set of 'hook' co-routines.
 *
 * hookNUM_HOOK_CO_ROUTINES co-routines are created.  Each co-routine blocks
 * to wait for a character to be received on a queue from the tick ISR, checks
 * to ensure the character received was that expected, then sends the number
 * back to the tick ISR on a different queue.
 * 
 * The tick ISR checks the numbers received back from the 'hook' co-routines 
 * matches the number previously sent.
 *
 * If at any time a queue function returns unexpectedly, or an incorrect value
 * is received either by the tick hook or a co-routine then an error is 
 * latched.
 *
 * This demo relies on each 'hook' co-routine to execute between each 
 * hookTICK_CALLS_BEFORE_POST tick interrupts.  This and the heavy use of 
 * queues from within an interrupt may result in an error being detected on 
 * slower targets simply due to timing.
 */

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "croutine.h"
#include "queue.h"

/* Demo application includes. */
#include "crhook.h"

/* The number of 'hook' co-routines that are to be created. */
#define hookNUM_HOOK_CO_ROUTINES        ( 4 )

/* The number of times the tick hook should be called before a character is 
posted to the 'hook' co-routines. */
#define hookTICK_CALLS_BEFORE_POST      ( 500 )

/* There should never be more than one item in any queue at any time. */
#define hookHOOK_QUEUE_LENGTH           ( 1 )

/* Don't block when initially posting to the queue. */
#define hookNO_BLOCK_TIME               ( 0 )

/* The priority relative to other co-routines (rather than tasks) that the
'hook' co-routines should take. */
#define mainHOOK_CR_PRIORITY			( 1 )
/*-----------------------------------------------------------*/

/*
 * The co-routine function itself.
 */
static void prvHookCoRoutine( xCoRoutineHandle xHandle, unsigned portBASE_TYPE uxIndex );


/*
 * The tick hook function.  This receives a number from each 'hook' co-routine
 * then sends a number to each co-routine.  An error is flagged if a send or 
 * receive fails, or an unexpected number is received.
 */
void vApplicationTickHook( void );

/*-----------------------------------------------------------*/

/* Queues used to send data FROM a co-routine TO the tick hook function.
The hook functions received (Rx's) on these queues.  One queue per
'hook' co-routine. */
static xQueueHandle xHookRxQueues[ hookNUM_HOOK_CO_ROUTINES ];

/* Queues used to send data FROM the tick hook TO a co-routine function.
The hood function transmits (Tx's) on these queues.  One queue per
'hook' co-routine. */
static xQueueHandle xHookTxQueues[ hookNUM_HOOK_CO_ROUTINES ];

/* Set to true if an error is detected at any time. */
static portBASE_TYPE xCoRoutineErrorDetected = pdFALSE;

/*-----------------------------------------------------------*/

void vStartHookCoRoutines( void )
{
unsigned portBASE_TYPE uxIndex, uxValueToPost = 0;

	for( uxIndex = 0; uxIndex < hookNUM_HOOK_CO_ROUTINES; uxIndex++ )
	{
		/* Create a queue to transmit to and receive from each 'hook' 
		co-routine. */
		xHookRxQueues[ uxIndex ] = xQueueCreate( hookHOOK_QUEUE_LENGTH, sizeof( unsigned portBASE_TYPE ) );
		xHookTxQueues[ uxIndex ] = xQueueCreate( hookHOOK_QUEUE_LENGTH, sizeof( unsigned portBASE_TYPE ) );

		/* To start things off the tick hook function expects the queue it 
		uses to receive data to contain a value.  */
		xQueueSend( xHookRxQueues[ uxIndex ], &uxValueToPost, hookNO_BLOCK_TIME );

		/* Create the 'hook' co-routine itself. */
		xCoRoutineCreate( prvHookCoRoutine, mainHOOK_CR_PRIORITY, uxIndex );
	}
}
/*-----------------------------------------------------------*/

static unsigned portBASE_TYPE uxCallCounter = 0, uxNumberToPost = 0;
void vApplicationTickHook( void )
{
unsigned portBASE_TYPE uxReceivedNumber;
signed portBASE_TYPE xIndex, xCoRoutineWoken;

	/* Is it time to talk to the 'hook' co-routines again? */
	uxCallCounter++;
	if( uxCallCounter >= hookTICK_CALLS_BEFORE_POST )
	{
		uxCallCounter = 0;

		for( xIndex = 0; xIndex < hookNUM_HOOK_CO_ROUTINES; xIndex++ )
		{
			xCoRoutineWoken = pdFALSE;
			if( crQUEUE_RECEIVE_FROM_ISR( xHookRxQueues[ xIndex ], &uxReceivedNumber, &xCoRoutineWoken ) != pdPASS )
			{
				/* There is no reason why we would not expect the queue to 
				contain a value. */
				xCoRoutineErrorDetected = pdTRUE;
			}
			else
			{
				/* Each queue used to receive data from the 'hook' co-routines 
				should contain the number we last posted to the same co-routine. */
				if( uxReceivedNumber != uxNumberToPost )
				{
					xCoRoutineErrorDetected = pdTRUE;
				}

				/* Nothing should be blocked waiting to post to the queue. */
				if( xCoRoutineWoken != pdFALSE )
				{
					xCoRoutineErrorDetected = pdTRUE;
				}
			}
		}

		/* Start the next cycle by posting the next number onto each Tx queue. */
		uxNumberToPost++;

		for( xIndex = 0; xIndex < hookNUM_HOOK_CO_ROUTINES; xIndex++ )
		{
			if( crQUEUE_SEND_FROM_ISR( xHookTxQueues[ xIndex ], &uxNumberToPost, pdFALSE ) != pdTRUE )
			{
				/* Posting to the queue should have woken the co-routine that 
				was blocked on the queue. */
				xCoRoutineErrorDetected = pdTRUE;
			}
		}
	}
}
/*-----------------------------------------------------------*/

static void prvHookCoRoutine( xCoRoutineHandle xHandle, unsigned portBASE_TYPE uxIndex )
{
static unsigned portBASE_TYPE uxReceivedValue[ hookNUM_HOOK_CO_ROUTINES ];
portBASE_TYPE xResult;

	/* Each co-routine MUST start with a call to crSTART(); */
	crSTART( xHandle );

	for( ;; )
	{
		/* Wait to receive a value from the tick hook. */
		xResult = pdFAIL;
		crQUEUE_RECEIVE( xHandle, xHookTxQueues[ uxIndex ], &( uxReceivedValue[ uxIndex ] ), portMAX_DELAY, &xResult );

		/* There is no reason why we should not have received something on
		the queue. */
		if( xResult != pdPASS )
		{
			xCoRoutineErrorDetected = pdTRUE;
		}

		/* Send the same number back to the idle hook so it can verify it. */
		xResult = pdFAIL;
		crQUEUE_SEND( xHandle, xHookRxQueues[ uxIndex ], &( uxReceivedValue[ uxIndex ] ), hookNO_BLOCK_TIME, &xResult );
		if( xResult != pdPASS )
		{
			/* There is no reason why we should not have been able to post to 
			the queue. */
			xCoRoutineErrorDetected = pdTRUE;
		}
	}

	/* Each co-routine MUST end with a call to crEND(). */
	crEND();
}
/*-----------------------------------------------------------*/

portBASE_TYPE xAreHookCoRoutinesStillRunning( void )
{
	if( xCoRoutineErrorDetected )
	{
		return pdFALSE;
	}
	else
	{
		return pdTRUE;
	}
}



