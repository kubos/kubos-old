/*
    FreeRTOS V7.0.2 - Copyright (C) 2011 Real Time Engineers Ltd.
	

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


#ifndef PORTMACRO_H
#define PORTMACRO_H

#ifdef __cplusplus
extern "C" {
#endif

/*-----------------------------------------------------------
 * Port specific definitions.  
 *
 * The settings in this file configure FreeRTOS correctly for the
 * given hardware and compiler.
 *
 * These settings should not be altered.
 *-----------------------------------------------------------
 */

/* Type definitions - these are a bit legacy and not really used now, other than
portSTACK_TYPE and portBASE_TYPE. */
#define portCHAR		char
#define portFLOAT		float
#define portDOUBLE		double
#define portLONG		long
#define portSHORT		short
#define portSTACK_TYPE	unsigned portLONG
#define portBASE_TYPE	long

#if( configUSE_16_BIT_TICKS == 1 )
	typedef unsigned portSHORT portTickType;
	#define portMAX_DELAY ( portTickType ) 0xffff
#else
	typedef unsigned portLONG portTickType;
	#define portMAX_DELAY ( portTickType ) 0xffffffff
#endif
/*-----------------------------------------------------------*/

/* Hardware specifics. */
#define portBYTE_ALIGNMENT			8	/* Could make four, according to manual. */
#define portSTACK_GROWTH			-1
#define portTICK_RATE_MS			( ( portTickType ) 1000 / configTICK_RATE_HZ )		
#define portNOP()					__asm volatile( "NOP" )

/* The location of the software interrupt register.  Software interrupts use
vector 27. */
#define portITU_SWINTR			( ( unsigned char * ) 0x000872E0 )
#define portYIELD()				*portITU_SWINTR = 0x01; portNOP(); portNOP(); portNOP(); portNOP(); portNOP()
#define portYIELD_FROM_ISR( x )	if( x != pdFALSE ) portYIELD()

/*
 * These macros should be called directly, but through the taskENTER_CRITICAL()
 * and taskEXIT_CRITICAL() macros.
 */
#define portENABLE_INTERRUPTS() 	__asm volatile ( "MVTIPL	#0" );
#define portDISABLE_INTERRUPTS() 	__asm volatile ( "MVTIPL	%0" ::"i"(configMAX_SYSCALL_INTERRUPT_PRIORITY) )

/* Critical nesting counts are stored in the TCB. */
#define portCRITICAL_NESTING_IN_TCB ( 1 )

/* The critical nesting functions defined within tasks.c. */
extern void vTaskEnterCritical( void );
extern void vTaskExitCritical( void );
#define portENTER_CRITICAL()	vTaskEnterCritical();
#define portEXIT_CRITICAL()		vTaskExitCritical();

/* As this port allows interrupt nesting... */
unsigned long ulPortGetIPL( void ) __attribute__((naked));
void vPortSetIPL( unsigned long ulNewIPL ) __attribute__((naked));
#define portSET_INTERRUPT_MASK_FROM_ISR() ulPortGetIPL(); portDISABLE_INTERRUPTS()
#define portCLEAR_INTERRUPT_MASK_FROM_ISR( uxSavedInterruptStatus ) vPortSetIPL( uxSavedInterruptStatus )

/*-----------------------------------------------------------*/

/* Task function macros as described on the FreeRTOS.org WEB site. */
#define portTASK_FUNCTION_PROTO( vFunction, pvParameters ) void vFunction( void *pvParameters )
#define portTASK_FUNCTION( vFunction, pvParameters ) void vFunction( void *pvParameters )

#ifdef __cplusplus
}
#endif

#endif /* PORTMACRO_H */

