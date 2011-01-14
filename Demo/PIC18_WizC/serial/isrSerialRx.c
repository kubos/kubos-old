/*
    FreeRTOS V6.1.1 - Copyright (C) 2011 Real Time Engineers Ltd.

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

/*
Changes from V3.0.0
	+ ISRcode pulled inline to reduce stack-usage.

	+ Added functionality to only call vTaskSwitchContext() once
	  when handling multiple interruptsources in a single interruptcall.

	+ Filename changed to a .c extension to allow stepping through code
	  using F7.

Changes from V3.0.1
*/

#ifndef _FREERTOS_SERIAL_ISRSERIALRX_C
#define _FREERTOS_SERIAL_ISRSERIALRX_C

#define serCONTINUOUS_RX		( 1 )
#define serCLEAR_OVERRUN		( 0 )

{
	/*
	 * Was the interrupt a byte being received?
	 */
	if( bRCIF && bRCIE)
	{
		/*
		 * Queue to interface between comms API and interrupt routine.
		*/
		extern xQueueHandle xRxedChars;
		
		/*
		 * Because we are not allowed to use local variables here,
		 * PRODL is (ab)used as temporary storage.  This is allowed
		 * because this SFR will be restored before exiting the ISR.
		 */
		extern char			cChar;
		extern portBASE_TYPE xHigherPriorityTaskWoken;
		#pragma locate cChar	&PRODL

		/*
		 * If there was a framing error, just get and ignore
		 * the character
		 */
		if( bFERR )
		{
			cChar = RCREG;
		}
		else
		{
			/*
			 * Get the character and post it on the queue of Rxed
			 * characters. If the post causes a task to wake ask
			 * for a context switch as the woken task may have a
			 * higher priority than the task we have interrupted.
			 */
			cChar = RCREG;

			/*
			 * Clear any overrun errors.
			 */
			if( bOERR )
			{
				bCREN = serCLEAR_OVERRUN;
				bCREN = serCONTINUOUS_RX;	
			}

			xHigherPriorityTaskWoken = pdFALSE;
			xQueueSendFromISR( xRxedChars, ( const void * ) &cChar, &xHigherPriorityTaskWoken );

			if( xHigherPriorityTaskWoken )
			{
				uxSwitchRequested = pdTRUE;
			}
		}
	}
}

#endif	/* _FREERTOS_SERIAL_ISRSERIALRX_C */
