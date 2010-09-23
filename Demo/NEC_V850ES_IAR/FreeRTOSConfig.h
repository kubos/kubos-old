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

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/* only include in C files */
#ifdef __IAR_SYSTEMS_ICC__
	#pragma language=extended
	#pragma system_include
	#include <intrinsics.h>
#endif  /* __IAR_SYSTEMS_ICC__ */

/* V850ES/Fx3 Memory Model
 * 1 = Tiny data model
 * 0 = Small/Large data model
 */
#define configDATA_MODE            0



/*
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 */
#define configUSE_PREEMPTION		1
/* only include in C files */

#ifdef __IAR_SYSTEMS_ICC__

	#define configUSE_IDLE_HOOK				0
	#define configUSE_TICK_HOOK				0
	#define configTICK_RATE_HZ				( ( portTickType ) 1000 )
	#define configMAX_PRIORITIES			( ( unsigned portBASE_TYPE ) 4 )
	#define configMINIMAL_STACK_SIZE		( ( unsigned short ) 85 )
	#define configMAX_TASK_NAME_LEN			( 10 )
	#define configUSE_TRACE_FACILITY		0
	#define configUSE_16_BIT_TICKS			0
	#define configIDLE_SHOULD_YIELD			0
	#define configUSE_CO_ROUTINES 			0
	#define configUSE_MUTEXES				1
	#define configCHECK_FOR_STACK_OVERFLOW	2
	#define configUSE_RECURSIVE_MUTEXES		1
	#define configQUEUE_REGISTRY_SIZE		0
	#define configUSE_COUNTING_SEMAPHORES	0

	/* Co-routine definitions. */
	#define configUSE_CO_ROUTINES 			0
	#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )

	/* Set the following definitions to 1 to include the API function, or zero
	to exclude the API function. */
	#define INCLUDE_vTaskPrioritySet		1
	#define INCLUDE_uxTaskPriorityGet		1
	#define INCLUDE_vTaskDelete				1
	#define INCLUDE_vTaskCleanUpResources	0
	#define INCLUDE_vTaskSuspend			1
	#define INCLUDE_vTaskDelayUntil			1
	#define INCLUDE_vTaskDelay				1

	/* This IAR workspace contains several different projects - each of which
	is targeted at a different device variant.  The definitions past this point
	are dependent on the variant being used. */
	#ifdef __IAR_V850ES_Fx3__
		#include "io70f3385.h"
		#define configTOTAL_HEAP_SIZE			( (size_t ) ( 20000 ) )
		#define configCPU_CLOCK_HZ				( ( unsigned long ) 48000000 )
	#endif

	#ifdef __IAR_V850ES_Jx3__
		#include "io70f3746.h"
		#define configTOTAL_HEAP_SIZE			( (size_t ) ( 9000 ) )
		#define configCPU_CLOCK_HZ				( ( unsigned long ) 16000000 )
	#endif

	#ifdef __IAR_V850ES_Jx3_L__
		#include "io70f3738.h"
		#define configTOTAL_HEAP_SIZE			( (size_t ) ( 9000 ) )
		#define configCPU_CLOCK_HZ				( ( unsigned long ) 20000000 )
	#endif

	#ifdef __IAR_V850ES_Jx2__
		#include "io70f3717.h"
		#define configTOTAL_HEAP_SIZE			( (size_t ) ( 9000 ) )
		#define configCPU_CLOCK_HZ				( ( unsigned long ) 20000000 )
	#endif

	#ifdef __IAR_V850ES_Hx2__
		#include "io70f3707.h"
		#define configTOTAL_HEAP_SIZE			( (size_t ) ( 9000 ) )
		#define configCPU_CLOCK_HZ				( ( unsigned long ) 20000000 )
	#endif

#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* FREERTOS_CONFIG_H */
