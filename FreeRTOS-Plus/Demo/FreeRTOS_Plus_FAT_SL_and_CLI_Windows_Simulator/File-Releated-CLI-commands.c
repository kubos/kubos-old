/*
    FreeRTOS V7.5.3 - Copyright (C) 2013 Real Time Engineers Ltd. 
    All rights reserved

    FEATURES AND PORTS ARE ADDED TO FREERTOS ALL THE TIME.  PLEASE VISIT
    http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

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

    >>>>>>NOTE<<<<<< The modification to the GPL is included to allow you to
    distribute a combined work that includes FreeRTOS without being obliged to
    provide the source code for proprietary components outside of the FreeRTOS
    kernel.

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details. You should have received a copy of the GNU General Public License
    and the FreeRTOS license exception along with FreeRTOS; if not itcan be
    viewed here: http://www.freertos.org/a00114.html and also obtained by
    writing to Real Time Engineers Ltd., contact details for whom are available
    on the FreeRTOS WEB site.

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
    including FreeRTOS+Trace - an indispensable productivity tool, and our new
    fully thread aware and reentrant UDP/IP stack.

    http://www.OpenRTOS.com - Real Time Engineers ltd license FreeRTOS to High
    Integrity Systems, who sell the code with commercial support,
    indemnification and middleware, under the OpenRTOS brand.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.
*/

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* FreeRTOS+CLI includes. */
#include "FreeRTOS_CLI.h"

/* File system includes. */
#include "fat_sl.h"
#include "api_mdriver_ram.h"
#include "test.h"

#ifdef _WINDOWS_
	#define snprintf _snprintf
#endif

#define cliNEW_LINE		"\r\n"

/*******************************************************************************
 * See the URL in the comments within main.c for the location of the online
 * documentation.
 ******************************************************************************/

/*
 * Print out information on a single file.
 */
static void prvCreateFileInfoString( int8_t *pcBuffer, F_FIND *pxFindStruct );

/*
 * Copies an existing file into a newly created file.
 */
static portBASE_TYPE prvPerformCopy( int8_t *pcSourceFile,
							int32_t lSourceFileLength,
							int8_t *pcDestinationFile,
							int8_t *pxWriteBuffer,
							size_t xWriteBufferLen );

/*
 * Implements the DIR command.
 */
static portBASE_TYPE prvDIRCommand( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString );

/*
 * Implements the CD command.
 */
static portBASE_TYPE prvCDCommand( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString );

/*
 * Implements the DEL command.
 */
static portBASE_TYPE prvDELCommand( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString );

/*
 * Implements the TYPE command.
 */
static portBASE_TYPE prvTYPECommand( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString );

/*
 * Implements the COPY command.
 */
static portBASE_TYPE prvCOPYCommand( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString );

/*
 * Implements the TEST command.
 */
static portBASE_TYPE prvTESTFSCommand( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString );

/* Structure that defines the DIR command line command, which lists all the
files in the current directory. */
static const CLI_Command_Definition_t xDIR =
{
	( const int8_t * const ) "dir", /* The command string to type. */
	( const int8_t * const ) "\r\ndir:\r\n Lists the files in the current directory\r\n",
	prvDIRCommand, /* The function to run. */
	0 /* No parameters are expected. */
};

/* Structure that defines the CD command line command, which changes the
working directory. */
static const CLI_Command_Definition_t xCD =
{
	( const int8_t * const ) "cd", /* The command string to type. */
	( const int8_t * const ) "\r\ncd <dir name>:\r\n Changes the working directory\r\n",
	prvCDCommand, /* The function to run. */
	1 /* One parameter is expected. */
};

/* Structure that defines the TYPE command line command, which prints the
contents of a file to the console. */
static const CLI_Command_Definition_t xTYPE =
{
	( const int8_t * const ) "type", /* The command string to type. */
	( const int8_t * const ) "\r\ntype <filename>:\r\n Prints file contents to the terminal\r\n",
	prvTYPECommand, /* The function to run. */
	1 /* One parameter is expected. */
};

/* Structure that defines the DEL command line command, which deletes a file. */
static const CLI_Command_Definition_t xDEL =
{
	( const int8_t * const ) "del", /* The command string to type. */
	( const int8_t * const ) "\r\ndel <filename>:\r\n deletes a file or directory\r\n",
	prvDELCommand, /* The function to run. */
	1 /* One parameter is expected. */
};

/* Structure that defines the COPY command line command, which deletes a file. */
static const CLI_Command_Definition_t xCOPY =
{
	( const int8_t * const ) "copy", /* The command string to type. */
	( const int8_t * const ) "\r\ncopy <source file> <dest file>:\r\n Copies <source file> to <dest file>\r\n",
	prvCOPYCommand, /* The function to run. */
	2 /* Two parameters are expected. */
};

/* Structure that defines the TEST command line command, which executes some
file system driver tests. */
static const CLI_Command_Definition_t xTEST_FS =
{
	( const int8_t * const ) "test-fs", /* The command string to type. */
	( const int8_t * const ) "\r\ntest-fs:\r\n Executes file system tests.  ALL FILES WILL BE DELETED!!!\r\n",
	prvTESTFSCommand, /* The function to run. */
	0 /* No parameters are expected. */
};

/*-----------------------------------------------------------*/

void vRegisterFileSystemCLICommands( void )
{
	/* Register all the command line commands defined immediately above. */
	FreeRTOS_CLIRegisterCommand( &xDIR );
	FreeRTOS_CLIRegisterCommand( &xCD );
	FreeRTOS_CLIRegisterCommand( &xTYPE );
	FreeRTOS_CLIRegisterCommand( &xDEL );
	FreeRTOS_CLIRegisterCommand( &xCOPY );
	FreeRTOS_CLIRegisterCommand( &xTEST_FS );
}
/*-----------------------------------------------------------*/

static portBASE_TYPE prvTYPECommand( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString )
{
int8_t *pcParameter;
portBASE_TYPE xParameterStringLength, xReturn = pdTRUE;
static F_FILE *pxFile = NULL;
int iChar;
size_t xByte;
size_t xColumns = 50U;

	/* Ensure there is always a null terminator after each character written. */
	memset( pcWriteBuffer, 0x00, xWriteBufferLen );

	/* Ensure the buffer leaves space for the \r\n. */
	configASSERT( xWriteBufferLen > ( strlen( cliNEW_LINE ) * 2 ) );
	xWriteBufferLen -= strlen( cliNEW_LINE );

	if( xWriteBufferLen < xColumns )
	{
		/* Ensure the loop that uses xColumns as an end condition does not
		write off the end of the buffer. */
		xColumns = xWriteBufferLen;
	}

	if( pxFile == NULL )
	{
		/* The file has not been opened yet.  Find the file name. */
		pcParameter = ( int8_t * ) FreeRTOS_CLIGetParameter
									(
										pcCommandString,		/* The command string itself. */
										1,						/* Return the first parameter. */
										&xParameterStringLength	/* Store the parameter string length. */
									);

		/* Sanity check something was returned. */
		configASSERT( pcParameter );

		/* Attempt to open the requested file. */
		pxFile = f_open( ( const char * ) pcParameter, "r" );
	}

	if( pxFile != NULL )
	{
		/* Read the next chunk of data from the file. */
		for( xByte = 0; xByte < xColumns; xByte++ )
		{
			iChar = f_getc( pxFile );

			if( iChar == -1 )
			{
				/* No more characters to return. */
				f_close( pxFile );
				pxFile = NULL;
				break;
			}
			else
			{
				pcWriteBuffer[ xByte ] = ( int8_t ) iChar;
			}
		}
	}

	if( pxFile == NULL )
	{
		/* Either the file was not opened, or all the data from the file has
		been returned and the file is now closed. */
		xReturn = pdFALSE;
	}

	strcat( ( char * ) pcWriteBuffer, cliNEW_LINE );

	return xReturn;
}
/*-----------------------------------------------------------*/

static portBASE_TYPE prvCDCommand( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString )
{
int8_t *pcParameter;
portBASE_TYPE xParameterStringLength;
unsigned char ucReturned;
size_t xStringLength;

	/* Obtain the parameter string. */
	pcParameter = ( int8_t * ) FreeRTOS_CLIGetParameter
								(
									pcCommandString,		/* The command string itself. */
									1,						/* Return the first parameter. */
									&xParameterStringLength	/* Store the parameter string length. */
								);

	/* Sanity check something was returned. */
	configASSERT( pcParameter );

	/* Attempt to move to the requested directory. */
	ucReturned = f_chdir( ( char * ) pcParameter );

	if( ucReturned == F_NO_ERROR )
	{
		sprintf( ( char * ) pcWriteBuffer, "In: " );
		xStringLength = strlen( ( const char * ) pcWriteBuffer );
		f_getcwd( ( char * ) &( pcWriteBuffer[ xStringLength ] ), ( unsigned char ) ( xWriteBufferLen - xStringLength ) );
	}
	else
	{
		sprintf( ( char * ) pcWriteBuffer, "Error" );
	}

	strcat( ( char * ) pcWriteBuffer, cliNEW_LINE );

	return pdFALSE;
}
/*-----------------------------------------------------------*/

static portBASE_TYPE prvDIRCommand( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString )
{
static F_FIND *pxFindStruct = NULL;
unsigned char ucReturned;
portBASE_TYPE xReturn = pdFALSE;

	/* This assumes pcWriteBuffer is long enough. */
	( void ) pcCommandString;

	/* Ensure the buffer leaves space for the \r\n. */
	configASSERT( xWriteBufferLen > ( strlen( cliNEW_LINE ) * 2 ) );
	xWriteBufferLen -= strlen( cliNEW_LINE );

	if( pxFindStruct == NULL )
	{
		/* This is the first time this function has been executed since the Dir
		command was run.  Create the find structure. */
		pxFindStruct = ( F_FIND * ) pvPortMalloc( sizeof( F_FIND ) );

		if( pxFindStruct != NULL )
		{
			ucReturned = f_findfirst( "*.*", pxFindStruct );

			if( ucReturned == F_NO_ERROR )
			{
				prvCreateFileInfoString( pcWriteBuffer, pxFindStruct );
				xReturn = pdPASS;
			}
			else
			{
				snprintf( ( char * ) pcWriteBuffer, xWriteBufferLen, "Error: f_findfirst() failed." );
			}
		}
		else
		{
			snprintf( ( char * ) pcWriteBuffer, xWriteBufferLen, "Failed to allocate RAM (using heap_4.c will prevent fragmentation)." );
		}
	}
	else
	{
		/* The find struct has already been created.  Find the next file in
		the directory. */
		ucReturned = f_findnext( pxFindStruct );

		if( ucReturned == F_NO_ERROR )
		{
			prvCreateFileInfoString( pcWriteBuffer, pxFindStruct );
			xReturn = pdPASS;
		}
		else
		{
			/* There are no more files.  Free the find structure. */
			vPortFree( pxFindStruct );
			pxFindStruct = NULL;

			/* No string to return. */
			pcWriteBuffer[ 0 ] = 0x00;
		}
	}

	strcat( ( char * ) pcWriteBuffer, cliNEW_LINE );

	return xReturn;
}
/*-----------------------------------------------------------*/

static portBASE_TYPE prvDELCommand( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString )
{
int8_t *pcParameter;
portBASE_TYPE xParameterStringLength;
unsigned char ucReturned;

	/* This function assumes xWriteBufferLen is large enough! */
	( void ) xWriteBufferLen;

	/* Obtain the parameter string. */
	pcParameter = ( int8_t * ) FreeRTOS_CLIGetParameter
								(
									pcCommandString,		/* The command string itself. */
									1,						/* Return the first parameter. */
									&xParameterStringLength	/* Store the parameter string length. */
								);

	/* Sanity check something was returned. */
	configASSERT( pcParameter );

	/* Attempt to delete the file. */
	ucReturned = f_delete( ( const char * ) pcParameter );

	if( ucReturned == F_NO_ERROR )
	{
		sprintf( ( char * ) pcWriteBuffer, "%s was deleted", pcParameter );
	}
	else
	{
		sprintf( ( char * ) pcWriteBuffer, "Error" );
	}

	strcat( ( char * ) pcWriteBuffer, cliNEW_LINE );

	return pdFALSE;
}
/*-----------------------------------------------------------*/

static portBASE_TYPE prvTESTFSCommand( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString )
{
unsigned portBASE_TYPE uxOriginalPriority;

	/* Avoid compiler warnings. */
	( void ) xWriteBufferLen;
	( void ) pcCommandString;

	/* Limitations in the interaction with the Windows TCP/IP stack require
	the command console to run at the idle priority.  Raise the priority for
	the duration of the tests to ensure there are not multiple switches to the
	idle task as in the simulated environment the idle task hook function may
	include a (relatively) long delay. */
	uxOriginalPriority = uxTaskPriorityGet( NULL );
	vTaskPrioritySet( NULL, configMAX_PRIORITIES - 1 );

	f_dotest( 0 );

	/* Reset back to the original priority. */
	vTaskPrioritySet( NULL, uxOriginalPriority );

	sprintf( ( char * ) pcWriteBuffer, "%s", "Test results were sent to Windows console" );

	return pdFALSE;
}
/*-----------------------------------------------------------*/

static portBASE_TYPE prvCOPYCommand( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString )
{
int8_t *pcSourceFile, *pcDestinationFile;
portBASE_TYPE xParameterStringLength;
long lSourceLength, lDestinationLength = 0;

	/* Obtain the name of the destination file. */
	pcDestinationFile = ( int8_t * ) FreeRTOS_CLIGetParameter
								(
									pcCommandString,		/* The command string itself. */
									2,						/* Return the second parameter. */
									&xParameterStringLength	/* Store the parameter string length. */
								);

	/* Sanity check something was returned. */
	configASSERT( pcDestinationFile );

	/* Obtain the name of the source file. */
	pcSourceFile = ( int8_t * ) FreeRTOS_CLIGetParameter
								(
									pcCommandString,		/* The command string itself. */
									1,						/* Return the first parameter. */
									&xParameterStringLength	/* Store the parameter string length. */
								);

	/* Sanity check something was returned. */
	configASSERT( pcSourceFile );

	/* Terminate the string. */
	pcSourceFile[ xParameterStringLength ] = 0x00;

	/* See if the source file exists, obtain its length if it does. */
	lSourceLength = f_filelength( ( const char * ) pcSourceFile );

	if( lSourceLength == 0 )
	{
		sprintf( ( char * ) pcWriteBuffer, "Source file does not exist" );
	}
	else
	{
		/* See if the destination file exists. */
		lDestinationLength = f_filelength( ( const char * ) pcDestinationFile );

		if( lDestinationLength != 0 )
		{
			sprintf( ( char * ) pcWriteBuffer, "Error: Destination file already exists" );
		}
	}

	/* Continue only if the source file exists and the destination file does
	not exist. */
	if( ( lSourceLength != 0 ) && ( lDestinationLength == 0 ) )
	{
		if( prvPerformCopy( pcSourceFile, lSourceLength, pcDestinationFile, pcWriteBuffer, xWriteBufferLen ) == pdPASS )
		{
			sprintf( ( char * ) pcWriteBuffer, "Copy made" );
		}
		else
		{
			sprintf( ( char * ) pcWriteBuffer, "Error during copy" );
		}
	}

	strcat( ( char * ) pcWriteBuffer, cliNEW_LINE );

	return pdFALSE;
}
/*-----------------------------------------------------------*/

static portBASE_TYPE prvPerformCopy( int8_t *pcSourceFile,
							int32_t lSourceFileLength,
							int8_t *pcDestinationFile,
							int8_t *pxWriteBuffer,
							size_t xWriteBufferLen )
{
int32_t lBytesRead = 0, lBytesToRead, lBytesRemaining;
F_FILE *pxFile;
portBASE_TYPE xReturn = pdPASS;

	/* NOTE:  Error handling has been omitted for clarity. */

	while( lBytesRead < lSourceFileLength )
	{
		/* How many bytes are left? */
		lBytesRemaining = lSourceFileLength - lBytesRead;

		/* How many bytes should be read this time around the loop.  Can't
		read more bytes than will fit into the buffer. */
		if( lBytesRemaining > ( long ) xWriteBufferLen )
		{
			lBytesToRead = ( long ) xWriteBufferLen;
		}
		else
		{
			lBytesToRead = lBytesRemaining;
		}

		/* Open the source file, seek past the data that has already been
		read from the file, read the next block of data, then close the
		file again so the destination file can be opened. */
		pxFile = f_open( ( const char * ) pcSourceFile, "r" );
		if( pxFile != NULL )
		{
			f_seek( pxFile, lBytesRead, F_SEEK_SET );
			f_read( pxWriteBuffer, lBytesToRead, 1, pxFile );
			f_close( pxFile );
		}
		else
		{
			xReturn = pdFAIL;
			break;
		}

		/* Open the destination file and write the block of data to the end of
		the file. */
		pxFile = f_open( ( const char * ) pcDestinationFile, "a" );
		if( pxFile != NULL )
		{
			f_write( pxWriteBuffer, lBytesToRead, 1, pxFile );
			f_close( pxFile );
		}
		else
		{
			xReturn = pdFAIL;
			break;
		}

		lBytesRead += lBytesToRead;
	}

	return xReturn;
}
/*-----------------------------------------------------------*/

static void prvCreateFileInfoString( int8_t *pcBuffer, F_FIND *pxFindStruct )
{
const char *pcWritableFile = "writable file", *pcReadOnlyFile = "read only file", *pcDirectory = "directory";
const char * pcAttrib;

	/* Point pcAttrib to a string that describes the file. */
	if( ( pxFindStruct->attr & F_ATTR_DIR ) != 0 )
	{
		pcAttrib = pcDirectory;
	}
	else if( pxFindStruct->attr & F_ATTR_READONLY )
	{
		pcAttrib = pcReadOnlyFile;
	}
	else
	{
		pcAttrib = pcWritableFile;
	}

	/* Create a string that includes the file name, the file size and the
	attributes string. */
	sprintf( ( char * ) pcBuffer, "%s [%s] [size=%d]", pxFindStruct->filename, pcAttrib, pxFindStruct->filesize );
}
