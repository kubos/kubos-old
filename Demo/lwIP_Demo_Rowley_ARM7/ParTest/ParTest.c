/*
	FreeRTOS.org V4.6.1 - Copyright (C) 2003-2007 Richard Barry.

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

	Also see http://www.SafeRTOS.com a version that has been certified for use
	in safety critical systems, plus commercial licensing, development and
	support options.
	***************************************************************************
*/

/* Scheduler includes. */
#include "FreeRTOS.h"

/* Demo application includes. */
#include "partest.h"

/* Hardware specific includes. */
#include "Board.h"


/*-----------------------------------------------------------
 * Simple parallel port IO routines for the LED's.  LED's can be set, cleared
 * or toggled.
 *-----------------------------------------------------------*/
const unsigned portLONG ulLED_MASK[ NB_LED ]= { LED1, LED2, LED3, LED4 };

void vParTestInitialise( void )
{	
	/* Start with all LED's off. */
    AT91C_BASE_PIOB->PIO_SODR = LED_MASK;
}
/*-----------------------------------------------------------*/

void vParTestSetLED( unsigned portBASE_TYPE uxLED, signed portBASE_TYPE xValue )
{
	if( uxLED < ( portBASE_TYPE ) NB_LED )
	{
		if( xValue )
		{
			AT91C_BASE_PIOB->PIO_SODR = ulLED_MASK[ uxLED ];
		}
		else
		{
			AT91C_BASE_PIOB->PIO_CODR = ulLED_MASK[ uxLED ];
		}
	}
}
/*-----------------------------------------------------------*/

void vParTestToggleLED( unsigned portBASE_TYPE uxLED )
{
	if( uxLED < ( portBASE_TYPE ) NB_LED )
	{
		if( AT91C_BASE_PIOB->PIO_PDSR & ulLED_MASK[ uxLED ] )
		{
			AT91C_BASE_PIOB->PIO_CODR = ulLED_MASK[ uxLED ];
		}
		else
		{
			AT91C_BASE_PIOB->PIO_SODR = ulLED_MASK[ uxLED ];
		}
	}
}

