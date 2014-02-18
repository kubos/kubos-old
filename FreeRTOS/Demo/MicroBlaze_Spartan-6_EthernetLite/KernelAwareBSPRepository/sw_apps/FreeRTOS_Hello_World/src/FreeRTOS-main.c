/*
    FreeRTOS V8.0.0 - Copyright (C) 2014 Real Time Engineers Ltd.
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

/*
 * FreeRTOS-main.c (this file) defines a very simple demo that creates two tasks,
 * one queue, and one timer.
 *
 * The main() Function:
 * main() creates one software timer, one queue, and two tasks.  It then starts
 * the scheduler.
 *
 * The Queue Send Task:
 * The queue send task is implemented by the prvQueueSendTask() function in
 * this file.  prvQueueSendTask() sits in a loop that causes it to repeatedly
 * block for 200 milliseconds, before sending the value 100 to the queue that
 * was created within main().  Once the value is sent, the task loops back
 * around to block for another 200 milliseconds.
 *
 * The Queue Receive Task:
 * The queue receive task is implemented by the prvQueueReceiveTask() function
 * in this file.  prvQueueReceiveTask() sits in a loop that causes it to
 * repeatedly attempt to read data from the queue that was created within
 * main().  When data is received, the task checks the value of the data, and
 * if the value equals the expected 100, increments the ulRecieved variable.
 * The 'block time' parameter passed to the queue receive function specifies
 * that the task should be held in the Blocked state indefinitely to wait for
 * data to be available on the queue.  The queue receive task will only leave
 * the Blocked state when the queue send task writes to the queue.  As the queue
 * send task writes to the queue every 200 milliseconds, the queue receive task
 * leaves the Blocked state every 200 milliseconds, and therefore toggles the LED
 * every 200 milliseconds.
 *
 * The Software Timer:
 * The software timer is configured to be an "auto reset" timer.  Its callback
 * function simply increments the ulCallback variable each time it executes.
 */

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

/* BSP includes. */
#include "xtmrctr.h"

/* Priorities at which the tasks are created. */
#define mainQUEUE_RECEIVE_TASK_PRIORITY		( tskIDLE_PRIORITY + 2 )
#define	mainQUEUE_SEND_TASK_PRIORITY		( tskIDLE_PRIORITY + 1 )

/* The rate at which data is sent to the queue, specified in milliseconds, and
converted to ticks using the portTICK_PERIOD_MS constant. */
#define mainQUEUE_SEND_FREQUENCY_MS			( 200 / portTICK_PERIOD_MS )

/* The number of items the queue can hold.  This is 1 as the receive task
will remove items as they are added because it has the higher priority, meaning
the send task should always find the queue empty. */
#define mainQUEUE_LENGTH					( 1 )

/* A block time of 0 simply means, "don't block". */
#define mainDONT_BLOCK						( TickType_t ) 0

/* The following constants describe the timer instance used in this application.
They are defined here such that a user can easily change all the needed parameters
in one place. */
#define TIMER_DEVICE_ID						XPAR_TMRCTR_0_DEVICE_ID
#define TIMER_FREQ_HZ						XPAR_TMRCTR_0_CLOCK_FREQ_HZ
#define TIMER_INTR_ID						XPAR_INTC_0_TMRCTR_0_VEC_ID

/*-----------------------------------------------------------*/

/*
 * The tasks as described in the comments at the top of this file.
 */
static void prvQueueReceiveTask( void *pvParameters );
static void prvQueueSendTask( void *pvParameters );

/*
 * The LED timer callback function.  This does nothing but increment the
 * ulCallback variable each time it executes.
 */
static void vSoftwareTimerCallback( TimerHandle_t xTimer );

/*-----------------------------------------------------------*/

/* The queue used by the queue send and queue receive tasks. */
static QueueHandle_t xQueue = NULL;

/* The LED software timer.  This uses vSoftwareTimerCallback() as its callback
function. */
static TimerHandle_t xExampleSoftwareTimer = NULL;

/*-----------------------------------------------------------*/

/* Structures that hold the state of the various peripherals used by this demo.
These are used by the Xilinx peripheral driver API functions. */
static XTmrCtr xTimer0Instance;

/* The variable that is incremented each time the receive task receives the
value 100. */
static unsigned long ulReceived = 0UL;

/* The variable that is incremented each time the software time callback function
executes. */
static unsigned long ulCallback = 0UL;

/*-----------------------------------------------------------*/

int main( void )
{
	/***************************************************************************
	See http://www.FreeRTOS.org for full information on FreeRTOS, including
	an API reference, pdf API reference manuals, and FreeRTOS tutorial books.

	See http://www.freertos.org/Free-RTOS-for-Xilinx-MicroBlaze-on-Spartan-6-FPGA.html
	for comprehensive standalone FreeRTOS for MicroBlaze demos.
	***************************************************************************/

	/* Create the queue used by the queue send and queue receive tasks as
	described in the comments at the top of this file. */
	xQueue = xQueueCreate( mainQUEUE_LENGTH, sizeof( unsigned long ) );

	/* Sanity check that the queue was created. */
	configASSERT( xQueue );

	/* Start the two tasks as described in the comments at the top of this
	file. */
	xTaskCreate( prvQueueReceiveTask, "Rx", configMINIMAL_STACK_SIZE, NULL, mainQUEUE_RECEIVE_TASK_PRIORITY, NULL );
	xTaskCreate( prvQueueSendTask, "TX", configMINIMAL_STACK_SIZE, NULL, mainQUEUE_SEND_TASK_PRIORITY, NULL );

	/* Create the software timer */
	xExampleSoftwareTimer = xTimerCreate( 	"SoftwareTimer", 			/* A text name, purely to help debugging. */
											( 5000 / portTICK_PERIOD_MS ),/* The timer period, in this case 5000ms (5s). */
											pdTRUE,						/* This is an auto-reload timer, so xAutoReload is set to pdTRUE. */
											( void * ) 0,				/* The ID is not used, so can be set to anything. */
											vSoftwareTimerCallback		/* The callback function that switches the LED off. */
										);

	/* Start the software timer. */
	xTimerStart( xExampleSoftwareTimer, mainDONT_BLOCK );

	/* Start the tasks and timer running. */
	vTaskStartScheduler();

	/* If all is well, the scheduler will now be running, and the following line
	will never be reached.  If the following line does execute, then there was
	insufficient FreeRTOS heap memory available for the idle and/or timer tasks
	to be created.  See the memory management section on the FreeRTOS web site
	for more details. */
	for( ;; );
}
/*-----------------------------------------------------------*/

/* The callback is executed when the software timer expires. */
static void vSoftwareTimerCallback( TimerHandle_t xTimer )
{
	/* Just increment the ulCallbac variable. */
	ulCallback++;
}
/*-----------------------------------------------------------*/

static void prvQueueSendTask( void *pvParameters )
{
TickType_t xNextWakeTime;
const unsigned long ulValueToSend = 100UL;

	/* Initialise xNextWakeTime - this only needs to be done once. */
	xNextWakeTime = xTaskGetTickCount();

	for( ;; )
	{
		/* Place this task in the blocked state until it is time to run again.
		The block time is specified in ticks, the constant used converts ticks
		to ms.  While in the Blocked state this task will not consume any CPU
		time. */
		vTaskDelayUntil( &xNextWakeTime, mainQUEUE_SEND_FREQUENCY_MS );

		/* Send to the queue - causing the queue receive task to unblock and
		toggle an LED.  0 is used as the block time so the sending operation
		will not block - it shouldn't need to block as the queue should always
		be empty at this point in the code. */
		xQueueSend( xQueue, &ulValueToSend, mainDONT_BLOCK );
	}
}
/*-----------------------------------------------------------*/

static void prvQueueReceiveTask( void *pvParameters )
{
unsigned long ulReceivedValue;

	for( ;; )
	{
		/* Wait until something arrives in the queue - this task will block
		indefinitely provided INCLUDE_vTaskSuspend is set to 1 in
		FreeRTOSConfig.h. */
		xQueueReceive( xQueue, &ulReceivedValue, portMAX_DELAY );

		/*  To get here something must have been received from the queue, but
		is it the expected value?  If it is, increment the ulReceived variable. */
		if( ulReceivedValue == 100UL )
		{
			ulReceived++;
		}
	}
}
/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
	/* vApplicationMallocFailedHook() will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	function that will get called if a call to pvPortMalloc() fails.
	pvPortMalloc() is called internally by the kernel whenever a task, queue or
	semaphore is created.  It is also called by various parts of the demo
	application.  If heap_1.c or heap_2.c are used, then the size of the heap
	available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
	FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
	to query the size of free heap space that remains (although it does not
	provide information on how the remaining heap might be fragmented). */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t *pxTask, signed char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* vApplicationStackOverflowHook() will only be called if
	configCHECK_FOR_STACK_OVERFLOW is set to either 1 or 2.  The handle and name
	of the offending task will be passed into the hook function via its
	parameters.  However, when a stack has overflowed, it is possible that the
	parameters will have been corrupted, in which case the pxCurrentTCB variable
	can be inspected directly. */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
	/* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
	to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
	task.  It is essential that code added to this hook function never attempts
	to block in any way (for example, call xQueueReceive() with a block time
	specified, or call vTaskDelay()).  If the application makes use of the
	vTaskDelete() API function (as this demo application does) then it is also
	important that vApplicationIdleHook() is permitted to return to its calling
	function, because it is the responsibility of the idle task to clean up
	memory allocated by the kernel to any task that has since been deleted. */
}
/*-----------------------------------------------------------*/

void vApplicationTickHook( void )
{
	/* vApplicationTickHook() will only be called if configUSE_TICK_HOOK is set
	to 1 in FreeRTOSConfig.h.  It executes from an interrupt context so must
	not use any FreeRTOS API functions that do not end in ...FromISR().

	This simple blinky demo does not use the tick hook, but a tick hook is
	required to be defined as the blinky and full demos share a
	FreeRTOSConfig.h header file. */
}
/*-----------------------------------------------------------*/

/* This is an application defined callback function used to install the tick
interrupt handler.  It is provided as an application callback because the kernel
will run on lots of different MicroBlaze and FPGA configurations - there could
be multiple timer instances in the hardware platform and the users can chose to
use any one of them. This example uses Timer 0. If that is available in  your
hardware platform then this example callback implementation should not require
modification. The definitions for the timer instance used are at the top of this
file so that users can change them at one place based on the timer instance they
use. The name of the interrupt handler that should be installed is vPortTickISR(),
which the function below declares as an extern. */
void vApplicationSetupTimerInterrupt( void )
{
portBASE_TYPE xStatus;
const unsigned char ucTimerCounterNumber = ( unsigned char ) 0U;
const unsigned long ulCounterValue = ( ( TIMER_FREQ_HZ / configTICK_RATE_HZ ) - 1UL );
extern void vPortTickISR( void *pvUnused );

	/* Initialise the timer/counter. */
	xStatus = XTmrCtr_Initialize( &xTimer0Instance, TIMER_DEVICE_ID );

	if( xStatus == XST_SUCCESS )
	{
		/* Install the tick interrupt handler as the timer ISR.
		*NOTE* The xPortInstallInterruptHandler() API function must be used for
		this purpose. */
		xStatus = xPortInstallInterruptHandler( TIMER_INTR_ID, vPortTickISR, NULL );
	}

	if( xStatus == pdPASS )
	{
		/* Enable the timer interrupt in the interrupt controller.
		*NOTE* The vPortEnableInterrupt() API function must be used for this
		purpose. */
		vPortEnableInterrupt( TIMER_INTR_ID );

		/* Configure the timer interrupt handler. */
		XTmrCtr_SetHandler( &xTimer0Instance, ( void * ) vPortTickISR, NULL );

		/* Set the correct period for the timer. */
		XTmrCtr_SetResetValue( &xTimer0Instance, ucTimerCounterNumber, ulCounterValue );

		/* Enable the interrupts.  Auto-reload mode is used to generate a
		periodic tick.  Note that interrupts are disabled when this function is
		called, so interrupts will not start to be processed until the first
		task has started to run. */
		XTmrCtr_SetOptions( &xTimer0Instance, ucTimerCounterNumber, ( XTC_INT_MODE_OPTION | XTC_AUTO_RELOAD_OPTION | XTC_DOWN_COUNT_OPTION ) );

		/* Start the timer. */
		XTmrCtr_Start( &xTimer0Instance, ucTimerCounterNumber );
	}

	/* Sanity check that the function executed as expected. */
	configASSERT( ( xStatus == pdPASS ) );
}
/*-----------------------------------------------------------*/

/* This is an application defined callback function used to clear whichever
interrupt was installed by the the vApplicationSetupTimerInterrupt() callback
function - in this case the interrupt generated by the AXI timer.  It is
provided as an application callback because the kernel will run on lots of
different MicroBlaze and FPGA configurations - not all of which will have the
same timer peripherals defined or available.  This example uses the AXI Timer 0.
If that is available on your hardware platform then this example callback
implementation should not require modification provided the example definition
of vApplicationSetupTimerInterrupt() is also not modified. */
void vApplicationClearTimerInterrupt( void )
{
unsigned long ulCSR;

	/* Clear the timer interrupt */
	ulCSR = XTmrCtr_GetControlStatusReg( XPAR_TMRCTR_0_BASEADDR, 0 );
	XTmrCtr_SetControlStatusReg( XPAR_TMRCTR_0_BASEADDR, 0, ulCSR );
}
/*-----------------------------------------------------------*/

