/*
    FreeRTOS V6.1.0 - Copyright (C) 2010 Real Time Engineers Ltd.

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

/*-----------------------------------------------------------
 * Simple parallel port IO routines.
 *-----------------------------------------------------------*/

/* FreeRTOS.org includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "partest.h"

/* Standard includes. */
#include <string.h>

/* Library includes. */
#include "stm32f10x_lib.h"

#define partstNUM_LEDs		4

/* Holds the current output state for each of the LEDs. */
static unsigned char ucBitStates[ partstNUM_LEDs ];

/* Holds the port used by each of the LEDs. */
static GPIO_TypeDef * uxIO_Port[ partstNUM_LEDs ];

/* Holds the pin used by each of the LEDs. */
static const unsigned short uxIO_Pins[ partstNUM_LEDs ] = { GPIO_Pin_14, GPIO_Pin_13, GPIO_Pin_3, GPIO_Pin_4 };

/*-----------------------------------------------------------*/

void vParTestInitialise( void )
{
GPIO_InitTypeDef GPIO_InitStructure;

	/* Configure PE14, PD13, PD3 and PD4 output push-pull */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init( GPIOE, &GPIO_InitStructure );

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init( GPIOD, &GPIO_InitStructure );

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_3 | GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init( GPIOD, &GPIO_InitStructure );

	memset( ucBitStates, 0x00, sizeof( ucBitStates ) );

	uxIO_Port[ 0 ] =  GPIOE;
    uxIO_Port[ 1 ] =  GPIOD;
    uxIO_Port[ 2 ] =  GPIOD;
    uxIO_Port[ 3 ] =  GPIOD;
}
/*-----------------------------------------------------------*/

void vParTestSetLED( unsigned portBASE_TYPE uxLED, signed portBASE_TYPE xValue )
{
	if( uxLED < partstNUM_LEDs )
	{
		portENTER_CRITICAL();
		{
			if( xValue != pdFALSE )
			{
				ucBitStates[ uxLED ] = pdTRUE;
			}
			else
			{
				ucBitStates[ uxLED ] = pdFALSE;
			}

            GPIO_WriteBit( uxIO_Port[ uxLED ], uxIO_Pins[ uxLED ], ucBitStates[ uxLED ] );
		}
		portEXIT_CRITICAL();
	}
}
/*-----------------------------------------------------------*/

void vParTestToggleLED( unsigned portBASE_TYPE uxLED )
{
	if( uxLED < partstNUM_LEDs )
	{
		portENTER_CRITICAL();
		{
			ucBitStates[ uxLED ] = !ucBitStates[ uxLED ];
            GPIO_WriteBit( uxIO_Port[ uxLED ], uxIO_Pins[ uxLED ], ucBitStates[ uxLED ] );
		}
		portEXIT_CRITICAL();
	}
}
/*-----------------------------------------------------------*/

portBASE_TYPE xGetLEDState( unsigned portBASE_TYPE uxLED )
{
	if( uxLED < partstNUM_LEDs )
	{
		return ( portBASE_TYPE ) ucBitStates[ uxLED ];
	}
	else
	{
		return 0;
	}
}
