/*
	FreeRTOS.org V4.6.0 - Copyright (C) 2003-2007 Richard Barry.

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

	Also see http://www.SafeRTOS.com for an IEC 61508 compliant version along
	with commercial development and support options.
	***************************************************************************
*/


#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "SAM7_EMAC.h"
#include "AT91SAM7X256.h"

/*-----------------------------------------------------------*/

/* The semaphore used to signal the arrival of new data to the interface
task. */
static xSemaphoreHandle xSemaphore = NULL;

/* The interrupt entry point is naked so we can control the context saving. */
void vEMACISR_Wrapper( void ) __attribute__((naked));

/* The interrupt handler function must be separate from the entry function
to ensure the correct stack frame is set up. */
void vEMACISR_Handler( void );

/*-----------------------------------------------------------*/
/*
 * The EMAC ISR.  Handles both Tx and Rx complete interrupts.
 */
void vEMACISR_Handler( void )
{
volatile unsigned portLONG ulIntStatus, ulEventStatus;
portBASE_TYPE xSwitchRequired = pdFALSE;
extern void vClearEMACTxBuffer( void );

	/* Find the cause of the interrupt. */
	ulIntStatus = AT91C_BASE_EMAC->EMAC_ISR;
	ulEventStatus = AT91C_BASE_EMAC->EMAC_RSR;

	if( ( ulIntStatus & AT91C_EMAC_RCOMP ) || ( ulEventStatus & AT91C_EMAC_REC ) )
	{
		/* A frame has been received, signal the lwIP task so it can process
		the Rx descriptors. */
		xSwitchRequired = xSemaphoreGiveFromISR( xSemaphore, pdFALSE );
		AT91C_BASE_EMAC->EMAC_RSR = AT91C_EMAC_REC;
	}

	if( ulIntStatus & AT91C_EMAC_TCOMP )
	{
		/* A frame has been transmitted.  Mark all the buffers used by the
		frame just transmitted as free again. */
		vClearEMACTxBuffer();
		AT91C_BASE_EMAC->EMAC_TSR = AT91C_EMAC_COMP;
	}

	/* Clear the interrupt. */
	AT91C_BASE_AIC->AIC_EOICR = 0;

	/* If a task was woken by either a frame being received then we may need to 
	switch to another task.  If the unblocked task was of higher priority then
	the interrupted task it will then execute immediately that the ISR
	completes. */
	if( xSwitchRequired )
	{
		portYIELD_FROM_ISR();
	}
}
/*-----------------------------------------------------------*/

void  vEMACISR_Wrapper( void )
{
	/* Save the context of the interrupted task. */
	portSAVE_CONTEXT();

	/* Call the handler to do the work.  This must be a separate
	function to ensure the stack frame is set up correctly. */
	vEMACISR_Handler();

	/* Restore the context of whichever task will execute next. */
	portRESTORE_CONTEXT();
}
/*-----------------------------------------------------------*/

void vPassEMACSemaphore( xSemaphoreHandle xCreatedSemaphore )
{
	/* Simply store the semaphore that should be used by the ISR. */
	xSemaphore = xCreatedSemaphore;
}

