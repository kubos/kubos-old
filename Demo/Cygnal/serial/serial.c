/*
	FreeRTOS.org V5.1.2 - Copyright (C) 2003-2009 Richard Barry.

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
    ***************************************************************************
    *                                                                         *
    * Get the FreeRTOS eBook!  See http://www.FreeRTOS.org/Documentation      *
	*                                                                         *
	* This is a concise, step by step, 'hands on' guide that describes both   *
	* general multitasking concepts and FreeRTOS specifics. It presents and   *
	* explains numerous examples that are written using the FreeRTOS API.     *
	* Full source code for all the examples is provided in an accompanying    *
	* .zip file.                                                              *
    *                                                                         *
    ***************************************************************************
    ***************************************************************************

	Please ensure to read the configuration and relevant port sections of the
	online documentation.

	http://www.FreeRTOS.org - Documentation, latest information, license and 
	contact details.

	http://www.SafeRTOS.com - A version that is certified for use in safety 
	critical systems.

	http://www.OpenRTOS.com - Commercial support, development, porting, 
	licensing and training services.
*/


/* BASIC INTERRUPT DRIVEN SERIAL PORT DRIVER FOR DEMO PURPOSES */
#include <stdlib.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "serial.h"

/* Constants required to setup the serial control register. */
#define ser8_BIT_MODE			( ( unsigned portCHAR ) 0x40 )
#define serRX_ENABLE			( ( unsigned portCHAR ) 0x10 )

/* Constants to setup the timer used to generate the baud rate. */
#define serCLOCK_DIV_48			( ( unsigned portCHAR ) 0x03 )
#define serUSE_PRESCALED_CLOCK	( ( unsigned portCHAR ) 0x10 )
#define ser8BIT_WITH_RELOAD		( ( unsigned portCHAR ) 0x20 )
#define serSMOD					( ( unsigned portCHAR ) 0x10 )

static xQueueHandle xRxedChars; 
static xQueueHandle xCharsForTx; 

data static unsigned portBASE_TYPE uxTxEmpty;

/*-----------------------------------------------------------*/

xComPortHandle xSerialPortInitMinimal( unsigned portLONG ulWantedBaud, unsigned portBASE_TYPE uxQueueLength )
{
unsigned portLONG ulReloadValue;
const portFLOAT fBaudConst = ( portFLOAT ) configCPU_CLOCK_HZ * ( portFLOAT ) 2.0;
unsigned portCHAR ucOriginalSFRPage;

	portENTER_CRITICAL();
	{
		ucOriginalSFRPage = SFRPAGE;
		SFRPAGE = 0;

		uxTxEmpty = pdTRUE;

		/* Create the queues used by the com test task. */
		xRxedChars = xQueueCreate( uxQueueLength, ( unsigned portBASE_TYPE ) sizeof( portCHAR ) );
		xCharsForTx = xQueueCreate( uxQueueLength, ( unsigned portBASE_TYPE ) sizeof( portCHAR ) );
	
		/* Calculate the baud rate to use timer 1. */
		ulReloadValue = ( unsigned portLONG ) ( ( ( portFLOAT ) 256 - ( fBaudConst / ( portFLOAT ) ( 32 * ulWantedBaud ) ) ) + ( portFLOAT ) 0.5 );

		/* Set timer one for desired mode of operation. */
		TMOD &= 0x08;
		TMOD |= ser8BIT_WITH_RELOAD;
		SSTA0 |= serSMOD;

		/* Set the reload and start values for the time. */
		TL1 = ( unsigned portCHAR ) ulReloadValue;
		TH1 = ( unsigned portCHAR ) ulReloadValue;

		/* Setup the control register for standard n, 8, 1 - variable baud rate. */
		SCON = ser8_BIT_MODE | serRX_ENABLE;

		/* Enable the serial port interrupts */
		ES = 1;

		/* Start the timer. */
		TR1 = 1;

		SFRPAGE = ucOriginalSFRPage;
	}
	portEXIT_CRITICAL();
	
	/* Unlike some ports, this serial code does not allow for more than one
	com port.  We therefore don't return a pointer to a port structure and can
	instead just return NULL. */
	return NULL;
}
/*-----------------------------------------------------------*/

void vSerialISR( void ) interrupt 4
{
portCHAR cChar;
portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	/* 8051 port interrupt routines MUST be placed within a critical section
	if taskYIELD() is used within the ISR! */

	portENTER_CRITICAL();
	{
		if( RI ) 
		{
			/* Get the character and post it on the queue of Rxed characters.
			If the post causes a task to wake force a context switch if the woken task
			has a higher priority than the task we have interrupted. */
			cChar = SBUF;
			RI = 0;

			xQueueSendFromISR( xRxedChars, &cChar, &xHigherPriorityTaskWoken );
		}

		if( TI ) 
		{
			if( xQueueReceiveFromISR( xCharsForTx, &cChar, &xHigherPriorityTaskWoken ) == ( portBASE_TYPE ) pdTRUE )
			{
				/* Send the next character queued for Tx. */
				SBUF = cChar;
			}
			else
			{
				/* Queue empty, nothing to send. */
				uxTxEmpty = pdTRUE;
			}

			TI = 0;
		}
	
		if( xHigherPriorityTaskWoken )
		{
			portYIELD();
		}
	}
	portEXIT_CRITICAL();
}
/*-----------------------------------------------------------*/

portBASE_TYPE xSerialGetChar( xComPortHandle pxPort, signed portCHAR *pcRxedChar, portTickType xBlockTime )
{
	/* There is only one port supported. */
	( void ) pxPort;

	/* Get the next character from the buffer.  Return false if no characters
	are available, or arrive before xBlockTime expires. */
	if( xQueueReceive( xRxedChars, pcRxedChar, xBlockTime ) )
	{
		return ( portBASE_TYPE ) pdTRUE;
	}
	else
	{
		return ( portBASE_TYPE ) pdFALSE;
	}
}
/*-----------------------------------------------------------*/

portBASE_TYPE xSerialPutChar( xComPortHandle pxPort, signed portCHAR cOutChar, portTickType xBlockTime )
{
portBASE_TYPE xReturn;

	/* There is only one port supported. */
	( void ) pxPort;

	portENTER_CRITICAL();
	{
		if( uxTxEmpty == pdTRUE )
		{
			SBUF = cOutChar;
			uxTxEmpty = pdFALSE;
			xReturn = ( portBASE_TYPE ) pdTRUE;
		}
		else
		{
			xReturn = xQueueSend( xCharsForTx, &cOutChar, xBlockTime );

			if( xReturn == ( portBASE_TYPE ) pdFALSE )
			{
				xReturn = ( portBASE_TYPE ) pdTRUE;
			}
		}
	}
	portEXIT_CRITICAL();

	return xReturn;
}
/*-----------------------------------------------------------*/

void vSerialClose( xComPortHandle xPort )
{
	/* Not implemented in this port. */
	( void ) xPort;
}
/*-----------------------------------------------------------*/





