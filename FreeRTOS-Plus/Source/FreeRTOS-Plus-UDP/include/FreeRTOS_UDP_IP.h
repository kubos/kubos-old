/*
 * FreeRTOS+UDP V1.0.0 (C) 2013 Real Time Engineers ltd.
 *
 * FreeRTOS+UDP is an add-on component to FreeRTOS.  It is not, in itself, part
 * of the FreeRTOS kernel.  FreeRTOS+UDP is licensed separately from FreeRTOS,
 * and uses a different license to FreeRTOS.  FreeRTOS+UDP uses a dual license
 * model, information on which is provided below:
 *
 * - Open source licensing -
 * FreeRTOS+UDP is a free download and may be used, modified and distributed
 * without charge provided the user adheres to version two of the GNU General
 * Public license (GPL) and does not remove the copyright notice or this text.
 * The GPL V2 text is available on the gnu.org web site, and on the following
 * URL: http://www.FreeRTOS.org/gpl-2.0.txt
 *
 * - Commercial licensing -
 * Businesses and individuals who wish to incorporate FreeRTOS+UDP into
 * proprietary software for redistribution in any form must first obtain a
 * (very) low cost commercial license - and in-so-doing support the maintenance,
 * support and further development of the FreeRTOS+UDP product.  Commercial
 * licenses can be obtained from http://shop.freertos.org and do not require any
 * source files to be changed.
 *
 * FreeRTOS+UDP is distributed in the hope that it will be useful.  You cannot
 * use FreeRTOS+UDP unless you agree that you use the software 'as is'.
 * FreeRTOS+UDP is provided WITHOUT ANY WARRANTY; without even the implied
 * warranties of NON-INFRINGEMENT, MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. Real Time Engineers Ltd. disclaims all conditions and terms, be they
 * implied, expressed, or statutory.
 *
 * 1 tab == 4 spaces!
 *
 * http://www.FreeRTOS.org
 * http://www.FreeRTOS.org/udp
 *
 */

#ifndef FREERTOS_IP_H
#define FREERTOS_IP_H

/* Use in FreeRTOSIPConfig.h. */
#define FREERTOS_LITTLE_ENDIAN	0
#define FREERTOS_BIG_ENDIAN		1

/* Application level configuration options. */
#include "FreeRTOSIPConfig.h"
#include "FreeRTOSIPConfigDefaults.h"
#include "IPTraceMacroDefaults.h"

/* The number of octets in the MAC and IP addresses respectively. */
#define ipMAC_ADDRESS_LENGTH_BYTES ( 6 )
#define ipIP_ADDRESS_LENGTH_BYTES ( 4 )

#include "pack_struct_start.h"
struct xMAC_ADDRESS
{
	uint8_t ucBytes[ ipMAC_ADDRESS_LENGTH_BYTES ];
}
#include "pack_struct_end.h"
typedef struct xMAC_ADDRESS xMACAddress_t;

typedef enum eNETWORK_EVENTS
{
	eNetworkUp,		/* The network is configured. */
	eNetworkDown	/* The network connection has been lost. */
} eIPCallbackEvent_t;

typedef enum ePING_REPLY_STATUS
{
	eSuccess = 0,		/* A correct reply has been received for an outgoing ping. */
	eInvalidChecksum,	/* A reply was received for an outgoing ping but the checksum of the reply was incorrect. */
	eInvalidData		/* A reply was received to an outgoing ping but the payload of the reply was not correct. */
} ePingReplyStatus_t;

/* Endian related definitions. */
#if( ipconfigBYTE_ORDER == FREERTOS_LITTLE_ENDIAN )

	uint16_t FreeRTOS_htons( uint16_t usIn );
	uint32_t FreeRTOS_htonl( uint32_t ulIn );

#else

	#define FreeRTOS_htons( x ) ( x )
	#define FreeRTOS_htonl( x ) ( x )

#endif /* ipconfigBYTE_ORDER == FREERTOS_LITTLE_ENDIAN */

#define FreeRTOS_ntohs( x ) FreeRTOS_htons( x )
#define FreeRTOS_ntohl( x ) FreeRTOS_htonl( x )

/**
 * FULL, UP-TO-DATE AND MAINTAINED REFERENCE DOCUMENTATION FOR ALL THESE
 * FUNCTIONS IS AVAILABLE ON THE FOLLOWING URL:
 * http://www.FreeRTOS.org/FreeRTOS-Plus/FreeRTOS_Plus_UDP/FreeRTOS_UDP_API_Functions.shtml
 */
portBASE_TYPE FreeRTOS_IPInit( const uint8_t ucIPAddress[ ipIP_ADDRESS_LENGTH_BYTES ], const uint8_t ucNetMask[ ipIP_ADDRESS_LENGTH_BYTES ], const uint8_t ucGatewayAddress[ ipIP_ADDRESS_LENGTH_BYTES ], const uint8_t ucDNSServerAddress[ ipIP_ADDRESS_LENGTH_BYTES ], const uint8_t ucMACAddress[ ipMAC_ADDRESS_LENGTH_BYTES ] );
void * FreeRTOS_GetUDPPayloadBuffer( size_t xRequestedSizeBytes, portTickType xBlockTimeTicks );
void FreeRTOS_GetAddressConfiguration( uint32_t *pulIPAddress, uint32_t *pulNetMask, uint32_t *pulGatewayAddress, uint32_t *pulDNSServerAddress );
portBASE_TYPE FreeRTOS_SendPingRequest( uint32_t ulIPAddress, size_t xNumberOfBytesToSend, portTickType xBlockTimeTicks );
void vApplicationIPNetworkEventHook( eIPCallbackEvent_t eNetworkEvent );
void vApplicationPingReplyHook( ePingReplyStatus_t eStatus, uint16_t usIdentifier );
void FreeRTOS_ReleaseUDPPayloadBuffer( void *pvBuffer );
uint8_t * FreeRTOS_GetMACAddress( void );

#endif /* FREERTOS_IP_H */













