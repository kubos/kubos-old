/*
	FreeRTOS.org V5.1.2 - Copyright (C) 2003-2009 Richard Barry.

	This file is part of the FreeRTOS.org distribution.

	FreeRTOS.org is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	FreeRTOS.org is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with FreeRTOS.org; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

	A special exception to the GPL can be applied should you wish to distribute
	a combined work that includes FreeRTOS.org, without being obliged to provide
	the source code for any proprietary components.  See the licensing section
	of http://www.FreeRTOS.org for full details of how and when the exception
	can be applied.

	***************************************************************************
	See http://www.FreeRTOS.org for documentation, latest information, license
	and contact details.  Please ensure to read the configuration and relevant
	port sections of the online documentation.
	***************************************************************************
*/


/* Task that controls the uIP TCP/IP stack. */


/* Standard includes. */
#include <string.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* uip includes. */
#include "uip.h"
#include "uip_arp.h"
#include "httpd.h"
#include "timer.h"
#include "clock-arch.h"

/* Demo includes. */
#include "FEC.h"
#include "partest.h"


/*-----------------------------------------------------------*/

/* Shortcut to the header within the Rx buffer. */
#define xHeader ((struct uip_eth_hdr *) &uip_buf[ 0 ])

/*-----------------------------------------------------------*/

/*
 * Port functions required by the uIP stack.
 */
void clock_init( void );
clock_time_t clock_time( void );
extern void timer_set(struct timer *t, clock_time_t interval);
extern int timer_expired(struct timer *t);
extern void timer_reset(struct timer *t);

/*-----------------------------------------------------------*/

/* The semaphore used by the ISR to wake the uIP task. */
extern xSemaphoreHandle xFECSemaphore;

/*-----------------------------------------------------------*/

void clock_init(void)
{
	/* This is done when the scheduler starts. */
}
/*-----------------------------------------------------------*/

/* Define clock functions here to avoid header file name clash between uIP
and the Luminary Micro driver library. */
clock_time_t clock_time( void )
{
	return xTaskGetTickCount();
}
/*-----------------------------------------------------------*/

void vuIP_Task( void *pvParameters )
{
portBASE_TYPE i;
uip_ipaddr_t xIPAddr;
struct timer periodic_timer, arp_timer;

	/* To prevent compiler warnings. */
	( void ) pvParameters;

	/* Initialise the uIP stack. */
	timer_set( &periodic_timer, configTICK_RATE_HZ / 2 );
	timer_set( &arp_timer, configTICK_RATE_HZ * 10 );
	uip_init();
	uip_ipaddr( xIPAddr, configIP_ADDR0, configIP_ADDR1, configIP_ADDR2, configIP_ADDR3 );
	uip_sethostaddr( xIPAddr );

	/* Initialise the WEB server. */
	httpd_init();

	/* Initialise the Ethernet controller peripheral. */
	vFECInit();

	for( ;; )
	{
		/* Is there received data ready to be processed? */
		uip_len = usFECGetRxedData();

		if( uip_len > 0 )
		{
			/* Standard uIP loop taken from the uIP manual. */

			if( xHeader->type == htons( UIP_ETHTYPE_IP ) )
			{
				uip_arp_ipin();
				uip_input();

				/* If the above function invocation resulted in data that
				should be sent out on the network, the global variable
				uip_len is set to a value > 0. */
				if( uip_len > 0 )
				{
					uip_arp_out();
					vFECSendData();
				}
				else
				{
					/* If we are not sending data then let the FEC driver know
					the buffer is no longer required. */
					vFECRxProcessingCompleted();
				}
			}
			else if( xHeader->type == htons( UIP_ETHTYPE_ARP ) )
			{
				uip_arp_arpin();

				/* If the above function invocation resulted in data that
				should be sent out on the network, the global variable
				uip_len is set to a value > 0. */
				if( uip_len > 0 )
				{
					vFECSendData();
				}
				else
				{
					/* If we are not sending data then let the FEC driver know
					the buffer is no longer required. */
					vFECRxProcessingCompleted();
				}
			}
			else
			{
				/* If we are not sending data then let the FEC driver know
				the buffer is no longer required. */
				vFECRxProcessingCompleted();
			}
		}
		else
		{
			if( timer_expired( &periodic_timer ) )
			{
				timer_reset( &periodic_timer );
				for( i = 0; i < UIP_CONNS; i++ )
				{
					uip_periodic( i );

					/* If the above function invocation resulted in data that
					should be sent out on the network, the global variable
					uip_len is set to a value > 0. */
					if( uip_len > 0 )
					{
						uip_arp_out();
						vFECSendData();
					}
				}

				/* Call the ARP timer function every 10 seconds. */
				if( timer_expired( &arp_timer ) )
				{
					timer_reset( &arp_timer );
					uip_arp_timer();
				}
			}
			else
			{
				/* We did not receive a packet, and there was no periodic
				processing to perform.  Block for a fixed period.  If a packet
				is received during this period we will be woken by the ISR
				giving us the Semaphore. */
				xSemaphoreTake( xFECSemaphore, configTICK_RATE_HZ / 2 );
			}
		}
	}
}
/*-----------------------------------------------------------*/



