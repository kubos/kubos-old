/*
    FreeRTOS V7.1.0 - Copyright (C) 2011 Real Time Engineers Ltd.
	

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

#include "FreeRTOS.h"
#include "task.h"

/*
 * Two test tasks that fill the CPU registers with known values before
 * continuously looping round checking that each register still contains its
 * expected value.  Both tasks use a separate set of values, with an incorrect
 * value being found at any time being indicative of an error in the context
 * switch mechanism.  One of the tasks uses a yield instruction to increase the
 * test coverage.  The nature of these tasks necessitates that they are written
 * in assembly code.
 */
static void vRegTest1( void *pvParameters );
static void vRegTest2( void *pvParameters );

/*
 * A task that tests the management of the Interrupt Controller (IC) during a
 * context switch.  The state of the IC current mask level must be maintained
 * across context switches.  Also, yields must be able to be performed when the
 * interrupt controller mask is not zero.  This task tests both these
 * requirements.
 */
static void prvICCheck1Task( void *pvParameters );

/* Counters used to ensure the tasks are still running. */
static volatile unsigned long ulRegTest1Counter = 0UL, ulRegTest2Counter = 0UL, ulICTestCounter = 0UL;

/* Handle to the task that checks the interrupt controller behaviour.  This is
used by the traceTASK_SWITCHED_OUT() macro, which is defined in
FreeRTOSConfig.h and can be removed - it is just for the purpose of this test. */
xTaskHandle xICTestTask = NULL;

/* Variable that gets set to pdTRUE by traceTASK_SWITCHED_OUT each time
is switched out. */
volatile unsigned long ulTaskSwitchedOut;
/*-----------------------------------------------------------*/

void vStartRegTestTasks( void )
{
	xTaskCreate( vRegTest1, ( signed char * ) "RTest1", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
	xTaskCreate( vRegTest2, ( signed char * ) "RTest1", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
	xTaskCreate( prvICCheck1Task, ( signed char * ) "ICCheck", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, &xICTestTask );
}
/*-----------------------------------------------------------*/

static void vRegTest1( void *pvParameters )
{
	__asm volatile
	(
		"	mov		r2, #0x02							\n" /* Fill the registers with known values, r0 is always 0 and r1 is the stack pointer. */
		"	mov		r3, #0x03							\n"
		"	mov		r4, #0x04							\n"
		"	mov		r5, #0x05							\n"
		"	mov		r6, #0x06							\n"
		"	mov		r7, #0x07							\n"
		"	mov		r8, #0x08							\n"
		"	mov		r9, #0x09							\n"
		"	mov		r10, #0x0a							\n"
		"	mov		r11, #0x0b							\n"
		"	mov		r12, #0x0c							\n"
		"	mov		r13, #0x0d							\n"
		"	mov		r14, #0x0e							\n"
		"	mov		r15, #0x0f							\n"
		"												\n"
		"reg_check_loop_1:								\n"
		"	trap	#31									\n"
		"	cmp		r2, #0x02							\n" /* Check that each register still contains the expected value, jump to an infinite loop if an error is found. */
		"	bne.s	reg_check_error_1					\n"
		"	cmp		r3, #0x03							\n"
		"	bne.s	reg_check_error_1					\n"
		"	cmp		r4, #0x04							\n"
		"	bne.s	reg_check_error_1					\n"
		"	cmp		r5, #0x05							\n"
		"	bne.s	reg_check_error_1					\n"
		"	cmp		r6, #0x06							\n"
		"	bne.s	reg_check_error_1					\n"
		"	cmp		r7, #0x07							\n"
		"	bne.s	reg_check_error_1					\n"
		"	cmp		r8, #0x08							\n"
		"	bne.s	reg_check_error_1					\n"
		"	cmp		r9, #0x09							\n"
		"	bne.s	reg_check_error_1					\n"
		"	cmp		r10, #0x0a							\n"
		"	bne.s	reg_check_error_1					\n"
		"	cmp		r11, #0x0b							\n"
		"	bne.s	reg_check_error_1					\n"
		"	cmp		r12, #0x0c							\n"
		"	bne.s	reg_check_error_1					\n"
		"	cmp		r13, #0x0d							\n"
		"	bne.s	reg_check_error_1					\n"
		"	cmp		r14, #0x0e							\n"
		"	bne.s	reg_check_error_1					\n"
		"	cmp		r15, #0x0f							\n"
		"	bne.s	reg_check_error_1					\n"
		"												\n"
		"	ld		r2, [r0]+short(ulRegTest1Counter)	\n" /* Increment the loop counter to show that this task is still running error free. */
		"	add		r2, #1								\n"
		"	st		r2, [r0]+short(ulRegTest1Counter)	\n"
		"	mov		r2, #0x02							\n"
		"												\n"
		"	bra.s	reg_check_loop_1					\n" /* Do it all again. */
		"												\n"
		"reg_check_error_1:								\n"
			"bra.s		.								\n"
	);
}
/*-----------------------------------------------------------*/

static void vRegTest2( void *pvParameters )
{
	__asm volatile
	(
		"	mov		r2, #0x12							\n" /* Fill the registers with known values, r0 is always 0 and r1 is the stack pointer. */
		"	mov		r3, #0x13							\n"
		"	mov		r4, #0x14							\n"
		"	mov		r5, #0x15							\n"
		"	mov		r6, #0x16							\n"
		"	mov		r7, #0x17							\n"
		"	mov		r8, #0x18							\n"
		"	mov		r9, #0x19							\n"
		"	mov		r10, #0x1a							\n"
		"	mov		r11, #0x1b							\n"
		"	mov		r12, #0x1c							\n"
		"	mov		r13, #0x1d							\n"
		"	mov		r14, #0x1e							\n"
		"	mov		r15, #0x1f							\n"
		"												\n"
		"reg_check_loop_2:								\n"
		"	cmp		r2, #0x12							\n" /* Check that each register still contains the expected value, jump to an infinite loop if an error is found. */
		"	bne.s	reg_check_error_2					\n"
		"	cmp		r3, #0x13							\n"
		"	bne.s	reg_check_error_2					\n"
		"	cmp		r4, #0x14							\n"
		"	bne.s	reg_check_error_2					\n"
		"	cmp		r5, #0x15							\n"
		"	bne.s	reg_check_error_2					\n"
		"	cmp		r6, #0x16							\n"
		"	bne.s	reg_check_error_2					\n"
		"	cmp		r7, #0x17							\n"
		"	bne.s	reg_check_error_2					\n"
		"	cmp		r8, #0x18							\n"
		"	bne.s	reg_check_error_2					\n"
		"	cmp		r9, #0x19							\n"
		"	bne.s	reg_check_error_2					\n"
		"	cmp		r10, #0x1a							\n"
		"	bne.s	reg_check_error_2					\n"
		"	cmp		r11, #0x1b							\n"
		"	bne.s	reg_check_error_2					\n"
		"	cmp		r12, #0x1c							\n"
		"	bne.s	reg_check_error_2					\n"
		"	cmp		r13, #0x1d							\n"
		"	bne.s	reg_check_error_2					\n"
		"	cmp		r14, #0x1e							\n"
		"	bne.s	reg_check_error_2					\n"
		"	cmp		r15, #0x1f							\n"
		"	bne.s	reg_check_error_2					\n"
		"												\n"
		"	ld		r2, [r0]+short(ulRegTest2Counter)	\n" /* Increment the loop counter to show that this task is still running error free. */
		"	add		r2, #1								\n"
		"	st		r2, [r0]+short(ulRegTest2Counter)	\n"
		"	mov		r2, #0x12							\n"
		"												\n"
		"	bra.s	reg_check_loop_2					\n" /* Do it all again. */
		"												\n"
		"reg_check_error_2:								\n"
			"bra.s		.								\n"
	);
}
/*-----------------------------------------------------------*/

static void prvICCheck1Task( void *pvParameters )
{
long lICCheckStatus = pdPASS;

	for( ;; )
	{
		/* At this point the interrupt mask should be zero. */
		if( ic->cpl != 0 )
		{
			lICCheckStatus = pdFAIL;
		}

		/* If we yield here, it should still be 0 when the task next runs.
		ulTaskSwitchedOut is just used to check that a switch does actually
		happen. */
		ulTaskSwitchedOut = pdFALSE;
		taskYIELD();
		if( ( ulTaskSwitchedOut != pdTRUE ) || ( ic->cpl != 0 ) )
		{
			lICCheckStatus = pdFAIL;
		}

		/* Set the interrupt mask to portSYSTEM_INTERRUPT_PRIORITY_LEVEL + 1,
		before checking it is as expected. */
		taskENTER_CRITICAL();
		if( ic->cpl != ( portSYSTEM_INTERRUPT_PRIORITY_LEVEL + 1 ) )
		{
			lICCheckStatus = pdFAIL;
		}

		/* If we yield here, it should still be
		portSYSTEM_INTERRUPT_PRIORITY_LEVEL + 10 when the task next runs.  */
		ulTaskSwitchedOut = pdFALSE;
		taskYIELD();
		if( ( ulTaskSwitchedOut != pdTRUE ) || ( ic->cpl != ( portSYSTEM_INTERRUPT_PRIORITY_LEVEL + 1 ) ) )
		{
			lICCheckStatus = pdFAIL;
		}

		/* Return the interrupt mask to its default state. */
		taskEXIT_CRITICAL();

		/* Just increment a loop counter so the check task knows if this task
		is still running or not. */
		if( lICCheckStatus == pdPASS )
		{
			ulICTestCounter++;
		}
	}
}
/*-----------------------------------------------------------*/

portBASE_TYPE xAreRegTestTasksStillRunning( void )
{
static unsigned long ulLastCounter1 = 0UL, ulLastCounter2 = 0UL, ulLastICTestCounter = 0UL;
long lReturn;

	/* Check that both loop counters are still incrementing, indicating that
	both reg test tasks are still running error free. */
	if( ulLastCounter1 == ulRegTest1Counter )
	{
		lReturn = pdFAIL;
	}
	else if( ulLastCounter2 == ulRegTest2Counter )
	{
		lReturn = pdFAIL;
	}
	else if( ulLastICTestCounter == ulICTestCounter )
	{
		lReturn = pdFAIL;
	}
	else
	{
		lReturn = pdPASS;
	}

	ulLastCounter1 = ulRegTest1Counter;
	ulLastCounter2 = ulRegTest2Counter;
	ulLastICTestCounter = ulICTestCounter;

	return lReturn;
}














