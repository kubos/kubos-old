/*
	FreeRTOS.org V4.2.0 - Copyright (C) 2003-2007 Richard Barry.

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
		extern portCHAR			cChar;
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

			if( xQueueSendFromISR( xRxedChars, ( const void * ) &cChar, pdFALSE ) )
			{
				uxSwitchRequested = pdTRUE;
			}
		}
	}
}

#endif	/* _FREERTOS_SERIAL_ISRSERIALRX_C */
