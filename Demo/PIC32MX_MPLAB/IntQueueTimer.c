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

#include "FreeRTOS.h"
#include "IntQueueTimer.h"
#include "IntQueue.h"

#define timerINTERRUPT3_FREQUENCY	( 2000UL )
#define timerINTERRUPT4_FREQUENCY	( 2001UL )

void vT3InterruptHandler( void );
void vT4InterruptHandler( void );

void __attribute__( (interrupt(ipl0), vector(_TIMER_3_VECTOR))) vT3InterruptWrapper( void );
void __attribute__( (interrupt(ipl0), vector(_TIMER_4_VECTOR))) vT4InterruptWrapper( void );

void vInitialiseTimerForIntQueueTest( void )
{
	/* Timer 1 is used for the tick interrupt, timer 2 is used for the high
	frequency interrupt test.  This file therefore uses timers 3 and 4. */

	T3CON = 0;
	TMR3 = 0;
	PR3 = ( unsigned portSHORT ) ( configPERIPHERAL_CLOCK_HZ / timerINTERRUPT3_FREQUENCY );

	/* Setup timer 3 interrupt priority to be above the kernel priority. */
	ConfigIntTimer3( T3_INT_ON | ( configMAX_SYSCALL_INTERRUPT_PRIORITY - 1 ) );

	/* Clear the interrupt as a starting condition. */
	IFS0bits.T3IF = 0;

	/* Enable the interrupt. */
	IEC0bits.T3IE = 1;

	/* Start the timer. */
	T3CONbits.TON = 1;


	/* Do the same for timer 4. */
	T4CON = 0;
	TMR4 = 0;
	PR4 = ( unsigned portSHORT ) ( configPERIPHERAL_CLOCK_HZ / timerINTERRUPT4_FREQUENCY );

	/* Setup timer 4 interrupt priority to be above the kernel priority. */
	ConfigIntTimer4( T4_INT_ON | ( configMAX_SYSCALL_INTERRUPT_PRIORITY ) );

	/* Clear the interrupt as a starting condition. */
	IFS0bits.T4IF = 0;

	/* Enable the interrupt. */
	IEC0bits.T4IE = 1;

	/* Start the timer. */
	T4CONbits.TON = 1;
}
/*-----------------------------------------------------------*/

void vT3InterruptHandler( void )
{
	IFS0bits.T3IF = 0;
	portEND_SWITCHING_ISR( xFirstTimerHandler() );
}
/*-----------------------------------------------------------*/

void vT4InterruptHandler( void )
{
	IFS0bits.T4IF = 0;
	portEND_SWITCHING_ISR( xSecondTimerHandler() );
}


