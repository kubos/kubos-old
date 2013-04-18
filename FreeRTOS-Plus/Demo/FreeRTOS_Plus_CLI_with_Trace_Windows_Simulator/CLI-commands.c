/*
    FreeRTOS V7.1.1 - Copyright (C) 2012 Real Time Engineers Ltd.


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
    
    ***************************************************************************
     *                                                                       *
     *    Having a problem?  Start by reading the FAQ "My application does   *
     *    not run, what could be wrong?                                      *
     *                                                                       *
     *    http://www.FreeRTOS.org/FAQHelp.html                               *
     *                                                                       *
    ***************************************************************************

    
    http://www.FreeRTOS.org - Documentation, training, latest information, 
    license and contact details.
    
    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool.

    Real Time Engineers ltd license FreeRTOS to High Integrity Systems, who sell 
    the code with commercial support, indemnification, and middleware, under 
    the OpenRTOS brand: http://www.OpenRTOS.com.  High Integrity Systems also
    provide a safety engineered and independently SIL3 certified version under 
    the SafeRTOS brand: http://www.SafeRTOS.com.
*/

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* FreeRTOS+CLI includes. */
#include "FreeRTOS_CLI.h"

/* FreeRTOS+Trace includes. */
#include "trcUser.h"

/*
 * Defines a command that returns a table showing the state of each task at the
 * time the command is called.
 */
static portBASE_TYPE prvTaskStatsCommand( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString );

/*
 * Defines a command that returns a table showing how much time each task has
 * spent in the Running state.
 */
static portBASE_TYPE prvRunTimeStatsCommand( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString );

/*
 * Defines a command that expects exactly three parameters.  Each of the three
 * parameter are echoed back one at a time.
 */
static portBASE_TYPE prvThreeParameterEchoCommand( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString );

/*
 * Defines a command that can take a variable number of parameters.  Each 
 * parameter is echoes back one at a time.
 */
static portBASE_TYPE prvParameterEchoCommand( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString );

/*
 * Defines a command that starts/stops events being recorded for offline viewing 
 * in FreeRTOS+Trace.
 */
static portBASE_TYPE prvStartStopTraceCommand( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString );

/* Structure that defines the "run-time-stats" command line command. */
static const CLI_Command_Definition_t xRunTimeStats =
{
	( const int8_t * const ) "run-time-stats", /* The command string to type. */
	( const int8_t * const ) "\r\nrun-time-stats:\r\n Displays a table showing how much processing time each FreeRTOS task has used\r\n\r\n",
	prvRunTimeStatsCommand, /* The function to run. */
	0 /* No parameters are expected. */
};

/* Structure that defines the "task-stats" command line command. */
static const CLI_Command_Definition_t xTaskStats =
{
	( const int8_t * const ) "task-stats", /* The command string to type. */
	( const int8_t * const ) "\r\ntask-stats:\r\n Displays a table showing the state of each FreeRTOS task\r\n\r\n",
	prvTaskStatsCommand, /* The function to run. */
	0 /* No parameters are expected. */
};

/* Structure that defines the "echo_3_parameters" command line command.  This 
takes exactly three parameters that the command simply echos back one at a 
time. */
static const CLI_Command_Definition_t xThreeParameterEcho =
{
	( const int8_t * const ) "echo_3_parameters",
	( const int8_t * const ) "\r\necho_3_parameters <param1> <param2> <param3>:\r\n Expects three parameters, echos each in turn\r\n\r\n",
	prvThreeParameterEchoCommand, /* The function to run. */
	3 /* Three parameters are expected, which can take any value. */
};

/* Structure that defines the "echo_parameters" command line command.  This 
takes a variable number of parameters that the command simply echos back one at 
a time. */
static const CLI_Command_Definition_t xParameterEcho =
{
	( const int8_t * const ) "echo_parameters",
	( const int8_t * const ) "\r\necho_parameters <...>:\r\n Take variable number of parameters, echos each in turn\r\n\r\n",
	prvParameterEchoCommand, /* The function to run. */
	-1 /* The user can enter any number of commands. */
};

/* Structure that defines the "trace" command line command.  This takes a single
parameter, which can be either "start" or "stop". */
static const CLI_Command_Definition_t xStartTrace =
{
	( const int8_t * const ) "trace",
	( const int8_t * const ) "\r\ntrace [start | stop]:\r\n Starts or stops a trace recording for viewing in FreeRTOS+Trace\r\n\r\n",
	prvStartStopTraceCommand, /* The function to run. */
	1 /* One parameter is expected.  Valid values are "start" and "stop". */
};

/*-----------------------------------------------------------*/

void vRegisterCLICommands( void )
{
	/* Register all the command line commands defined immediately above. */
	FreeRTOS_CLIRegisterCommand( &xTaskStats );
	FreeRTOS_CLIRegisterCommand( &xRunTimeStats );
	FreeRTOS_CLIRegisterCommand( &xThreeParameterEcho );
	FreeRTOS_CLIRegisterCommand( &xParameterEcho );
	FreeRTOS_CLIRegisterCommand( &xStartTrace );
}
/*-----------------------------------------------------------*/

static portBASE_TYPE prvTaskStatsCommand( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString )
{
const int8_t *const pcHeader = ( int8_t * ) "Task          State  Priority  Stack	#\r\n************************************************\r\n";

	/* Remove compile time warnings about unused parameters, and check the
	write buffer is not NULL.  NOTE - for simplicity, this example assumes the 
	write buffer length is adequate, so does not check for buffer overflows. */
	( void ) pcCommandString;
	( void ) xWriteBufferLen;
	configASSERT( pcWriteBuffer );

	/* Generate a table of task stats. */
	strcpy( ( char * ) pcWriteBuffer, ( char * ) pcHeader );
	vTaskList( pcWriteBuffer + strlen( ( char * ) pcHeader ) );

	/* There is no more data to return after this single string, so return
	pdFALSE. */
	return pdFALSE;
}
/*-----------------------------------------------------------*/

static portBASE_TYPE prvRunTimeStatsCommand( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString )
{
const int8_t * const pcHeader = ( int8_t * ) "Task            Abs Time      % Time\r\n****************************************\r\n";

	/* Remove compile time warnings about unused parameters, and check the
	write buffer is not NULL.  NOTE - for simplicity, this example assumes the 
	write buffer length is adequate, so does not check for buffer overflows. */
	( void ) pcCommandString;
	( void ) xWriteBufferLen;
	configASSERT( pcWriteBuffer );

	/* Generate a table of task stats. */
	strcpy( ( char * ) pcWriteBuffer, ( char * ) pcHeader );
	vTaskGetRunTimeStats( pcWriteBuffer + strlen( ( char * ) pcHeader ) );

	/* There is no more data to return after this single string, so return
	pdFALSE. */
	return pdFALSE;
}
/*-----------------------------------------------------------*/

static portBASE_TYPE prvThreeParameterEchoCommand( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString )
{
int8_t *pcParameter;
portBASE_TYPE lParameterStringLength, xReturn;
static portBASE_TYPE lParameterNumber = 0;

	/* Remove compile time warnings about unused parameters, and check the
	write buffer is not NULL.  NOTE - for simplicity, this example assumes the 
	write buffer length is adequate, so does not check for buffer overflows. */
	( void ) pcCommandString;
	( void ) xWriteBufferLen;
	configASSERT( pcWriteBuffer );

	if( lParameterNumber == 0 )
	{
		/* The first time the function is called after the command has been
		entered just a header string is returned. */
		sprintf( ( char * ) pcWriteBuffer, "The three parameters were:\r\n" );

		/* Next time the function is called the first parameter will be echoed
		back. */
		lParameterNumber = 1L;

		/* There is more data to be returned as no parameters have been echoed
		back yet. */
		xReturn = pdPASS;
	}
	else
	{
		/* Obtain the parameter string. */
		pcParameter = ( int8_t * ) FreeRTOS_CLIGetParameter
									( 
										pcCommandString,		/* The command string itself. */
										lParameterNumber,		/* Return the next parameter. */
										&lParameterStringLength	/* Store the parameter string length. */
									);

		/* Sanity check something was returned. */
		configASSERT( pcParameter );

		/* Return the parameter string. */
		memset( pcWriteBuffer, 0x00, xWriteBufferLen );
		sprintf( ( char * ) pcWriteBuffer, "%d: ", lParameterNumber );
		strncat( ( char * ) pcWriteBuffer, ( const char * ) pcParameter, lParameterStringLength );
		strncat( ( char * ) pcWriteBuffer, "\r\n", strlen( "\r\n" ) );

		/* If this is the last of the three parameters then there are no more
		strings to return after this one. */
		if( lParameterNumber == 3L )
		{
			/* If this is the last of the three parameters then there are no more
			strings to return after this one. */
			xReturn = pdFALSE;
			lParameterNumber = 0L;
		}
		else
		{
			/* There are more parameters to return after this one. */
			xReturn = pdTRUE;
			lParameterNumber++;
		}
	}

	return xReturn;
}
/*-----------------------------------------------------------*/

static portBASE_TYPE prvParameterEchoCommand( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString )
{
int8_t *pcParameter;
portBASE_TYPE lParameterStringLength, xReturn;
static portBASE_TYPE lParameterNumber = 0;

	/* Remove compile time warnings about unused parameters, and check the
	write buffer is not NULL.  NOTE - for simplicity, this example assumes the 
	write buffer length is adequate, so does not check for buffer overflows. */
	( void ) pcCommandString;
	( void ) xWriteBufferLen;
	configASSERT( pcWriteBuffer );

	if( lParameterNumber == 0 )
	{
		/* The first time the function is called after the command has been
		entered just a header string is returned. */
		sprintf( ( char * ) pcWriteBuffer, "The parameters were:\r\n" );

		/* Next time the function is called the first parameter will be echoed
		back. */
		lParameterNumber = 1L;

		/* There is more data to be returned as no parameters have been echoed
		back yet. */
		xReturn = pdPASS;
	}
	else
	{
		/* Obtain the parameter string. */
		pcParameter = ( int8_t * ) FreeRTOS_CLIGetParameter
									( 
										pcCommandString,		/* The command string itself. */
										lParameterNumber,		/* Return the next parameter. */
										&lParameterStringLength	/* Store the parameter string length. */
									);

		if( pcParameter != NULL )
		{
			/* Return the parameter string. */
			memset( pcWriteBuffer, 0x00, xWriteBufferLen );
			sprintf( ( char * ) pcWriteBuffer, "%d: ", lParameterNumber );
			strncat( ( char * ) pcWriteBuffer, ( const char * ) pcParameter, lParameterStringLength );
			strncat( ( char * ) pcWriteBuffer, "\r\n", strlen( "\r\n" ) );

			/* There might be more parameters to return after this one. */
			xReturn = pdTRUE;
			lParameterNumber++;
		}
		else
		{
			/* No more parameters were found.  Make sure the write buffer does
			not contain a valid string. */
			pcWriteBuffer[ 0 ] = 0x00;

			/* No more data to return. */
			xReturn = pdFALSE;

			/* Start over the next time this command is executed. */
			lParameterNumber = 0;
		}
	}

	return xReturn;
}
/*-----------------------------------------------------------*/

static portBASE_TYPE prvStartStopTraceCommand( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString )
{
int8_t *pcParameter;
portBASE_TYPE lParameterStringLength;

	/* Remove compile time warnings about unused parameters, and check the
	write buffer is not NULL.  NOTE - for simplicity, this example assumes the 
	write buffer length is adequate, so does not check for buffer overflows. */
	( void ) pcCommandString;
	( void ) xWriteBufferLen;
	configASSERT( pcWriteBuffer );

	/* Obtain the parameter string. */
	pcParameter = ( int8_t * ) FreeRTOS_CLIGetParameter
								( 
									pcCommandString,		/* The command string itself. */
									1,						/* Return the first parameter. */
									&lParameterStringLength	/* Store the parameter string length. */
								);

	/* Sanity check something was returned. */
	configASSERT( pcParameter );

	/* There are only two valid parameter values. */
	if( strncmp( ( const char * ) pcParameter, "start", strlen( "start" ) ) == 0 )
	{
		/* Start or restart the trace. */
		vTraceStop();
		vTraceClear();
		vTraceStart();

		sprintf( ( char * ) pcWriteBuffer, "Trace recording (re)started.\r\n" );
	}
	else if( strncmp( ( const char * ) pcParameter, "stop", strlen( "stop" ) ) == 0 )
	{
		/* End the trace, if one is running. */
		vTraceStop();
		sprintf( ( char * ) pcWriteBuffer, "Stopping trace recording.\r\n" );
	}
	else
	{
		sprintf( ( char * ) pcWriteBuffer, "Valid parameters are 'start' and 'stop'.\r\n" );
	}

	/* There is no more data to return after this single string, so return
	pdFALSE. */
	return pdFALSE;
}
/*-----------------------------------------------------------*/


