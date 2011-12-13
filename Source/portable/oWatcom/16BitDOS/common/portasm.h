/*
    FreeRTOS V7.1.0 - Copyright (C) 2011 Real Time Engineers Ltd.
	

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

typedef void tskTCB;
extern volatile tskTCB * volatile pxCurrentTCB;
extern void vTaskSwitchContext( void );

/* 
 * Saves the stack pointer for one task into its TCB, calls 
 * vTaskSwitchContext() to update the TCB being used, then restores the stack 
 * from the new TCB read to run the task. 
 */
void portSWITCH_CONTEXT( void );

/*
 * Load the stack pointer from the TCB of the task which is going to be first
 * to execute.  Then force an IRET so the registers and IP are popped off the
 * stack.
 */
void portFIRST_CONTEXT( void );

/* There are slightly different versions depending on whether you are building
to include debugger information.  If debugger information is used then there
are a couple of extra bytes left of the ISR stack (presumably for use by the
debugger).  The true stack pointer is then stored in the bp register.  We add
2 to the stack pointer to remove the extra bytes before we restore our context. */

#ifdef DEBUG_BUILD

	#pragma aux portSWITCH_CONTEXT =	"mov	ax, seg pxCurrentTCB"														\
										"mov	ds, ax"																		\
										"les	bx, pxCurrentTCB"			/* Save the stack pointer into the TCB. */		\
										"mov	es:0x2[ bx ], ss"															\
										"mov	es:[ bx ], sp"																\
										"call	vTaskSwitchContext"			/* Perform the switch. */						\
										"mov	ax, seg pxCurrentTCB"		/* Restore the stack pointer from the TCB. */	\
										"mov	ds, ax"																		\
										"les	bx, dword ptr pxCurrentTCB"													\
										"mov	ss, es:[ bx + 2 ]"															\
										"mov	sp, es:[ bx ]"																\
										"mov	bp, sp"						/* Prepair the bp register for the restoration of the SP in the compiler generated portion of the ISR */	\
										"add	bp, 0x0002"

										

	#pragma aux portFIRST_CONTEXT =		"mov	ax, seg pxCurrentTCB"			\
										"mov	ds, ax"							\
										"les	bx, dword ptr pxCurrentTCB"		\
										"mov	ss, es:[ bx + 2 ]"				\
										"mov	sp, es:[ bx ]"					\
										"add	sp, 0x0002"						/* Remove the extra bytes that exist in debug builds before restoring the context. */ \
										"pop	ax"								\
										"pop	ax"								\
										"pop	es"								\
										"pop	ds"								\
										"popa"									\
										"iret"									
#else

	#pragma aux portSWITCH_CONTEXT =	"mov	ax, seg pxCurrentTCB"														\
										"mov	ds, ax"																		\
										"les	bx, pxCurrentTCB"			/* Save the stack pointer into the TCB. */		\
										"mov	es:0x2[ bx ], ss"															\
										"mov	es:[ bx ], sp"																\
										"call	vTaskSwitchContext"			/* Perform the switch. */						\
										"mov	ax, seg pxCurrentTCB"		/* Restore the stack pointer from the TCB. */	\
										"mov	ds, ax"																		\
										"les	bx, dword ptr pxCurrentTCB"													\
										"mov	ss, es:[ bx + 2 ]"															\
										"mov	sp, es:[ bx ]"
										

	#pragma aux portFIRST_CONTEXT =		"mov	ax, seg pxCurrentTCB"			\
										"mov	ds, ax"							\
										"les	bx, dword ptr pxCurrentTCB"		\
										"mov	ss, es:[ bx + 2 ]"				\
										"mov	sp, es:[ bx ]"					\
										"pop	ax"								\
										"pop	ax"								\
										"pop	es"								\
										"pop	ds"								\
										"popa"									\
										"iret"									
#endif


