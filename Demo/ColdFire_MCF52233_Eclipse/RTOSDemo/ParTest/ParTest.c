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

#include "FreeRTOS.h"
#include "task.h"
#include "partest.h"

#define partstNUM_LEDs	4

/*-----------------------------------------------------------
 * Simple parallel port IO routines.
 *-----------------------------------------------------------*/

void vParTestInitialise( void )
{
    /* Ensure LED outputs are set to GPIO */
    MCF_GPIO_PTCPAR = MCF_GPIO_PTCPAR_DTIN3_GPIO | MCF_GPIO_PTCPAR_DTIN2_GPIO | MCF_GPIO_PTCPAR_DTIN1_GPIO | MCF_GPIO_PTCPAR_DTIN0_GPIO;

    /* Set GPIO to outputs. */
    MCF_GPIO_DDRTC = MCF_GPIO_DDRTC_DDRTC3 | MCF_GPIO_DDRTC_DDRTC2 | MCF_GPIO_DDRTC_DDRTC1 | MCF_GPIO_DDRTC_DDRTC0;

    /* Start with all LEDs off. */
	MCF_GPIO_PORTTC = 0x00;
}
/*-----------------------------------------------------------*/

void vParTestSetLED( unsigned portBASE_TYPE uxLED, signed portBASE_TYPE xValue )
{
	if( uxLED < partstNUM_LEDs )
	{
		if( xValue != 0 )
		{
			taskENTER_CRITICAL();
				MCF_GPIO_PORTTC |= ( 1 << uxLED );
			taskEXIT_CRITICAL();
		}
		else
		{
			taskENTER_CRITICAL();
				MCF_GPIO_PORTTC &= ~( 1 << uxLED );
			taskEXIT_CRITICAL();
		}
	}
}
/*-----------------------------------------------------------*/

void vParTestToggleLED( unsigned portBASE_TYPE uxLED )
{
	if( uxLED < partstNUM_LEDs )
	{
		taskENTER_CRITICAL();
		{
			if( ( MCF_GPIO_PORTTC & ( 1 << uxLED ) ) == ( unsigned portCHAR ) 0 )
			{
				MCF_GPIO_PORTTC |= ( 1 << uxLED );
			}
			else
			{
				MCF_GPIO_PORTTC &= ~( 1 << uxLED );
			}
		}
		taskEXIT_CRITICAL();
	}
}
/*-----------------------------------------------------------*/

unsigned portBASE_TYPE uxParTestGetLED( unsigned portBASE_TYPE uxLED )
{
unsigned portBASE_TYPE uxReturn = pdFALSE;

	if( uxLED < partstNUM_LEDs )
	{
		if( ( MCF_GPIO_PORTTC & ( 1 << uxLED ) ) != 0 )
		{
			uxReturn = pdTRUE;
		}
	}

	return uxReturn;
}



