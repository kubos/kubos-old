
/*
	FreeRTOS V5.4.2 - Copyright (C) 2009 Real Time Engineers Ltd.

	This file is part of the FreeRTOS distribution.

	FreeRTOS is free software; you can redistribute it and/or modify it	under 
	the terms of the GNU General Public License (version 2) as published by the 
	Free Software Foundation and modified by the FreeRTOS exception.
	**NOTE** The exception to the GPL is included to allow you to distribute a
	combined work that includes FreeRTOS without being obliged to provide the 
	source code for proprietary components outside of the FreeRTOS kernel.  
	Alternative commercial license and support terms are also available upon 
	request.  See the licensing section of http://www.FreeRTOS.org for full 
	license details.

	FreeRTOS is distributed in the hope that it will be useful,	but WITHOUT
	ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
	FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
	more details.

	You should have received a copy of the GNU General Public License along
	with FreeRTOS; if not, write to the Free Software Foundation, Inc., 59
	Temple Place, Suite 330, Boston, MA  02111-1307  USA.


	***************************************************************************
	*                                                                         *
	* Looking for a quick start?  Then check out the FreeRTOS eBook!          *
	* See http://www.FreeRTOS.org/Documentation for details                   *
	*                                                                         *
	***************************************************************************

	1 tab == 4 spaces!

	Please ensure to read the configuration and relevant port sections of the
	online documentation.

	http://www.FreeRTOS.org - Documentation, latest information, license and
	contact details.

	http://www.SafeRTOS.com - A version that is certified for use in safety
	critical systems.

	http://www.OpenRTOS.com - Commercial support, development, porting,
	licensing and training services.
*/

/*
	Implements a simplistic WEB server.  Every time a connection is made and
	data is received a dynamic page that shows the current TCP/IP statistics
	is generated and returned.  The connection is then closed.
*/


/*------------------------------------------------------------------------------*/
/*                            PROTOTYPES                                        */
/*------------------------------------------------------------------------------*/

/* Standard includes. */
#include <stdio.h>
#include <string.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* Demo includes. */
#include "BasicWEB.h"

/* lwIP includes. */
#include "lwip/api.h"
#include "lwip/tcpip.h"
#include "lwip/memp.h"
#include "lwip/stats.h"
#include "netif/loopif.h"
#include "lcd.h"
#include "httpd.h"

#define lwipTCP_STACK_SIZE			600


/*------------------------------------------------------------------------------*/
/*                            GLOBALS                                          */
/*------------------------------------------------------------------------------*/
static struct netif EMAC_if;

/*------------------------------------------------------------------------------*/
/*                            FUNCTIONS                                         */
/*------------------------------------------------------------------------------*/


void vlwIPInit( void )
{
    /* Initialize lwIP and its interface layer. */
	sys_init();
	mem_init();								
	memp_init();
	pbuf_init();
	netif_init();
	ip_init();
	sys_set_state(( signed portCHAR * ) "lwIP", lwipTCP_STACK_SIZE);
	tcpip_init( NULL, NULL );	
	sys_set_default_state();
}
/*------------------------------------------------------------*/

void vBasicWEBServer( void *pvParameters )
{
struct ip_addr xIpAddr, xNetMast, xGateway;
extern err_t ethernetif_init( struct netif *netif );

    /* Parameters are not used - suppress compiler error. */
    ( void ) pvParameters;

    /* Create and configure the EMAC interface. */
    IP4_ADDR( &xIpAddr, emacIPADDR0, emacIPADDR1, emacIPADDR2, emacIPADDR3 );
    IP4_ADDR( &xNetMast, emacNET_MASK0, emacNET_MASK1, emacNET_MASK2, emacNET_MASK3 );
    IP4_ADDR( &xGateway, emacGATEWAY_ADDR0, emacGATEWAY_ADDR1, emacGATEWAY_ADDR2, emacGATEWAY_ADDR3 );
    netif_add( &EMAC_if, &xIpAddr, &xNetMast, &xGateway, NULL, ethernetif_init, tcpip_input );

    /* make it the default interface */
    netif_set_default( &EMAC_if );

    /* bring it up */
    netif_set_up(&EMAC_if);

    /* Initialize HTTP */
    httpd_init();

	/* Nothing else to do.  No point hanging around. */
	vTaskDelete( NULL );
}


