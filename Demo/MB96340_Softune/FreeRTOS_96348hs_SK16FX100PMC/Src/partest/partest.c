/*
    FreeRTOS V6.0.0 - Copyright (C) 2009 Real Time Engineers Ltd.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it    under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation and modified by the FreeRTOS exception.
    **NOTE** The exception to the GPL is included to allow you to distribute a
    combined work that includes FreeRTOS without being obliged to provide the
    source code for proprietary components outside of the FreeRTOS kernel.
    Alternative commercial license and support terms are also available upon
    request.  See the licensing section of http://www.FreeRTOS.org for full
    license details.

    FreeRTOS is distributed in the hope that it will be useful,    but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
    more details.

    You should have received a copy of the GNU General Public License along
    with FreeRTOS; if not, write to the Free Software Foundation, Inc., 59
    Temple Place, Suite 330, Boston, MA  02111-1307  USA.


    ***************************************************************************
    *                                                                         *
    * The FreeRTOS eBook and reference manual are available to purchase for a *
    * small fee. Help yourself get started quickly while also helping the     *
    * FreeRTOS project! See http://www.FreeRTOS.org/Documentation for details *
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


/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"

#define partstNUM_LEDs	8

static unsigned portCHAR sState[ partstNUM_LEDs ] = { pdFALSE };
static unsigned portCHAR sState1[ partstNUM_LEDs ] = { pdFALSE };


/*-----------------------------------------------------------*/
void vParTestInitialise( void )
{
portBASE_TYPE x;

	/* Set port for LED outputs. */
	DDR00 = 0xFF;
	DDR09 = 0xFF;

	/* Start with LEDs off. */
	PDR09 = 0xff;
	PDR00 = 0xff;

	for( x = 0; x < partstNUM_LEDs; x++ )
	{
		sState[ x ] = pdTRUE;
		sState1[ x ] = pdTRUE;
	}
}
/*-----------------------------------------------------------*/

void vParTestToggleLED( unsigned portBASE_TYPE uxLED )
{
	if( uxLED < partstNUM_LEDs )
	{
		taskENTER_CRITICAL();
		{		
			/* Toggle the state of the single genuine on board LED. */
			if( sState[ uxLED ] )
			{
				PDR09 |= ( 1 << uxLED );
			}
			else
			{
				PDR09 &= ~( 1 << uxLED );
			}
		
			sState[uxLED] = !( sState[ uxLED ] );
		}		
		taskEXIT_CRITICAL();
	}
	else
	{
		uxLED -= partstNUM_LEDs;

		if( uxLED < partstNUM_LEDs )
		{		
			taskENTER_CRITICAL();
			{		
				/* Toggle the state of the single genuine on board LED. */
				if( sState1[uxLED])	
				{
					PDR00 |= ( 1 << uxLED );
				}
				else
				{
					PDR00 &= ~( 1 << uxLED );
				}
			
				sState1[ uxLED ] = !( sState1[ uxLED ] );
			}
			taskEXIT_CRITICAL();
		}
	}
}
/*-----------------------------------------------------------*/

void vParTestSetLED( unsigned portBASE_TYPE uxLED, signed portBASE_TYPE xValue )
{
	/* Set or clear the output [in this case show or hide the '*' character. */
	if( uxLED < partstNUM_LEDs )
	{
		taskENTER_CRITICAL();
		{
			if( xValue )
			{
				PDR09 |= ( 1 << uxLED );
				sState[ uxLED ] = 1;
			}
			else
			{
				PDR09 &= ~( 1 << uxLED );
				sState[ uxLED ] = 0;
			}
		}
		taskEXIT_CRITICAL();
	}
	else 
	{
		uxLED -= partstNUM_LEDs;

		taskENTER_CRITICAL();
		{
			if( xValue )
			{
				PDR00 |= ( 1 << uxLED );
				sState1[ uxLED ] = 1;
			}
			else
			{
				PDR00 &= ~( 1 << uxLED );
				sState1[ uxLED ] = 0;
			}
		}
		taskEXIT_CRITICAL();
	}
}

