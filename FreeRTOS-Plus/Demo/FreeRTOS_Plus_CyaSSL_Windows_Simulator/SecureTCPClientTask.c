/*
    FreeRTOS V7.5.3 - Copyright (C) 2013 Real Time Engineers Ltd. 
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

#pragma comment( lib, "ws2_32.lib" )

/* Win32 includes. */
#include <WinSock2.h>

/* CyaSSL includes. */
#include "cyassl/ssl.h"

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/*-----------------------------------------------------------*/

/* The CyaSSL context for the client. */
static CYASSL_CTX* xCyaSSL_ClientContext = NULL;

/*-----------------------------------------------------------*/

/* See the comments at the top of main.c. */
void vSecureTCPClientTask( void *pvParameters )
{
SOCKET xClientSocket;
struct sockaddr_in xConnection;
CYASSL* xCyaSSL_Object;
WORD wVersionRequested;
WSADATA xWSAData;
uint8_t cString[ 50 ];
portBASE_TYPE lReturned;
uint32_t ulCount = 0UL;

	/* Remove compiler warning about unused parameters. */
	( void ) pvParameters;

	/* Prepare to use WinSock. */
	wVersionRequested = MAKEWORD( 2, 2 );
	configASSERT( WSAStartup( wVersionRequested, &xWSAData ) == 0 );

	/* Set family and port for client socket. */
	memset( ( void * ) &xConnection, 0x00, sizeof( struct sockaddr_in ) );
	xConnection.sin_family = AF_INET;
	xConnection.sin_addr.s_addr = inet_addr("127.0.0.1");
	xConnection.sin_port = htons( configTCP_PORT_NUMBER );

    /* Attempt to create a context that uses the TLS V1 server protocol. */
    xCyaSSL_ClientContext = CyaSSL_CTX_new( CyaTLSv1_client_method() );
	configASSERT( xCyaSSL_ClientContext );

    /* Load the CA certificate. */
    lReturned = CyaSSL_CTX_load_verify_locations( xCyaSSL_ClientContext, "ca-cert.pem", 0 );
	configASSERT( lReturned == SSL_SUCCESS );

	for( ;; )
	{
		/* Create the socket. */
		xClientSocket = socket( AF_INET, SOCK_STREAM, 0 );
		configASSERT( xClientSocket != INVALID_SOCKET );

		/* Connect to the secure server. */
		if( connect( xClientSocket, ( SOCKADDR * ) &xConnection, sizeof( xConnection ) ) == 0 )
		{
			/* The connect was successful.  Create a CyaSSL object to associate 
			with this connection. */
			xCyaSSL_Object = CyaSSL_new( xCyaSSL_ClientContext );
        
			if( xCyaSSL_Object != NULL )
			{
				/* Associate the created CyaSSL object with the connected 
				socket. */
				lReturned = CyaSSL_set_fd( xCyaSSL_Object, xClientSocket );
				configASSERT( lReturned == SSL_SUCCESS );

				/* The count is used to differentiate between messages sent to
				the server, and to break out of the do while loop below. */
				ulCount = 0UL;

				do
				{
					/* Create the string that is sent to the secure server. */
					sprintf( ( char * ) cString, "Message number %lu\r\n", ulCount );

					/* The next line is the secure equivalent of the standard 
					sockets call:
					lReturned = send( xClientSocket, cString, strlen( cString ) + 1, 0 ); */
					lReturned = CyaSSL_write( xCyaSSL_Object, ( const char * ) cString, strlen( ( const char * ) cString ) + 1 );
					
					
					/* Short delay to prevent the messages streaming up the
					console too quickly. */
					vTaskDelay( 5 );
					ulCount++;

				} while( ( lReturned != SOCKET_ERROR ) && ( ulCount < 10UL ) );
			}
						
			CyaSSL_free( xCyaSSL_Object );
			closesocket( xClientSocket );

			/* Delay for a short time before starting over. */
			vTaskDelay( 50 );
		}
	}
}
/*-----------------------------------------------------------*/

