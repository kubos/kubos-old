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

#ifndef I2C_H
#define I2C_H

/* Structure used to capture the I2C message details.  The structure is then
 * queued for processing by the I2C ISR. 
 */
typedef struct AN_I2C_MESSAGE
{
	portLONG lMessageLength;					/*< How many bytes of data to send or received - excluding the buffer address. */
	unsigned portCHAR ucSlaveAddress;			/*< The slave address of the WIZnet on the I2C bus. */
	unsigned portCHAR ucBufferAddressLowByte;	/*< The address within the WIZnet device to which data should be read from / written to. */
	unsigned portCHAR ucBufferAddressHighByte;	/*< As above, high byte. */
	xSemaphoreHandle xMessageCompleteSemaphore;	/*< Contains a reference to a semaphore if the application tasks wants notifying when the message has been transacted. */
	unsigned portCHAR *pucBuffer;				/*< Pointer to the buffer from where data will be read for transmission, or into which received data will be placed. */
} xI2CMessage;

/* Constants to use as the ulDirection parameter of i2cMessage(). */
#define i2cWRITE				( ( unsigned portLONG ) 0 )
#define i2cREAD					( ( unsigned portLONG ) 1 )

/**
 * Must be called once before any calls to i2cMessage.
 */
void i2cInit( void );

/**
 * Send or receive a message over the I2C bus.
 *
 * @param pucMessage	 The data to be transmitted or the buffer into which
 *						 received data will be placed. 
 *
 * @param lMessageLength The number of bytes to either transmit or receive.
 *
 * @param ucSlaveAddress The slave address of the WIZNet device on the I2C bus.
 *
 * @param usBufferAddress The address within the WIZNet device to which data is
 *						 either written to or read from.  The WIZnet has it's
 *						 own Rx and Tx buffers.
 *
 * @param ulDirection	 Must be either i2cWRITE or i2cREAD as #defined above.
 *
 * @param xMessageCompleteSemaphore
 *						 Can be used to pass a semaphore reference if the 
 *						 calling task want notification of when the message has
 *						 completed.  Otherwise NULL can be passed.
 * 
 * @param xBlockTime	 The time to wait for a space in the message queue to 
 *						 become available should one not be available 
 *						 immediately.
 */
void i2cMessage( const unsigned portCHAR * const pucMessage, portLONG lMessageLength, unsigned portCHAR ucSlaveAddress, unsigned portSHORT usBufferAddress, unsigned portLONG ulDirection, xSemaphoreHandle xMessageCompleteSemaphore, portTickType xBlockTime );

#endif

