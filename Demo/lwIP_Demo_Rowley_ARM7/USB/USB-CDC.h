/*
	FreeRTOS V5.4.1 - Copyright (C) 2009 Real Time Engineers Ltd.

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

#ifndef USB_CDC_H
#define USB_CDC_H

#include "usb.h"

#define USB_CDC_QUEUE_SIZE    200

/* Structure used to take a snapshot of the USB status from within the ISR. */
typedef struct X_ISR_STATUS
{
	unsigned portLONG ulISR;
	unsigned portLONG ulCSR0;
	unsigned portCHAR ucFifoData[ 8 ];
} xISRStatus;

/* Structure used to hold the received requests. */
typedef struct 
{
	unsigned portCHAR ucReqType;
	unsigned portCHAR ucRequest;
	unsigned portSHORT usValue;
	unsigned portSHORT usIndex;
	unsigned portSHORT usLength;
} xUSB_REQUEST;

typedef enum
{
	eNOTHING,
	eJUST_RESET,
	eJUST_GOT_CONFIG,
	eJUST_GOT_ADDRESS,
	eSENDING_EVEN_DESCRIPTOR,
	eREADY_TO_SEND
} eDRIVER_STATE;

/* Structure used to control the data being sent to the host. */
typedef struct
{
	unsigned portCHAR ucBuffer[ usbMAX_CONTROL_MESSAGE_SIZE ];
	unsigned portLONG ulNextCharIndex;
	unsigned portLONG ulTotalDataLength;
} xCONTROL_MESSAGE;

/*-----------------------------------------------------------*/
void vUSBCDCTask( void *pvParameters );

/* Send cByte down the USB port.  Characters are simply buffered and not
sent unless the port is connected. */
void vUSBSendByte( portCHAR cByte );


#endif


