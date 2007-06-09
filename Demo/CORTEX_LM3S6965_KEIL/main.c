/*
	FreeRTOS.org V4.3.1 - Copyright (C) 2003-2007 Richard Barry.

	This file is part of the FreeRTOS.org distribution.

	FreeRTOS.org is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	FreeRTOS.org is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with FreeRTOS.org; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

	A special exception to the GPL can be applied should you wish to distribute
	a combined work that includes FreeRTOS.org, without being obliged to provide
	the source code for any proprietary components.  See the licensing section
	of http://www.FreeRTOS.org for full details of how and when the exception
	can be applied.

	***************************************************************************
	See http://www.FreeRTOS.org for documentation, latest information, license
	and contact details.  Please ensure to read the configuration and relevant
	port sections of the online documentation.
	***************************************************************************
*/


/*
 * Creates all the demo application tasks, then starts the scheduler.  The WEB
 * documentation provides more details of the standard demo application tasks.
 * In addition to the standard demo tasks, the following tasks and tests are
 * defined and/or created within this file:
 *
 * "Fast Interrupt Test" - A high frequency periodic interrupt is generated
 * using a free running timer to demonstrate the use of the
 * configKERNEL_INTERRUPT_PRIORITY configuration constant.  The interrupt
 * service routine measures the number of processor clocks that occur between
 * each interrupt - and in so doing measures the jitter in the interrupt timing.
 * The maximum measured jitter time is latched in the ulMaxJitter variable, and
 * displayed on the OLED display by the 'Check' task as described below.  The
 * fast interrupt is configured and handled in the timertest.c source file.
 *
 * "OLED" task - the OLED task is a 'gatekeeper' task.  It is the only task that
 * is permitted to access the display directly.  Other tasks wishing to write a
 * message to the OLED send the message on a queue to the OLED task instead of
 * accessing the OLED themselves.  The OLED task just blocks on the queue waiting
 * for messages - waking and displaying the messages as they arrive.
 *
 * "Check" task -  This only executes every five seconds but has the highest
 * priority so is guaranteed to get processor time.  Its main function is to
 * check that all the standard demo tasks are still operational.  Should any
 * unexpected behaviour within a demo task be discovered the 'check' task will
 * write an error to the OLED (via the OLED task).  If all the demo tasks are
 * executing with their expected behaviour then the check task writes PASS
 * along with the max jitter time to the OLED (again via the OLED task), as
 * described above.
 *
 * "uIP" task -  This is the task that handles the uIP stack.  All TCP/IP
 * processing is performed in this task.
 */



/* Standard includes. */
#include <stdio.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "Task.h"
#include "queue.h"
#include "semphr.h"

/* Demo app includes. */
#include "BlockQ.h"
#include "death.h"
#include "integer.h"
#include "blocktim.h"
#include "flash.h"
#include "partest.h"
#include "semtest.h"
#include "pollq.h"
#include "lcd_message.h"
#include "bitmap.h"

/* Hardware library includes. */
#include "hw_memmap.h"
#include "hw_types.h"
#include "sysctl.h"
#include "gpio.h"
#include "osram128x64x4.h"

/*-----------------------------------------------------------*/

/* The time between cycles of the 'check' task. */
#define mainCHECK_DELAY						( ( portTickType ) 5000 / portTICK_RATE_MS )

/* Size of the stack allocated to the uIP task. */
#define mainBASIC_WEB_STACK_SIZE            ( 200 )

/* The check task uses the sprintf function so requires a little more stack too. */
#define mainCHECK_TASK_STACK_SIZE			( configMINIMAL_STACK_SIZE + 50 )

/* Task priorities. */
#define mainQUEUE_POLL_PRIORITY				( tskIDLE_PRIORITY + 2 )
#define mainCHECK_TASK_PRIORITY				( tskIDLE_PRIORITY + 3 )
#define mainSEM_TEST_PRIORITY				( tskIDLE_PRIORITY + 1 )
#define mainBLOCK_Q_PRIORITY				( tskIDLE_PRIORITY + 2 )
#define mainCREATOR_TASK_PRIORITY           ( tskIDLE_PRIORITY + 3 )
#define mainINTEGER_TASK_PRIORITY           ( tskIDLE_PRIORITY )

/* The maximum number of message that can be waiting for display at any one
time. */
#define mainOLED_QUEUE_SIZE					( 3 )

/* Dimensions the buffer into which the jitter time is written. */
#define mainMAX_MSG_LEN						25

/* The period of the system clock in nano seconds.  This is used to calculate
the jitter time in nano seconds. */
#define mainNS_PER_CLOCK					( ( unsigned portLONG ) ( ( 1.0 / ( double ) configCPU_CLOCK_HZ ) * 1000000000.0 ) )

/* Constants used when writing strings to the display. */
#define mainCHARACTER_HEIGHT				( 9 )
#define mainMAX_ROWS						( mainCHARACTER_HEIGHT * 7 )
#define mainFULL_SCALE						( 15 )
#define ulSSI_FREQUENCY						1000000

/*-----------------------------------------------------------*/

/*
 * Checks the status of all the demo tasks then prints a message to the
 * display.  The message will be either PASS - an include in brackets the
 * maximum measured jitter time (as described at the to of the file), or a
 * message that describes which of the standard demo tasks an error has been
 * discovered in.
 *
 * Messages are not written directly to the terminal, but passed to vOLEDTask
 * via a queue.
 */
static void vCheckTask( void *pvParameters );

/*
 * The task that handles the uIP stack.  All TCP/IP processing is performed in
 * this task.
 */
extern void vuIP_Task( void *pvParameters );

/*
 * The display is written two by more than one task so is controlled by a
 * 'gatekeeper' task.  This is the only task that is actually permitted to
 * access the display directly.  Other tasks wanting to display a message send
 * the message to the gatekeeper.
 */
static void vOLEDTask( void *pvParameters );

/*
 * Configure the hardware for the demo.
 */
static void prvSetupHardware( void );

/*
 * Configures the high frequency timers - those used to measure the timing
 * jitter while the real time kernel is executing.
 */
extern void vSetupTimer( void );

/*-----------------------------------------------------------*/

/* The queue used to send messages to the OLED task. */
xQueueHandle xOLEDQueue;

/* The welcome text. */
const portCHAR * const pcWelcomeMessage = "   www.FreeRTOS.org";

/*-----------------------------------------------------------*/

int main( void )
{
	prvSetupHardware();

	/* Create the queue used by the OLED task.  Messages for display on the OLED
	are received via this queue. */
	xOLEDQueue = xQueueCreate( mainOLED_QUEUE_SIZE, sizeof( xOLEDMessage ) );

	/* Create the uIP task. */
    xTaskCreate( vuIP_Task, ( signed portCHAR * ) "uIP", mainBASIC_WEB_STACK_SIZE, NULL, mainCHECK_TASK_PRIORITY - 1, NULL );

	/* Start the standard demo tasks. */
	vStartBlockingQueueTasks( mainBLOCK_Q_PRIORITY );
    vCreateBlockTimeTasks();
    vStartSemaphoreTasks( mainSEM_TEST_PRIORITY );
    vStartPolledQueueTasks( mainQUEUE_POLL_PRIORITY );
    vStartIntegerMathTasks( mainINTEGER_TASK_PRIORITY );

	/* Start the tasks defined within this file/specific to this demo. */
    xTaskCreate( vCheckTask, ( signed portCHAR * ) "Check", mainCHECK_TASK_STACK_SIZE, NULL, mainCHECK_TASK_PRIORITY, NULL );
	xTaskCreate( vOLEDTask, ( signed portCHAR * ) "OLED", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );

	/* The suicide tasks must be created last as they need to know how many
	tasks were running prior to their creation in order to ascertain whether
	or not the correct/expected number of tasks are running at any given time. */
    vCreateSuicidalTasks( mainCREATOR_TASK_PRIORITY );

	/* Configure the high frequency interrupt used to measure the interrupt
	jitter time. */
	#ifdef __ICCARM__
		vSetupTimer();
	#endif
	
	/* Start the scheduler. */
	vTaskStartScheduler();

    /* Will only get here if there was insufficient memory to create the idle
    task. */
	return 0;
}
/*-----------------------------------------------------------*/

void prvSetupHardware( void )
{
	/* Set the clocking to run from the PLL at 50 MHz */
	SysCtlClockSet( SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ );
	
	/* Enable/Reset the Ethernet Controller */
	SysCtlPeripheralEnable( SYSCTL_PERIPH_ETH );
	SysCtlPeripheralReset( SYSCTL_PERIPH_ETH );
	
	/* 	Enable Port F for Ethernet LEDs
		LED0        Bit 3   Output
		LED1        Bit 2   Output */
	SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOF );
	GPIODirModeSet( GPIO_PORTF_BASE, (GPIO_PIN_2 | GPIO_PIN_3), GPIO_DIR_MODE_HW );
	GPIOPadConfigSet( GPIO_PORTF_BASE, (GPIO_PIN_2 | GPIO_PIN_3 ), GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD );	
	
	vParTestInitialise();
}
/*-----------------------------------------------------------*/

static void vCheckTask( void *pvParameters )
{
portTickType xLastExecutionTime;
xOLEDMessage xMessage;
static portCHAR cPassMessage[ mainMAX_MSG_LEN ];
extern unsigned portLONG ulMaxJitter;

	xLastExecutionTime = xTaskGetTickCount();
	xMessage.pcMessage = cPassMessage;
	
    for( ;; )
	{
		/* Perform this check every mainCHECK_DELAY milliseconds. */
		vTaskDelayUntil( &xLastExecutionTime, mainCHECK_DELAY );

		/* Has an error been found in any task? */

        if( xAreBlockingQueuesStillRunning() != pdTRUE )
		{
			xMessage.pcMessage = "ERROR IN BLOCK Q";
		}
		else if( xAreBlockTimeTestTasksStillRunning() != pdTRUE )
		{
			xMessage.pcMessage = "ERROR IN BLOCK TIME";
		}
        else if( xAreSemaphoreTasksStillRunning() != pdTRUE )
        {
            xMessage.pcMessage = "ERROR IN SEMAPHORE";
        }
        else if( xArePollingQueuesStillRunning() != pdTRUE )
        {
            xMessage.pcMessage = "ERROR IN POLL Q";
        }
        else if( xIsCreateTaskStillRunning() != pdTRUE )
        {
            xMessage.pcMessage = "ERROR IN CREATE";
        }
        else if( xAreIntegerMathsTaskStillRunning() != pdTRUE )
        {
            xMessage.pcMessage = "ERROR IN MATH";
        }
		else
		{
			#ifdef __ICCARM__
				sprintf( cPassMessage, "PASS [%uns]", ulMaxJitter * mainNS_PER_CLOCK );
			#else
				sprintf( cPassMessage, "PASS" );
			#endif
		}

		/* Send the message to the OLED gatekeeper for display. */
		xQueueSend( xOLEDQueue, &xMessage, portMAX_DELAY );
	}
}
/*-----------------------------------------------------------*/



void vOLEDTask( void *pvParameters )
{
xOLEDMessage xMessage;
unsigned portLONG ulY = mainMAX_ROWS;

	/* Initialise the OLED and display a startup message. */
	OSRAM128x64x4Init( ulSSI_FREQUENCY );	
	
	OSRAM128x64x4StringDraw( " POWERED BY FreeRTOS", 0, 0, mainFULL_SCALE );
	OSRAM128x64x4ImageDraw( pucImage, 0, mainCHARACTER_HEIGHT + 1, bmpBITMAP_WIDTH, bmpBITMAP_HEIGHT );
	
	for( ;; )
	{
		/* Wait for a message to arrive that requires displaying. */
		xQueueReceive( xOLEDQueue, &xMessage, portMAX_DELAY );
	
		/* Write the message on the next available row. */
		ulY += mainCHARACTER_HEIGHT;
		if( ulY >= mainMAX_ROWS )
		{
			ulY = mainCHARACTER_HEIGHT;
			OSRAM128x64x4Clear();
			OSRAM128x64x4StringDraw( pcWelcomeMessage, 0, 0, mainFULL_SCALE );			
		}

		/* Display the message. */
		OSRAM128x64x4StringDraw( xMessage.pcMessage, 0, ulY, mainFULL_SCALE );
	}
}
