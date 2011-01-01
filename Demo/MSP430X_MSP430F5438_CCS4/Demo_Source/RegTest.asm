;
;    FreeRTOS V6.1.0 - Copyright (C) 2010 Real Time Engineers Ltd.
;
;    ***************************************************************************
;    *                                                                         *
;    * If you are:                                                             *
;    *                                                                         *
;    *    + New to FreeRTOS,                                                   *
;    *    + Wanting to learn FreeRTOS or multitasking in general quickly       *
;    *    + Looking for basic training,                                        *
;    *    + Wanting to improve your FreeRTOS skills and productivity           *
;    *                                                                         *
;    * then take a look at the FreeRTOS books - available as PDF or paperback  *
;    *                                                                         *
;    *        "Using the FreeRTOS Real Time Kernel - a Practical Guide"        *
;    *                  http://www.FreeRTOS.org/Documentation                  *
;    *                                                                         *
;    * A pdf reference manual is also available.  Both are usually delivered   *
;    * to your inbox within 20 minutes to two hours when purchased between 8am *
;    * and 8pm GMT (although please allow up to 24 hours in case of            *
;    * exceptional circumstances).  Thank you for your support!                *
;    *                                                                         *
;    ***************************************************************************
;
;    This file is part of the FreeRTOS distribution.
;
;    FreeRTOS is free software; you can redistribute it and/or modify it under
;    the terms of the GNU General Public License (version 2) as published by the
;    Free Software Foundation AND MODIFIED BY the FreeRTOS exception.
;    ***NOTE*** The exception to the GPL is included to allow you to distribute
;    a combined work that includes FreeRTOS without being obliged to provide the
;    source code for proprietary components outside of the FreeRTOS kernel.
;    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT
;    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
;    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
;    more details. You should have received a copy of the GNU General Public
;    License and the FreeRTOS license exception along with FreeRTOS; if not it
;    can be viewed here: http://www.freertos.org/a00114.html and also obtained
;    by writing to Richard Barry, contact details for whom are available on the
;    FreeRTOS WEB site.
;
;    1 tab == 4 spaces!
;
;    http://www.FreeRTOS.org - Documentation, latest information, license and
;    contact details.
;
;    http://www.SafeRTOS.com - A version that is certified for use in safety
;    critical systems.
;
;    http://www.OpenRTOS.com - Commercial support, development, porting,
;    licensing and training services.

; * The definition of the "register test" tasks, as described at the top of
; * main.c




	.global usRegTest1Counter
	.global usRegTest2Counter
	.global vPortYield
	
	.def vRegTest1Task
	.def vRegTest2Task

	.text

vRegTest1Task:

	; Fill each general purpose register with a known value.
	movx.a	#0x44444, r4
	movx.a	#0x55555, r5
	movx.a	#0x66666, r6
	movx.a	#0x77777, r7
	movx.a	#0x88888, r8
	movx.a	#0x99999, r9
	movx.a	#0xaaaaa, r10
	movx.a	#0xbbbbb, r11
	movx.a	#0xccccc, r12
	movx.a	#0xddddd, r13
	movx.a	#0xeeeee, r14
	movx.a	#0xfffff, r15
	
prvRegTest1Loop:

	; Test each general purpose register to check that it still contains the
	; expected known value, jumping to vRegTest1Error if any register contains
	; an unexpected value.
	cmpx.a	#0x44444, r4
	jne		vRegTest1Error
	cmpx.a	#0x55555, r5
	jne		vRegTest1Error
	cmpx.a	#0x66666, r6
	jne		vRegTest1Error
	cmpx.a	#0x77777, r7
	jne		vRegTest1Error
	cmpx.a	#0x88888, r8
	jne		vRegTest1Error
	cmpx.a	#0x99999, r9
	jne		vRegTest1Error
	cmpx.a	#0xaaaaa, r10
	jne		vRegTest1Error
	cmpx.a	#0xbbbbb, r11
	jne		vRegTest1Error
	cmpx.a	#0xccccc, r12
	jne		vRegTest1Error
	cmpx.a	#0xddddd, r13
	jne		vRegTest1Error
	cmpx.a	#0xeeeee, r14
	jne		vRegTest1Error
	cmpx.a	#0xfffff, r15
	jne		vRegTest1Error
	
	; This task is still running without jumping to vRegTest1Error, so increment
	; the loop counter so the check task knows the task is running error free.
	incx.w	&usRegTest1Counter
	
	; Loop again, performing the same tests.
	jmp		prvRegTest1Loop
	nop

	
vRegTest1Error:
	jmp vRegTest1Error
	nop
	
; -----------------------------------------------------------

; See the comments in vRegTest1Task.  This task is the same, it just uses
; different values in its registers.
vRegTest2Task:

	movx.a	#0x14441, r4
	movx.a	#0x15551, r5
	movx.a	#0x16661, r6
	movx.a	#0x17771, r7
	movx.a	#0x18881, r8
	movx.a	#0x19991, r9
	movx.a	#0x1aaa1, r10
	movx.a	#0x1bbb1, r11
	movx.a	#0x1ccc1, r12
	movx.a	#0x1ddd1, r13
	movx.a	#0x1eee1, r14
	movx.a	#0x1fff1, r15
	
prvRegTest2Loop:

	cmpx.a	#0x14441, r4
	jne		vRegTest2Error
	cmpx.a	#0x15551, r5
	jne		vRegTest2Error
	cmpx.a	#0x16661, r6
	jne		vRegTest2Error
	cmpx.a	#0x17771, r7
	jne		vRegTest2Error
	cmpx.a	#0x18881, r8
	jne		vRegTest2Error
	cmpx.a	#0x19991, r9
	jne		vRegTest2Error
	cmpx.a	#0x1aaa1, r10
	jne		vRegTest2Error
	cmpx.a	#0x1bbb1, r11
	jne		vRegTest2Error
	cmpx.a	#0x1ccc1, r12
	jne		vRegTest2Error
	cmpx.a	#0x1ddd1, r13
	jne		vRegTest2Error
	cmpx.a	#0x1eee1, r14
	jne		vRegTest2Error
	cmpx.a	#0x1fff1, r15
	jne		vRegTest2Error
	
	; Also perform a manual yield, just to increase the scope of the test.
	calla 	#vPortYield
	
	incx.w	&usRegTest2Counter
	jmp		prvRegTest2Loop
	nop

	
vRegTest2Error:
	jmp vRegTest2Error
	nop
; /*-----------------------------------------------------------

     		
	.end
		
