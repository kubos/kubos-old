/*
	FreeRTOS V5.4.2 - Copyright (C) 2009 Real Time Engineers Ltd.

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

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include <stdio.h>
#include "AT91SAM7X256.h"

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE. 
 *
 * See http://www.freertos.org/a00110.html.
 *----------------------------------------------------------*/

#define configUSE_PREEMPTION		1
#define configUSE_IDLE_HOOK         0
#define configUSE_TICK_HOOK         1
#define configCPU_CLOCK_HZ          ( ( unsigned portLONG ) 47923200 )
#define configTICK_RATE_HZ          ( ( portTickType ) 1000 )
#define configMAX_PRIORITIES		( ( unsigned portBASE_TYPE ) 4 )
#define configMINIMAL_STACK_SIZE	( ( unsigned portSHORT ) 130 )
#define configTOTAL_HEAP_SIZE		( ( size_t ) ( 24 * 1024 ) )
#define configMAX_TASK_NAME_LEN		( 10 )
#define configUSE_TRACE_FACILITY	1
#define configUSE_16_BIT_TICKS		0
#define configIDLE_SHOULD_YIELD		1
#define configUSE_MUTEXES			1

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES 		0
#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )


/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */

#define INCLUDE_vTaskPrioritySet            1
#define INCLUDE_uxTaskPriorityGet           1
#define INCLUDE_vTaskDelete                 1
#define INCLUDE_vTaskCleanUpResources       0
#define INCLUDE_vTaskSuspend                1
#define INCLUDE_vTaskDelayUntil             1
#define INCLUDE_vTaskDelay                  1
#define INCLUDE_xTaskGetCurrentTaskHandle	1



/* MAC address configuration. */
#define uipMAC_ADDR0	0x00
#define uipMAC_ADDR1	0x12
#define uipMAC_ADDR2	0x13
#define uipMAC_ADDR3	0x10
#define uipMAC_ADDR4	0x15
#define uipMAC_ADDR5	0x11

/* IP address configuration. */
#define uipIP_ADDR0		172
#define uipIP_ADDR1		25
#define uipIP_ADDR2		218
#define uipIP_ADDR3		10	

#endif /* FREERTOS_CONFIG_H */
