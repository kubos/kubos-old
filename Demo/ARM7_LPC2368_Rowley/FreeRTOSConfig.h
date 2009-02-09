/*
	FreeRTOS.org V5.1.2 - Copyright (C) 2003-2009 Richard Barry.

	This file is part of the FreeRTOS distribution.

	FreeRTOS is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	FreeRTOS is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with FreeRTOS; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

	A special exception to the GPL can be applied should you wish to distribute
	a combined work that includes FreeRTOS, without being obliged to provide
	the source code for any proprietary components.  See the licensing section 
	of http://www.FreeRTOS.org for full details of how and when the exception
	can be applied.

	***************************************************************************
	See http://www.FreeRTOS.org for documentation, latest information, license 
	and contact details.  Please ensure to read the configuration and relevant 
	port sections of the online documentation.
	***************************************************************************
*/

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include <stdio.h>
#include <targets/LPC2368.h>
#define vPortYieldProcessor swi_handler

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


/* Value to use on old rev '-' devices. */
//#define configPINSEL2_VALUE	0x50151105

/* Value to use on rev 'A' and newer devices. */
//#define configPINSEL2_VALUE 	0x50150105

#ifndef configPINSEL2_VALUE
	#error Please uncomment one of the two configPINSEL2_VALUE definitions above, depending on the revision of the LPC2000 device being used.
#endif

#define configUSE_PREEMPTION		1
#define configUSE_IDLE_HOOK         0
#define configUSE_TICK_HOOK         0
#define configCPU_CLOCK_HZ          ( ( unsigned portLONG ) 57600000 )	
#define configTICK_RATE_HZ          ( ( portTickType ) 1000 )
#define configMAX_PRIORITIES		( ( unsigned portBASE_TYPE ) 4 )
#define configMINIMAL_STACK_SIZE	( ( unsigned portSHORT ) 120 )
#define configTOTAL_HEAP_SIZE		( ( size_t ) ( 18 * 1024 ) )
#define configMAX_TASK_NAME_LEN		( 16 )
#define configUSE_TRACE_FACILITY	1
#define configUSE_16_BIT_TICKS		0
#define configIDLE_SHOULD_YIELD		1

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


#endif /* FREERTOS_CONFIG_H */


#ifndef sbi
#define sbi(x,y)	x|=(1 << (y))
#endif

#ifndef cbi 
#define cbi(x,y)	x&=~(1 << (y))
#endif

#ifndef tstb
#define tstb(x,y)	(x & (1 << (y)) ? 1 : 0)
#endif

#ifndef toggle 
#define toggle(x,y)	x^=(1 << (y))
#endif

#ifndef BIT
#define BIT(x)	(1 << (x))


#define VICVectAddr    VICAddress
#define VICVectCntl4 VICVectPriority4
typedef struct
{
	long xColumn;
	signed char *pcMessage;
} xLCDMessage;

#endif
