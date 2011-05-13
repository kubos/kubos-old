/*
    FreeRTOS V7.0.1 - Copyright (C) 2011 Real Time Engineers Ltd.
	

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

/*-----------------------------------------------------------
 * Simple IO routines to control the LEDs.
 *-----------------------------------------------------------*/

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Demo includes. */
#include "partest.h"

#define partestNUM_LEDS ( 6 )
#define partestALL_LEDS ( usLEDMasks[ 0 ] | usLEDMasks[ 1 ] | usLEDMasks[ 2 ] | usLEDMasks[ 3 ] | usLEDMasks[ 4 ] | usLEDMasks[ 5 ] )

static const unsigned short usLEDMasks[ partestNUM_LEDS ] = { ( 1 << 9 ), ( 1 << 11 ), ( 1 << 12 ), ( 1 << 13 ), ( 1 << 14 ), ( 1 << 15 ) };
/*-----------------------------------------------------------*/

void vParTestInitialise( void )
{
	/* Select port functions for PE9 to PE15. */
	PFC.PECRL3.WORD &= ( unsigned short ) ~partestALL_LEDS;

	/* Turn all LEDs off. */
	PE.DR.WORD &= ( unsigned short ) ~partestALL_LEDS;
	
	/* Set all LEDs to output. */
	PFC.PEIORL.WORD |= ( unsigned short ) partestALL_LEDS;
}
/*-----------------------------------------------------------*/

void vParTestSetLED( unsigned portBASE_TYPE uxLED, signed portBASE_TYPE xValue )
{
	if( uxLED < partestNUM_LEDS )
	{
		if( xValue )
		{
			/* Turn the LED on. */
			taskENTER_CRITICAL();
			{
				PE.DR.WORD |= usLEDMasks[ uxLED ];
			}
			taskEXIT_CRITICAL();
		}
		else
		{
			/* Turn the LED off. */
			taskENTER_CRITICAL();
			{
				PE.DR.WORD &= ( unsigned short ) ~usLEDMasks[ uxLED ];
			}
			taskEXIT_CRITICAL();
		}
	}
}
/*-----------------------------------------------------------*/

void vParTestToggleLED( unsigned portBASE_TYPE uxLED )
{
	if( uxLED < partestNUM_LEDS )
	{
		taskENTER_CRITICAL();
		{
			if( ( PE.DR.WORD & usLEDMasks[ uxLED ] ) != 0x00 )
			{
				PE.DR.WORD &= ( unsigned short ) ~usLEDMasks[ uxLED ];
			}
			else
			{
				PE.DR.WORD |= usLEDMasks[ uxLED ];
			}
		}
		taskEXIT_CRITICAL();
	}
}
/*-----------------------------------------------------------*/
							
long lParTestGetLEDState( void )
{
	/* Returns the state of the fifth LED. */
	return !( PE.DR.WORD & usLEDMasks[ 4 ] );
}
/*-----------------------------------------------------------*/




							