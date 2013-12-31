/*
    FreeRTOS V8.0.0:rc1 - Copyright (C) 2014 Real Time Engineers Ltd. 
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

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"

/*
 * The register test task as described in the comments at the top of main-full.c.
 */
void vRegisterTest1( void *pvParameters );
void vRegisterTest2( void *pvParameters );

/* Variables that are incremented on each iteration of the reg test tasks -
provided the tasks have not reported any errors.  The check timer inspects these
variables to ensure they are still incrementing as expected.  If a variable
stops incrementing then it is likely that its associate task has stalled or
detected an error. */
volatile unsigned long ulRegTest1CycleCount = 0UL, ulRegTest2CycleCount = 0UL;

/*-----------------------------------------------------------*/

void vRegisterTest1( void *pvParameters )
{
	/* This task uses an infinite loop that is implemented in the assembly 
	code.
	
	First fill the relevant registers with known values. */
	asm volatile (	"	addi r3, r0, 3		\n\t" \
					"	addi r4, r0, 4		\n\t" \
					"	addi r6, r0, 6		\n\t" \
					"	addi r7, r0, 7		\n\t" \
					"	addi r8, r0, 8		\n\t" \
					"	addi r9, r0, 9		\n\t" \
					"	addi r10, r0, 10	\n\t" \
					"	addi r11, r0, 11	\n\t" \
					"	addi r12, r0, 12	\n\t" \
					"	addi r16, r0, 16	\n\t" \
					"	addi r19, r0, 19	\n\t" \
					"	addi r20, r0, 20	\n\t" \
					"	addi r21, r0, 21	\n\t" \
					"	addi r22, r0, 22	\n\t" \
					"	addi r23, r0, 23	\n\t" \
					"	addi r24, r0, 24	\n\t" \
					"	addi r25, r0, 25	\n\t" \
					"	addi r26, r0, 26	\n\t" \
					"	addi r27, r0, 27	\n\t" \
					"	addi r28, r0, 28	\n\t" \
					"	addi r29, r0, 29	\n\t" \
					"	addi r30, r0, 30	\n\t" \
					"	addi r31, r0, 31	\n\t"
				);

	/* Now test the register values to ensure they contain the same value that
	was written to them above.	 This task will get preempted frequently so 
	other tasks are likely to have executed since the register values were 
	written.  If any register contains an unexpected value then the task will
	branch to Error_Loop_1, which in turn prevents it from incrementing its
	loop counter, enabling the check timer to determine that all is not as it
	should be. */

	asm volatile (	"Loop_Start_1:				\n\t" \
					"	xori r18, r3, 3			\n\t" \
					"	bnei r18, Error_Loop_1	\n\t" \
					"	xori r18, r4, 4			\n\t" \
					"	bnei r18, Error_Loop_1	\n\t" \
					"	xori r18, r6, 6			\n\t" \
					"	bnei r18, Error_Loop_1	\n\t" \
					"	xori r18, r7, 7			\n\t" \
					"	bnei r18, Error_Loop_1	\n\t" \
					"	xori r18, r8, 8			\n\t" \
					"	bnei r18, Error_Loop_1	\n\t" \
					"	xori r18, r9, 9			\n\t" \
					"	bnei r18, Error_Loop_1	\n\t" \
					"	xori r18, r10, 10		\n\t" \
					"	bnei r18, Error_Loop_1	\n\t" \
					"	xori r18, r11, 11		\n\t" \
					"	bnei r18, Error_Loop_1	\n\t" \
					"	xori r18, r12, 12		\n\t" \
					"	bnei r18, Error_Loop_1	\n\t" \
					"	xori r18, r16, 16		\n\t" \
					"	bnei r18, Error_Loop_1	\n\t" \
					"	xori r18, r19, 19		\n\t" \
					"	bnei r18, Error_Loop_1	\n\t" \
					"	xori r18, r20, 20		\n\t" \
					"	bnei r18, Error_Loop_1	\n\t" \
					"	xori r18, r21, 21		\n\t" \
					"	bnei r18, Error_Loop_1	\n\t" \
					"	xori r18, r22, 22		\n\t" \
					"	bnei r18, Error_Loop_1	\n\t" \
					"	xori r18, r23, 23		\n\t" \
					"	bnei r18, Error_Loop_1	\n\t" \
					"	xori r18, r24, 24		\n\t" \
					"	bnei r18, Error_Loop_1	\n\t" \
					"	xori r18, r25, 25		\n\t" \
					"	bnei r18, Error_Loop_1	\n\t" \
					"	xori r18, r26, 26		\n\t" \
					"	bnei r18, Error_Loop_1	\n\t" \
					"	xori r18, r27, 27		\n\t" \
					"	bnei r18, Error_Loop_1	\n\t" \
					"	xori r18, r28, 28		\n\t" \
					"	bnei r18, Error_Loop_1	\n\t" \
					"	xori r18, r29, 29		\n\t" \
					"	bnei r18, Error_Loop_1	\n\t" \
					"	xori r18, r30, 30		\n\t" \
					"	bnei r18, Error_Loop_1	\n\t" \
					"	xori r18, r31, 31		\n\t" \
					"	bnei r18, Error_Loop_1	\n\t"
				 );

	/* If this task has not branched to the error loop, then everything is ok,
	and the check variable can be incremented to indicate that this task
	is still running.  Then, brach back to the top to check the register
	contents again. */
	asm volatile (  "	lwi r18, r0, ulRegTest1CycleCount	\n\t" \
					"	addik r18, r18, 1			 		\n\t" \
					"	swi r18, r0, ulRegTest1CycleCount 	\n\t" \
					"										\n\t" \
					"	bri Loop_Start_1 "
				 );

	 /* The test function will branch here if it discovers an error.  This part
	of the code just sits in a NULL loop, which prevents the check variable
	incrementing any further to allow the check timer to recognize that this
	test has failed. */
	asm volatile (	"Error_Loop_1:			\n\t" \
					"	bri 0				\n\t" \
					"	nop					\n\t" \
				 );

	( void ) pvParameters;
}
/*-----------------------------------------------------------*/

void vRegisterTest2( void *pvParameters )
{
	/* This task uses an infinite loop that is implemented in the assembly 
	code.
	
	First fill the registers with known values. */
	asm volatile (	"	addi r16, r0, 1016	\n\t" \
					"	addi r19, r0, 1019	\n\t" \
					"	addi r20, r0, 1020	\n\t" \
					"	addi r21, r0, 1021	\n\t" \
					"	addi r22, r0, 1022	\n\t" \
					"	addi r23, r0, 1023	\n\t" \
					"	addi r24, r0, 1024	\n\t" \
					"	addi r25, r0, 1025	\n\t" \
					"	addi r26, r0, 1026	\n\t" \
					"	addi r27, r0, 1027	\n\t" \
					"	addi r28, r0, 1028	\n\t" \
					"	addi r29, r0, 1029	\n\t" \
					"	addi r30, r0, 1030	\n\t" \
					"	addi r31, r0, 1031	\n\t" \
					"							" \
					"Loop_Start_2:				"
				);

	/* Unlike vRegisterTest1, vRegisterTest2 performs a yield.  This increases
	the test coverage, but does mean volatile registers need re-loading with 
	their exepcted values. */
	taskYIELD();

	/* taskYIELD() could have changed temporaries - set them back to those
	expected by the reg test task. */
	asm volatile (  "	addi r3, r0, 103	\n\t" \
					"	addi r4, r0, 104	\n\t" \
					"	addi r6, r0, 106	\n\t" \
					"	addi r7, r0, 107	\n\t" \
					"	addi r8, r0, 108	\n\t" \
					"	addi r9, r0, 109	\n\t" \
					"	addi r10, r0, 1010	\n\t" \
					"	addi r11, r0, 1011	\n\t" \
					"	addi r12, r0, 1012	\n\t" \
				);


	/* Now test the register values to ensure they contain the same value that
	was written to them above.	 This task will get preempted frequently so 
	other tasks are likely to have executed since the register values were 
	written. */
	asm volatile (	"	xori r18, r3, 103		\n\t" \
					"	bnei r18, Error_Loop_2	\n\t" \
					"	xori r18, r4, 104		\n\t" \
					"	bnei r18, Error_Loop_2	\n\t" \
					"	xori r18, r6, 106		\n\t" \
					"	bnei r18, Error_Loop_2	\n\t" \
					"	xori r18, r7, 107		\n\t" \
					"	bnei r18, Error_Loop_2	\n\t" \
					"	xori r18, r8, 108		\n\t" \
					"	bnei r18, Error_Loop_2	\n\t" \
					"	xori r18, r9, 109		\n\t" \
					"	bnei r18, Error_Loop_2	\n\t" \
					"	xori r18, r10, 1010		\n\t" \
					"	bnei r18, Error_Loop_2	\n\t" \
					"	xori r18, r11, 1011		\n\t" \
					"	bnei r18, Error_Loop_2	\n\t" \
					"	xori r18, r12, 1012		\n\t" \
					"	bnei r18, Error_Loop_2	\n\t" \
					"	xori r18, r16, 1016		\n\t" \
					"	bnei r18, Error_Loop_2	\n\t" \
					"	xori r18, r19, 1019		\n\t" \
					"	bnei r18, Error_Loop_2	\n\t" \
					"	xori r18, r20, 1020		\n\t" \
					"	bnei r18, Error_Loop_2	\n\t" \
					"	xori r18, r21, 1021		\n\t" \
					"	bnei r18, Error_Loop_2	\n\t" \
					"	xori r18, r22, 1022		\n\t" \
					"	bnei r18, Error_Loop_2	\n\t" \
					"	xori r18, r23, 1023		\n\t" \
					"	bnei r18, Error_Loop_2	\n\t" \
					"	xori r18, r24, 1024		\n\t" \
					"	bnei r18, Error_Loop_2	\n\t" \
					"	xori r18, r25, 1025		\n\t" \
					"	bnei r18, Error_Loop_2	\n\t" \
					"	xori r18, r26, 1026		\n\t" \
					"	bnei r18, Error_Loop_2	\n\t" \
					"	xori r18, r27, 1027		\n\t" \
					"	bnei r18, Error_Loop_2	\n\t" \
					"	xori r18, r28, 1028		\n\t" \
					"	bnei r18, Error_Loop_2	\n\t" \
					"	xori r18, r29, 1029		\n\t" \
					"	bnei r18, Error_Loop_2	\n\t" \
					"	xori r18, r30, 1030		\n\t" \
					"	bnei r18, Error_Loop_2	\n\t" \
					"	xori r18, r31, 1031		\n\t" \
					"	bnei r18, Error_Loop_2	\n\t"
				 );

	/* If this task has not branched to the error loop, then everything is ok,
	and the check variable should be incremented to indicate that this task
	is still running.  Then, brach back to the top to check the registers
	again. */
	asm volatile (  "	lwi r18, r0, ulRegTest2CycleCount	\n\t" \
					"	addik r18, r18, 1			 		\n\t" \
					"	swi r18, r0, ulRegTest2CycleCount 	\n\t" \
					"										\n\t" \
					"	bri Loop_Start_2 "
				 );

	 /* The test function will branch here if it discovers an error.  This part
	of the code just sits in a NULL loop, which prevents the check variable
	incrementing any further to allow the check timer to recognize that this
	test has failed. */
	asm volatile (	"Error_Loop_2:			\n\t" \
					"	bri 0				\n\t" \
					"	nop					\n\t" \
				 );

	( void ) pvParameters;
}






