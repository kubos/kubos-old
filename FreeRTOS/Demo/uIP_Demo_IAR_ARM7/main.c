/*
    FreeRTOS V7.4.1 - Copyright (C) 2013 Real Time Engineers Ltd.

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

/*
 * Creates all the application tasks, then starts the scheduler.
 *
 * A task is also created called "uIP".  This executes the uIP stack and small
 * WEB server sample.  All the other tasks are from the set of standard
 * demo tasks.  The WEB documentation provides more details of the standard
 * demo application tasks.
 *
 * Main.c also creates a task called "Check".  This only executes every three
 * seconds but has the highest priority so is guaranteed to get processor time.
 * Its main function is to check the status of all the other demo application
 * tasks.  LED mainCHECK_LED is toggled every three seconds by the check task
 * should no error conditions be detected in any of the standard demo tasks.
 * The toggle rate increasing to 500ms indicates that at least one error has
 * been detected.
 */


/* Standard includes. */
#include <stdlib.h>
#include <string.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Demo application includes. */
#include "partest.h"
#include "PollQ.h"
#include "dynamic.h"
#include "semtest.h"
#include "flash.h"
#include "integer.h"
#include "flop.h"
#include "BlockQ.h"
#include "death.h"
#include "uip_task.h"

/*-----------------------------------------------------------*/

/* Priorities/stacks for the demo application tasks. */
#define mainQUEUE_POLL_PRIORITY		( tskIDLE_PRIORITY + 1 )
#define mainCHECK_TASK_PRIORITY		( tskIDLE_PRIORITY + 3 )
#define mainSEM_TEST_PRIORITY		( tskIDLE_PRIORITY + 1 )
#define mainUIP_PRIORITY			( tskIDLE_PRIORITY + 3 )
#define mainFLASH_PRIORITY			( tskIDLE_PRIORITY + 2 )
#define mainBLOCK_Q_PRIORITY		( tskIDLE_PRIORITY + 1 )
#define mainDEATH_PRIORITY			( tskIDLE_PRIORITY + 2 )
#define mainUIP_TASK_STACK_SIZE		( 250 )

/* The rate at which the on board LED will toggle when there is/is not an
error. */
#define mainNO_ERROR_FLASH_PERIOD	( ( portTickType ) 3000 / portTICK_RATE_MS  )
#define mainERROR_FLASH_PERIOD		( ( portTickType ) 500 / portTICK_RATE_MS  )

/* The LED used by the check task to indicate the system status. */
#define mainCHECK_LED 				( 3 )
/*-----------------------------------------------------------*/

/*
 * Checks that all the demo application tasks are still executing without error
 * - as described at the top of the file.
 */
static long prvCheckOtherTasksAreStillRunning( void );

/*
 * The task that executes at the highest priority and calls
 * prvCheckOtherTasksAreStillRunning().  See the description at the top
 * of the file.
 */
static void vErrorChecks( void *pvParameters );

/*
 * Configure the processor for use with the Atmel demo board.  This is very
 * minimal as most of the setup is performed in the startup code.
 */
static void prvSetupHardware( void );

/*-----------------------------------------------------------*/

/*
 * Starts all the other tasks, then starts the scheduler.
 */
int main( void )
{
	/* Configure the processor. */
	prvSetupHardware();

	/* Setup the port used to flash the LED's. */
	vParTestInitialise();

	/* Start the task that handles the TCP/IP and WEB server functionality. */
    xTaskCreate( vuIP_TASK, "uIP", mainUIP_TASK_STACK_SIZE, NULL, mainUIP_PRIORITY, NULL );
	
	/* Start the demo/test application tasks.  These are created in addition
	to the TCP/IP task for demonstration and test purposes. */
	vStartPolledQueueTasks( mainQUEUE_POLL_PRIORITY );
	vStartDynamicPriorityTasks();
	vStartSemaphoreTasks( mainSEM_TEST_PRIORITY );
	vStartLEDFlashTasks( mainFLASH_PRIORITY );
	vStartIntegerMathTasks( tskIDLE_PRIORITY );
	vStartMathTasks( tskIDLE_PRIORITY );
	vStartBlockingQueueTasks( mainBLOCK_Q_PRIORITY );

	/* Start the check task - which is defined in this file. */	
    xTaskCreate( vErrorChecks, "Check", configMINIMAL_STACK_SIZE, NULL, mainCHECK_TASK_PRIORITY, NULL );

	/* Must be last to get created. */
	vCreateSuicidalTasks( mainDEATH_PRIORITY );

	/* Now all the tasks have been started - start the scheduler. */
	vTaskStartScheduler();

	/* Should never reach here because the tasks should now be executing! */
	return 0;
}
/*-----------------------------------------------------------*/

static void prvSetupHardware( void )
{
	/* When using the JTAG debugger the hardware is not always initialised to
	the correct default state.  This line just ensures that this does not
	cause all interrupts to be masked at the start. */
	AT91C_BASE_AIC->AIC_EOICR = 0;
	
	/* Most setup is performed by the low level init function called from the
	startup asm file.

	Configure the PIO Lines corresponding to LED1 to LED4 to be outputs as
	well as the UART Tx line. */
	AT91F_PIO_CfgOutput( AT91C_BASE_PIOB, LED_MASK );

	/* Enable the peripheral clock. */
	AT91F_PMC_EnablePeriphClock( AT91C_BASE_PMC, 1 << AT91C_ID_PIOA );
	AT91F_PMC_EnablePeriphClock( AT91C_BASE_PMC, 1 << AT91C_ID_PIOB ) ;
	AT91F_PMC_EnablePeriphClock( AT91C_BASE_PMC, 1 << AT91C_ID_EMAC ) ;
}
/*-----------------------------------------------------------*/

static void vErrorChecks( void *pvParameters )
{
portTickType xDelayPeriod = mainNO_ERROR_FLASH_PERIOD;
portTickType xLastWakeTime;

	/* Initialise xLastWakeTime to ensure the first call to vTaskDelayUntil()
	functions correctly. */
	xLastWakeTime = xTaskGetTickCount();

	/* Cycle for ever, delaying then checking all the other tasks are still
	operating without error.  If an error is detected then the delay period
	is decreased from mainNO_ERROR_FLASH_PERIOD to mainERROR_FLASH_PERIOD so
	the Check LED flash rate will increase. */
	for( ;; )
	{
		/* Delay until it is time to execute again.  The delay period is
		shorter following an error. */
		vTaskDelayUntil( &xLastWakeTime, xDelayPeriod );
	
		/* Check all the standard demo application tasks are executing without
		error.  */
		if( prvCheckOtherTasksAreStillRunning() != pdPASS )
		{
			/* An error has been detected in one of the tasks - flash faster. */
			xDelayPeriod = mainERROR_FLASH_PERIOD;
		}

		vParTestToggleLED( mainCHECK_LED );
	}
}
/*-----------------------------------------------------------*/

static long prvCheckOtherTasksAreStillRunning( void )
{
long lReturn = ( long ) pdPASS;


	/* Check all the demo tasks (other than the flash tasks) to ensure
	that they are all still running, and that none of them have detected
	an error. */

	if( xArePollingQueuesStillRunning() != pdTRUE )
	{
		lReturn = ( long ) pdFAIL;
	}

	if( xAreDynamicPriorityTasksStillRunning() != pdTRUE )
	{
		lReturn = ( long ) pdFAIL;
	}

	if( xAreSemaphoreTasksStillRunning() != pdTRUE )
	{
		lReturn = ( long ) pdFAIL;
	}

	if( xAreIntegerMathsTaskStillRunning() != pdTRUE )
	{
		lReturn = ( long ) pdFAIL;
	}

	if( xAreMathsTaskStillRunning() != pdTRUE )
	{
		lReturn = ( long ) pdFAIL;
	}

	if( xAreBlockingQueuesStillRunning() != pdTRUE )
	{
		lReturn = ( long ) pdFAIL;
	}

	if( xIsCreateTaskStillRunning() != pdTRUE )
	{
		lReturn = ( long ) pdFAIL;
	}

	return lReturn;
}



