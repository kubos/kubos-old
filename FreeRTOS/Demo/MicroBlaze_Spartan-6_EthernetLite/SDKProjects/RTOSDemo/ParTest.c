/*
    FreeRTOS V7.6.0 - Copyright (C) 2013 Real Time Engineers Ltd. 
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that has become a de facto standard.             *
     *                                                                       *
     *    Help yourself get started quickly and support the FreeRTOS         *
     *    project by purchasing a FreeRTOS tutorial book, reference          *
     *    manual, or both from: http://www.FreeRTOS.org/Documentation        *
     *                                                                       *
     *    Thank you!                                                         *
     *                                                                       *
    ***************************************************************************

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>!AND MODIFIED BY!<< the FreeRTOS exception.

    >>! NOTE: The modification to the GPL is included to allow you to distribute
    >>! a combined work that includes FreeRTOS without being obliged to provide
    >>! the source code for proprietary components outside of the FreeRTOS
    >>! kernel.

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available from the following
    link: http://www.freertos.org/a00114.html

    1 tab == 4 spaces!

    ***************************************************************************
     *                                                                       *
     *    Having a problem?  Start by reading the FAQ "My application does   *
     *    not run, what could be wrong?"                                     *
     *                                                                       *
     *    http://www.FreeRTOS.org/FAQHelp.html                               *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org - Documentation, books, training, latest versions,
    license and Real Time Engineers Ltd. contact details.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.OpenRTOS.com - Real Time Engineers ltd license FreeRTOS to High
    Integrity Systems to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

/*-----------------------------------------------------------
 * Simple digital IO routines.
 *-----------------------------------------------------------*/

/* Kernel includes. */
#include "FreeRTOS.h"

/* Demo application includes. */
#include "partest.h"

/* Library includes. */
#include "xgpio.h"

/* The hardware design that accompanies this demo project has four LED 
outputs. */
#define partstMAX_LED	4

/*-----------------------------------------------------------*/

/* A hardware specific constant required to use the Xilinx driver library. */
static const unsigned portBASE_TYPE uxGPIOOutputChannel = 1UL;

/* The current state of the output port. */
static unsigned char ucGPIOState = 0U;

/* Structure that hold the state of the ouptut peripheral used by this demo.
This is used by the Xilinx peripheral driver API functions. */
static XGpio xOutputGPIOInstance;

/*
 * Setup the IO for the LED outputs.
 */
void vParTestInitialise( void )
{
portBASE_TYPE xStatus;
const unsigned char ucSetToOutput = 0U;

	/* Initialise the GPIO for the LEDs. */
	xStatus = XGpio_Initialize( &xOutputGPIOInstance, XPAR_LEDS_4BITS_DEVICE_ID );
	if( xStatus == XST_SUCCESS )
	{
		/* All bits on this channel are going to be outputs (LEDs). */
		XGpio_SetDataDirection( &xOutputGPIOInstance, uxGPIOOutputChannel, ucSetToOutput );

		/* Start with all LEDs off. */
		ucGPIOState = 0U;
		XGpio_DiscreteWrite( &xOutputGPIOInstance, uxGPIOOutputChannel, ucGPIOState );
	}
	
	configASSERT( xStatus == XST_SUCCESS );
}
/*-----------------------------------------------------------*/

void vParTestSetLED( unsigned portBASE_TYPE uxLED, signed portBASE_TYPE xValue )
{
unsigned char ucLED = 1U;

	/* Only attempt to set the LED if it is in range. */
	if( uxLED < partstMAX_LED )
	{
		ucLED <<= ( unsigned char ) uxLED;

		portENTER_CRITICAL();
		{
			if( xValue == pdFALSE )
			{
				ucGPIOState &= ~ucLED;
			}
			else
			{
				ucGPIOState |= ucLED;
			}
			XGpio_DiscreteWrite( &xOutputGPIOInstance, uxGPIOOutputChannel, ucGPIOState );
		}
		portEXIT_CRITICAL();
	}
}
/*-----------------------------------------------------------*/

void vParTestToggleLED( unsigned portBASE_TYPE uxLED )
{
unsigned char ucLED = 1U;

	/* Only attempt to toggle the LED if it is in range. */
	if( uxLED < partstMAX_LED )
	{
		ucLED <<= ( unsigned char ) uxLED;

		portENTER_CRITICAL();
		{
			if( ( ucGPIOState & ucLED ) != 0 )
			{
				ucGPIOState &= ~ucLED;
			}
			else
			{
				ucGPIOState |= ucLED;
			}

			XGpio_DiscreteWrite( &xOutputGPIOInstance, uxGPIOOutputChannel, ucGPIOState );
		}
		portEXIT_CRITICAL();
	}
}



