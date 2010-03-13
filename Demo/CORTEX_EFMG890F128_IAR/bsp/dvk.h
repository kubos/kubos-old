/**************************************************************************//**
 * @file
 * @brief DVK Board Support, master header file
 * @author Energy Micro AS
 * @version 1.0.1
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2009 Energy Micro AS, http://www.energymicro.com</b>
 ******************************************************************************
 *
 * This source code is the property of Energy Micro AS. The source and compiled
 * code may only be used on Energy Micro "EFM32" microcontrollers.
 *
 * This copyright notice may not be removed from the source code nor changed.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Energy Micro AS has no
 * obligation to support this Software. Energy Micro AS is providing the
 * Software "AS IS", with no express or implied warranties of any kind,
 * including, but not limited to, any implied warranties of merchantability
 * or fitness for any particular purpose or warranties against infringement
 * of any proprietary rights of a third party.
 *
 * Energy Micro AS will not be liable for any consequential, incidental, or
 * special damages, or any other relief, or for any claim by any third party,
 * arising from your use of this Software.
 *
 *****************************************************************************/

#ifndef __DVK_H
#define __DVK_H

#include <stdint.h>
#include "dvk_boardcontrol.h"
#include "dvk_bcregisters.h"

/* IF not user overrides default, try to decide DVK access interface based on
 * part number */
#ifndef DVK_SPI_CONTROL
#ifndef DVK_EBI_CONTROL

#if defined(EFM32G200F16)
#define DVK_SPI_CONTROL
#elif defined(EFM32G200F32)
#define DVK_SPI_CONTROL
#elif defined(EFM32G200F64)
#define DVK_SPI_CONTROL
#elif defined(EFM32G210F128)
#define DVK_SPI_CONTROL
#elif defined(EFM32G230F128)
#define DVK_SPI_CONTROL
#elif defined(EFM32G230F32)
#define DVK_SPI_CONTROL
#elif defined(EFM32G230F64)
#define DVK_SPI_CONTROL
#elif defined(EFM32G280F128)
#define DVK_EBI_CONTROL
#elif defined(EFM32G280F32)
#define DVK_EBI_CONTROL
#elif defined(EFM32G280F64)
#define DVK_EBI_CONTROL
#elif defined(EFM32G290F128)
#define DVK_EBI_CONTROL
#elif defined(EFM32G290F32)
#define DVK_EBI_CONTROL
#elif defined(EFM32G290F64)
#define DVK_EBI_CONTROL
#elif defined(EFM32G840F128)
#define DVK_SPI_CONTROL
#elif defined(EFM32G840F32)
#define DVK_SPI_CONTROL
#elif defined(EFM32G840F64)
#define DVK_SPI_CONTROL
#elif defined(EFM32G880F128)
#define DVK_SPI_CONTROL
#elif defined(EFM32G880F32)
#define DVK_SPI_CONTROL
#elif defined(EFM32G880F64)
#define DVK_SPI_CONTROL
#elif defined(EFM32G890F128)
#define DVK_SPI_CONTROL
#elif defined(EFM32G890F32)
#define DVK_SPI_CONTROL
#elif defined(EFM32G890F64)
#define DVK_SPI_CONTROL
#else
#define DVK_SPI_CONTROL
#endif

#endif
#endif

/* EBI access */
void DVK_EBI_init(void);
void DVK_EBI_disable(void);

void DVK_EBI_writeRegister(volatile uint16_t *addr, uint16_t data);
uint16_t DVK_EBI_readRegister(volatile uint16_t *addr);

/* SPI access */
void DVK_SPI_init(void);
void DVK_SPI_disable(void);

void DVK_SPI_writeRegister(volatile uint16_t *addr, uint16_t data);
uint16_t DVK_SPI_readRegister(volatile uint16_t *addr);


/* Accodring to configuration, use either SPI or EBI */
#ifdef DVK_EBI_CONTROL
#define DVK_writeRegister(A, B)    DVK_EBI_writeRegister(A, B)
#define DVK_readRegister(A)        DVK_EBI_readRegister(A)
#endif

#ifdef DVK_SPI_CONTROL
#define DVK_writeRegister(A, B)    DVK_SPI_writeRegister(A, B)
#define DVK_readRegister(A)        DVK_SPI_readRegister(A)
#endif

/* General initialization routines */
void DVK_init(void);
void DVK_disable(void);

#endif
