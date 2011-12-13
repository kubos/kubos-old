/*
    FreeRTOS V7.1.0 - Copyright (C) 2011 Real Time Engineers Ltd.
	

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

    http://www.FreeRTOS.org - Documentation, latest information, license and
    contact details.

    http://www.SafeRTOS.com - A version that is certified for use in safety
    critical systems.

    http://www.OpenRTOS.com - Commercial support, development, porting,
    licensing and training services.
*/

/*
	- DESCRIPTOR DEFINITIONS -
*/

/* String descriptors used during the enumeration process.
These take the form:

{
	Length of descriptor,
	Descriptor type,
	Data
}
*/

const char pxLanguageStringDescriptor[] =
{
	4,
	usbDESCRIPTOR_TYPE_STRING,
	0x09, 0x04
};

const char pxManufacturerStringDescriptor[] = 
{
	18,
	usbDESCRIPTOR_TYPE_STRING,

	'F', 0x00, 'r', 0x00, 'e', 0x00, 'e', 0x00, 'R', 0x00, 'T', 0x00, 'O', 0x00, 'S', 0x00
};

const char pxProductStringDescriptor[] = 
{
	36,
	usbDESCRIPTOR_TYPE_STRING,

	'F', 0x00, 'r', 0x00, 'e', 0x00, 'e', 0x00, 'R', 0x00, 'T', 0x00, 'O', 0x00, 'S', 0x00, ' ', 0x00, 'C', 0x00, 'D', 0x00,
	'C', 0x00, ' ', 0x00, 'D', 0x00, 'E', 0x00, 'M', 0x00, 'O', 0x00
};

const char pxConfigurationStringDescriptor[] = 
{
	38,
	usbDESCRIPTOR_TYPE_STRING,

	'C', 0x00, 'o', 0x00, 'n', 0x00, 'f', 0x00, 'i', 0x00, 'g', 0x00, 'u', 0x00, 'r', 0x00, 'a', 0x00, 't', 0x00, 'i', 0x00,
	'o', 0x00, 'n', 0x00, ' ', 0x00, 'N', 0x00, 'a', 0x00, 'm', 0x00, 'e', 0x00
};

const char pxInterfaceStringDescriptor[] = 
{
	30,
	usbDESCRIPTOR_TYPE_STRING,

	'I', 0x00, 'n', 0x00, 't', 0x00, 'e', 0x00, 'r', 0x00, 'f', 0x00, 'a', 0x00, 'c', 0x00, 'e', 0x00, ' ', 0x00, 'N', 0x00,
	'a', 0x00, 'm', 0x00, 'e', 0x00
};

/* Device should properly be 0x134A:0x9001, using 0x05F9:0xFFFF for Linux testing */
const char pxDeviceDescriptor[] = 
{
	/* Device descriptor */
	0x12,								/* bLength				*/
	0x01,								/* bDescriptorType		*/
	0x10, 0x01,							/* bcdUSBL				*/
	0x02,								/* bDeviceClass:		*/
	0x00,								/* bDeviceSubclass:		*/
	0x00,								/* bDeviceProtocol:		*/
	0x08,								/* bMaxPacketSize0		*/
	0x03, 0xEB,							/* idVendorL			*/
	0x20, 0x09,							/* idProductL			*/
	0x10, 0x01,							/* bcdDeviceL			*/
	usbMANUFACTURER_STRING,  			/* iManufacturer		*/
	usbPRODUCT_STRING,					/* iProduct				*/
	0x00,								/* SerialNumber			*/
	0x01								/* bNumConfigs			*/
};

const char pxConfigDescriptor[] = {

	/* Configuration 1 descriptor
	Here we define two interfaces (0 and 1) and a total of 3 endpoints.
	Interface 0 is a CDC Abstract Control Model interface with one interrupt-in endpoint.
	Interface 1 is a CDC Data Interface class, with a bulk-in and bulk-out endpoint.
	Endpoint 0 gets used as the CDC management element.
	*/
	0x09,				/* CbLength								*/
	0x02,				/* CbDescriptorType					  	*/
	0x43, 0x00,			/* CwTotalLength 2 EP + Control		?	*/
	0x02,				/* CbNumInterfaces			  			*/
	0x01,				/* CbConfigurationValue					*/
	usbCONFIGURATION_STRING,/* CiConfiguration					*/
	usbBUS_POWERED,		/* CbmAttributes Bus powered + Remote Wakeup*/
	0x32,				/* CMaxPower: 100mA						*/

	/* Communication Class Interface Descriptor Requirement		*/
	0x09,				/* bLength								*/
	0x04,				/* bDescriptorType						*/
	0x00,				/* bInterfaceNumber						*/
	0x00,				/* bAlternateSetting					*/
	0x01,				/* bNumEndpoints						*/
	0x02,				/* bInterfaceClass: Comm Interface Class */
	0x02,				/* bInterfaceSubclass: Abstract Control Model*/
	0x00,				/* bInterfaceProtocol					*/
	usbINTERFACE_STRING,/* iInterface							*/

	/* Header Functional Descriptor								*/
	0x05,				/* bLength								*/
	0x24,				/* bDescriptor type: CS_INTERFACE		*/
	0x00,				/* bDescriptor subtype: Header Func Desc*/
	0x10, 0x01,			/* bcdCDC:1.1  							*/

	/* ACM Functional Descriptor								*/
	0x04,				/* bFunctionLength						*/
	0x24,				/* bDescriptor type: CS_INTERFACE		*/
	0x02,				/* bDescriptor subtype: ACM Func Desc	*/
	0x00,				/* bmCapabilities: We don't support squat*/

	/* Union Functional Descriptor								*/
	0x05,				/* bFunctionLength						*/
	0x24,				/* bDescriptor type: CS_INTERFACE		*/
	0x06,				/* bDescriptor subtype: Union Func Desc	*/
	0x00,				/* bMasterInterface: CDC Interface		*/
	0x01,				/* bSlaveInterface0: Data Class Interface*/

	/* Call Management Functional Descriptor
	0 in D1 and D0 indicates that device does not handle call management*/
	0x05,				/* bFunctionLength						*/
	0x24,				/* bDescriptor type: CS_INTERFACE		*/
	0x01,				/* bDescriptor subtype: Call Management Func*/
	0x01,				/* bmCapabilities: D1 + D0				*/
	0x01,				/* bDataInterface: Data Class Interface 1*/

	/* CDC Control - Endpoint 3 descriptor
	This endpoint serves as a notification element.				*/

	0x07,				/* bLength								*/
	0x05,				/* bDescriptorType						*/
	0x83,				/* bEndpointAddress, Endpoint 03 - IN	*/
	0x03,				/* bmAttributes	  INT					*/
	0x08, 0x00,			/* wMaxPacketSize: 8 bytes		   		*/
	0xFF,				/* bInterval							*/

	/* Data Class Interface Descriptor Requirement				*/
	0x09,				/* bLength								*/
	0x04,				/* bDescriptorType						*/
	0x01,				/* bInterfaceNumber						*/
	0x00,				/* bAlternateSetting					*/
	0x02,				/* bNumEndPoints						*/
	0x0A,				/* bInterfaceClass						*/
	0x00,				/* bInterfaceSubclass					*/
	0x00,				/* bInterfaceProtocol					*/
	0x00,				/* iInterface							*/

	/* CDC Data - Endpoint 1 descriptor */
	0x07,				/* bLenght								*/
	0x05,				/* bDescriptorType						*/
	0x01,				/* bEndPointAddress, Endpoint 01 - OUT	*/
	0x02,				/* bmAttributes BULK					*/
	64,					/* wMaxPacketSize						*/
	0x00,
	0x00,				/* bInterval							*/

	/* CDC Data - Endpoint 2 descriptor */
	0x07,				/* bLength								*/
	0x05,				/* bDescriptorType						*/
	0x82,				/* bEndPointAddress, Endpoint 02 - IN	*/
	0x02,				/* bmAttributes BULK					*/
	64,					/* wMaxPacketSize						*/
	0x00,
	0x00				/* bInterval							*/
};

