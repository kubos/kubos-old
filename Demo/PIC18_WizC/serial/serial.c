/*
	FreeRTOS V5.4.1 - Copyright (C) 2009 Real Time Engineers Ltd.

	This file is part of the FreeRTOS distribution.

	FreeRTOS is free software; you can redistribute it and/or modify it	under 
	the terms of the GNU General Public License (version 2) as published by the 
	Free Software Foundation and modified by the FreeRTOS exception.
	**NOTE** The exception to the GPL is included to allow you to distribute a
	combined work that includes FreeRTOS without being obliged to provide the 
	source code for proprietary components outside of the FreeRTOS kernel.  
	Alternative commercial license and support terms are also available upon 
	request.  See the licensing section of http://www.FreeRTOS.org for full 
	license details.

	FreeRTOS is distributed in the hope that it will be useful,	but WITHOUT
	ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
	FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
	more details.

	You should have received a copy of the GNU General Public License along
	with FreeRTOS; if not, write to the Free Software Foundation, Inc., 59
	Temple Place, Suite 330, Boston, MA  02111-1307  USA.


	***************************************************************************
	*                                                                         *
	* Looking for a quick start?  Then check out the FreeRTOS eBook!          *
	* See http://www.FreeRTOS.org/Documentation for details                   *
	*                                                                         *
	***************************************************************************

	1 tab == 4 spaces!

	Please ensure to read the configuration and relevant port sections of the
	online documentation.

	http://www.FreeRTOS.org - Documentation, latest information, license and
	contact details.

	http://www.SafeRTOS.com - A version that is certified for use in safety
	critical systems.

	http://www.OpenRTOS.com - Commercial support, development, porting,
	licensing and training services.
*/

/*
Changes from V3.0.0
	+ ISRcode removed. Is now pulled inline to reduce stack-usage.

Changes from V3.0.1
*/

/* BASIC INTERRUPT DRIVEN SERIAL PORT DRIVER. */

/* Scheduler header files. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "serial.h"

/* Hardware pin definitions. */
#define serTX_PIN				bTRC6
#define serRX_PIN				bTRC7

/* Bit/register definitions. */
#define serINPUT				( 1 )
#define serOUTPUT				( 0 )
#define serINTERRUPT_ENABLED 	( 1 )

/* All ISR's use the PIC18 low priority interrupt. */
#define	serLOW_PRIORITY			( 0 )

/*-----------------------------------------------------------*/

/* Queues to interface between comms API and interrupt routines. */
xQueueHandle xRxedChars; 
xQueueHandle xCharsForTx;
portBASE_TYPE xHigherPriorityTaskWoken;

/*-----------------------------------------------------------*/

xComPortHandle xSerialPortInitMinimal( unsigned portLONG ulWantedBaud, unsigned portCHAR ucQueueLength )
{
	unsigned portSHORT usSPBRG;
	
	/* Create the queues used by the ISR's to interface to tasks. */
	xRxedChars = xQueueCreate( ucQueueLength, ( unsigned portBASE_TYPE ) sizeof( portCHAR ) );
	xCharsForTx = xQueueCreate( ucQueueLength, ( unsigned portBASE_TYPE ) sizeof( portCHAR ) );

	portENTER_CRITICAL();

	/* Setup the IO pins to enable the USART IO. */
	serTX_PIN	= serINPUT;		// YES really! See datasheet
	serRX_PIN	= serINPUT;

	/* Set the TX config register. */
	TXSTA = 0b00100000;
		//	  ||||||||--bit0: TX9D	= n/a
		//	  |||||||---bit1: TRMT	= ReadOnly
		//	  ||||||----bit2: BRGH	= High speed
		//	  |||||-----bit3: SENDB = n/a
		//	  ||||------bit4: SYNC	= Asynchronous mode
		//	  |||-------bit5: TXEN	= Transmit enable
		//	  ||--------bit6: TX9	= 8-bit transmission
		//	  |---------bit7: CSRC	= n/a

	/* Set the Receive config register. */
	RCSTA = 0b10010000;
		//	  ||||||||--bit0: RX9D	= ReadOnly
		//	  |||||||---bit1: OERR	= ReadOnly
		//	  ||||||----bit2: FERR	= ReadOnly
		//	  |||||-----bit3: ADDEN	= n/a
		//	  ||||------bit4: CREN	= Enable receiver
		//	  |||-------bit5: SREN	= n/a
		//	  ||--------bit6: RX9	= 8-bit reception
		//	  |---------bit7: SPEN	= Serial port enabled

	/* Calculate the baud rate generator value.
	   We use low-speed (BRGH=0), the formula is
	   SPBRG = ( ( FOSC / Desired Baud Rate ) / 64 ) - 1 */
	usSPBRG = ( ( APROCFREQ / ulWantedBaud ) / 64 ) - 1;
	if( usSPBRG > 255 )
	{
		SPBRG = 255;
	}
	else
	{
		SPBRG = usSPBRG;
	}

	/* Set the serial interrupts to use the same priority as the tick. */
	bTXIP = serLOW_PRIORITY;
	bRCIP = serLOW_PRIORITY;

	/* Enable the Rx interrupt now, the Tx interrupt will get enabled when
	we have data to send. */
	bRCIE = serINTERRUPT_ENABLED;
	
	portEXIT_CRITICAL();

	/* Unlike other ports, this serial code does not allow for more than one
	com port.  We therefore don't return a pointer to a port structure and 
	can	instead just return NULL. */
	return NULL;
}
/*-----------------------------------------------------------*/

xComPortHandle xSerialPortInit( eCOMPort ePort, eBaud eWantedBaud, eParity eWantedParity, eDataBits eWantedDataBits, eStopBits eWantedStopBits, unsigned portCHAR ucBufferLength )
{
	/* This is not implemented in this port.
	Use xSerialPortInitMinimal() instead. */
	return NULL;
}
/*-----------------------------------------------------------*/

portBASE_TYPE xSerialGetChar( xComPortHandle pxPort, portCHAR *pcRxedChar, portTickType xBlockTime )
{
	/* Get the next character from the buffer.  Return false if no characters
	are available, or arrive before xBlockTime expires. */
	if( xQueueReceive( xRxedChars, pcRxedChar, xBlockTime ) )
	{
		return ( portCHAR ) pdTRUE;
	}

	return ( portCHAR ) pdFALSE;
}
/*-----------------------------------------------------------*/

portBASE_TYPE xSerialPutChar( xComPortHandle pxPort, portCHAR cOutChar, portTickType xBlockTime )
{
	/* Return false if after the block time there is no room on the Tx queue. */
	if( xQueueSend( xCharsForTx, ( const void * ) &cOutChar, xBlockTime ) != ( portCHAR ) pdPASS )
	{
		return pdFAIL;
	}

	/* Turn interrupt on - ensure the compiler only generates a single 
	instruction for this. */
	bTXIE = serINTERRUPT_ENABLED;

	return pdPASS;
}
/*-----------------------------------------------------------*/

void vSerialClose( xComPortHandle xPort )
{
	/* Not implemented for this port.
	To implement, turn off the interrupts and delete the memory
	allocated to the queues. */
}
