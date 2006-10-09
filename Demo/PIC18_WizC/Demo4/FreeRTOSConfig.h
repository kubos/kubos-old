/*
	FreeRTOS.org V4.1.2 - Copyright (C) 2003-2006 Richard Barry.

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
	***************************************************************************
*/

/*
Changes from V3.0.0
	+ TickRate reduced to 250Hz.

	+ configIDLE_SHOULD_YIELD added.

Changes from V3.0.1
*/

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE. 
 *----------------------------------------------------------*/

#define configUSE_PREEMPTION			( 1 )
#define configUSE_IDLE_HOOK				( 0 )
#define configUSE_TICK_HOOK				( 0 )
#define configTICK_RATE_HZ				( 250 )
#define configMAX_PRIORITIES			( 4 )
#define configMINIMAL_STACK_SIZE		portMINIMAL_STACK_SIZE
#define configMAX_TASK_NAME_LEN			( 3 )
#define configUSE_TRACE_FACILITY		( 0 )
#define configUSE_16_BIT_TICKS			( 1 )
#define configIDLE_SHOULD_YIELD			( 1 )

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES 		0
#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )

/* Set the following definitions to 1 to include the component, or zero
to exclude the component. */

/* Include/exclude the stated API function. */
#define INCLUDE_vTaskPrioritySet		( 1 )
#define INCLUDE_uxTaskPriorityGet		( 1 )
#define INCLUDE_vTaskDelete				( 0 )
#define INCLUDE_vTaskCleanUpResources	( 0 )
#define INCLUDE_vTaskSuspend			( 1 )
#define INCLUDE_vTaskDelayUntil			( 1 )
#define INCLUDE_vTaskDelay				( 1 )

#endif /* FREERTOS_CONFIG_H */
