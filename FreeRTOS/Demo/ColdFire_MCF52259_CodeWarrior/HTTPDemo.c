/*
    FreeRTOS V7.2.0 - Copyright (C) 2012 Real Time Engineers Ltd.
	

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

/*
    Implements a simplistic WEB server.  Every time a connection is made and
    data is received a dynamic page that shows the current TCP/IP statistics
    is generated and returned.  The connection is then closed.

    This file was adapted from a FreeRTOS lwIP slip demo supplied by a third
    party.
*/

/* ------------------------ System includes ------------------------------- */


/* ------------------------ FreeRTOS includes ----------------------------- */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* ------------------------ lwIP includes --------------------------------- */
#include "lwip/api.h"
#include "lwip/tcpip.h"
#include "lwip/ip.h"
#include "lwip/memp.h"
#include "lwip/stats.h"
#include "netif/loopif.h"

/* ------------------------ Project includes ------------------------------ */
#include "common.h"

#include "HTTPDemo.h"

/* ------------------------ Defines --------------------------------------- */
/* The size of the buffer in which the dynamic WEB page is created. */
#define webMAX_PAGE_SIZE        ( 1024 ) /*FSL: buffer containing array*/

/* Standard GET response. */
#define webHTTP_OK  "HTTP/1.0 200 OK\r\nContent-type: text/html\r\n\r\n"

/* The port on which we listen. */
#define webHTTP_PORT            ( 80 )

/* Delay on close error. */
#define webSHORT_DELAY          ( 10 )

/* Format of the dynamic page that is returned on each connection. */
#define webHTML_START \
"<html>\
<head>\
</head>\
<BODY onLoad=\"window.setTimeout(&quot;location.href='index.html'&quot;,1000)\"bgcolor=\"#CCCCff\">\
\r\n\r\nPage Hits = "

#define webHTML_END \
"\r\n" \
"</pre>\r\n" \
"</BODY>\r\n" \
"</html>"

#if INCLUDE_uxTaskGetStackHighWaterMark
	static volatile unsigned portBASE_TYPE uxHighWaterMark_web = 0;
#endif

/* ------------------------ Prototypes ------------------------------------ */
static void     vProcessConnection( struct netconn *pxNetCon );

/*------------------------------------------------------------*/

/*
 * Process an incoming connection on port 80.
 *
 * This simply checks to see if the incoming data contains a GET request, and
 * if so sends back a single dynamically created page.  The connection is then
 * closed.  A more complete implementation could create a task for each
 * connection.
 */
static void vProcessConnection( struct netconn *pxNetCon )
{
    static portCHAR cDynamicPage[webMAX_PAGE_SIZE], cPageHits[11];
    struct netbuf  *pxRxBuffer;
    portCHAR       *pcRxString;
    unsigned portSHORT usLength;
    static unsigned portLONG ulPageHits = 0;

    /* We expect to immediately get data. */
    pxRxBuffer = netconn_recv( pxNetCon );

    if( pxRxBuffer != NULL )
    {
        /* Where is the data? */
        netbuf_data( pxRxBuffer, ( void * )&pcRxString, &usLength );

        /* Is this a GET?  We don't handle anything else. */
        if( !strncmp( pcRxString, "GET", 3 ) )
        {
            pcRxString = cDynamicPage;

            /* Update the hit count. */
            ulPageHits++;
            sprintf( cPageHits, "%d", (int)ulPageHits );

            /* Write out the HTTP OK header. */            
            netconn_write( pxNetCon, webHTTP_OK, ( u16_t ) strlen( webHTTP_OK ), NETCONN_COPY );

            /* Generate the dynamic page...
            
               ... First the page header. */
            strcpy( cDynamicPage, webHTML_START );
            /* ... Then the hit count... */
            strcat( cDynamicPage, cPageHits );
            
            strcat( cDynamicPage,
                    "<p><pre>Task          State  Priority  Stack      #<br>************************************************<br>" );
            /* ... Then the list of tasks and their status... */
            vTaskList( ( signed portCHAR * )cDynamicPage + strlen( cDynamicPage ) );            
            
            /* ... Finally the page footer. */
            strcat( cDynamicPage, webHTML_END );

            /* Write out the dynamically generated page. */
            netconn_write( pxNetCon, cDynamicPage, ( u16_t ) strlen( cDynamicPage ), NETCONN_COPY );
        }
        netbuf_delete( pxRxBuffer );
    }
    netconn_close( pxNetCon );
}

/*------------------------------------------------------------*/

void vlwIPInit( void )
{
    /* Initialize lwIP and its interface layer. */
    tcpip_init( NULL, NULL );
}

/*------------------------------------------------------------*/

void vBasicWEBServer( void *pvParameters )
{
    struct netconn *pxHTTPListener, *pxNewConnection;
    struct ip_addr  xIpAddr, xNetMast, xGateway;
    static struct netif fec523x_if;
	extern err_t ethernetif_init(struct netif *netif);

    /* Parameters are not used - suppress compiler error. */
    ( void )pvParameters;

	vlwIPInit();

    /* Create and configure the FEC interface. */
    IP4_ADDR( &xIpAddr, configIP_ADDR0, configIP_ADDR1, configIP_ADDR2, configIP_ADDR3 );
    IP4_ADDR( &xNetMast, configNET_MASK0, configNET_MASK1, configNET_MASK2, configNET_MASK3 );
    IP4_ADDR( &xGateway, configGW_ADDR0, configGW_ADDR1, configGW_ADDR2, configGW_ADDR3 );
    netif_add( &fec523x_if, &xIpAddr, &xNetMast, &xGateway, NULL, ethernetif_init, tcpip_input );

    /* make it the default interface */
    netif_set_default( &fec523x_if );

    /* bring it up */
    netif_set_up( &fec523x_if );

    /* Create a new tcp connection handle */
    pxHTTPListener = netconn_new( NETCONN_TCP );
    netconn_bind( pxHTTPListener, NULL, webHTTP_PORT );
    netconn_listen( pxHTTPListener );

    /* Loop forever */
    for( ;; )
    {	
        /* Wait for connection. */
        pxNewConnection = netconn_accept( pxHTTPListener );

        if( pxNewConnection != NULL )
        {
            /* Service connection. */
            vProcessConnection( pxNewConnection );
            while( netconn_delete( pxNewConnection ) != ERR_OK )
            {
                vTaskDelay( webSHORT_DELAY );
            }
        }
    }
}
