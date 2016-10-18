/**************************************************************************//**
 * @file
 * @brief DVK Peripheral Board Control API implementation
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

#include "efm32.h"
#include "dvk.h"
#include "dvk_boardcontrol.h"
#include "dvk_bcregisters.h"

/**************************************************************************//**
 * @brief Enable EFM32 access to periheral on DVK board
 * @param peri Peripheral to enable
 *****************************************************************************/
void DVK_enablePeripheral(DVKPeripheral peri)
{
  uint16_t bit;
  uint16_t tmp;

  /* Calculate which bit to set */
  bit = (uint16_t) peri;

  /* Read peripheral control register */
  tmp = DVK_readRegister(BC_PERCTRL);

  /* Enable peripheral */
  tmp |= bit;

  /* Special case for RS232, if enabled disable shutdown */
  if ((peri == DVK_RS232A) || (peri == DVK_RS232B))
  {
    /* clear shutdown bit */
    tmp &= ~(BC_PERCTRL_RS232_SHUTDOWN);
  }

  /* Special case for IRDA if enabled disable shutdown */
  if (peri == DVK_IRDA)
  {
    /* clear shutdown bit */
    tmp &= ~(BC_PERCTRL_IRDA_SHUTDOWN);
  }

  DVK_writeRegister(BC_PERCTRL, tmp);
}

/**************************************************************************//**
 * @brief Disable EFM32 access to peripheral on DVK board
 * @param peri Peripheral to disable
 *****************************************************************************/
void DVK_disablePeripheral(DVKPeripheral peri)
{
  uint16_t bit;
  uint16_t tmp;

  /* Calculate which bit to set */
  bit = (uint16_t) peri;

  /* Read peripheral control register */
  tmp = DVK_readRegister(BC_PERCTRL);

  /* Disable peripheral */
  tmp &= ~(bit);

  /* Special case for RS232, if enabled disable shutdown */
  if ((peri == DVK_RS232A) || (peri == DVK_RS232B))
  {
    /* Set shutdown bit */
    tmp |= (BC_PERCTRL_RS232_SHUTDOWN);
  }

  /* Special case for IRDA */
  if (peri == DVK_IRDA)
  {
    /* Set shutdown bit */
    tmp |= (BC_PERCTRL_IRDA_SHUTDOWN);
  }


  DVK_writeRegister(BC_PERCTRL, tmp);
}


/**************************************************************************//**
 * @brief Enable BUS access
 *****************************************************************************/
void DVK_enableBus(void)
{
  /* Enable bus access */
  DVK_writeRegister(BC_BUS_CFG, 1);
}


/**************************************************************************//**
 * @brief Disable BUS access
 *****************************************************************************/
void DVK_disableBus(void)
{
  DVK_writeRegister(BC_BUS_CFG, 0);
}


/**************************************************************************//**
 * @brief Inform AEM about current energy mode
 * @param energyMode What energy mode we are going to use next
 *****************************************************************************/
void DVK_setEnergyMode(uint16_t energyMode)
{
  DVK_writeRegister(BC_EM, energyMode);
}


/**************************************************************************//**
 * @brief Get status of bush buttons
 * @return Status of push buttons
 *****************************************************************************/
uint16_t DVK_getPushButtons(void)
{
  uint16_t tmp;

  tmp = (~(DVK_readRegister(BC_PUSHBUTTON))) & 0x000f;
  return tmp;
}

/**************************************************************************//**
 * @brief Get joystick button status
 * @return Joystick controller status
 *****************************************************************************/
uint16_t DVK_getJoystick(void)
{
  uint16_t tmp;

  tmp = (~(DVK_readRegister(BC_JOYSTICK))) & 0x001f;
  return tmp;
}

/**************************************************************************//**
 * @brief Get dipswitch status
 *        The DIP switches are free for user programmable purposes
 * @return Joystick controller status
 *****************************************************************************/
uint16_t DVK_getDipSwitch(void)
{
  uint16_t tmp;

  tmp = (~(DVK_readRegister(BC_DIPSWITCH))) & 0x00ff;
  return tmp;
}

/**************************************************************************//**
 * @brief Sets user leds
 * @param leds 16-bits which enables or disables the board "User leds"
 *****************************************************************************/
void DVK_setLEDs(uint16_t leds)
{
  DVK_writeRegister(BC_LED, leds);
}

/**************************************************************************//**
 * @brief Get status of user LEDs
 * @return Status of 16 user leds, bit 1 = on, bit 0 = off
 *****************************************************************************/
uint16_t DVK_getLEDs(void)
{
  return DVK_readRegister(BC_LED);
}

/**************************************************************************//**
 * @brief Enable "Control" buttons/joystick/dip switch interrupts
 * @param flags Board control interrupt flags, BC_INTEN_<something>
 *****************************************************************************/
void DVK_enableInterrupt(uint16_t flags)
{
  uint16_t tmp;

  /* Add flags to interrupt enable register */
  tmp  = DVK_readRegister(BC_INTEN);
  tmp |= flags;
  DVK_writeRegister(BC_INTEN, tmp);
}

/**************************************************************************//**
 * @brief Disable "Control" buttons/joystick/dip switch interrupts
 * @param flags Board control interrupt flags, BC_INTEN_<something>
 *****************************************************************************/
void DVK_disableInterrupt(uint16_t flags)
{
  uint16_t tmp;

  /* Clear flags from interrupt enable register */
  tmp   = DVK_readRegister(BC_INTEN);
  flags = ~(flags);
  tmp  &= flags;
  DVK_writeRegister(BC_INTEN, tmp);
}

/**************************************************************************//**
 * @brief Clear interrupts
 * @param flags Board control interrupt flags, BC_INTEN_<something>
 *****************************************************************************/
void DVK_clearInterruptFlags(uint16_t flags)
{
  DVK_writeRegister(BC_INTFLAG, flags);
}

/**************************************************************************//**
 * @brief Read interrupt flags
 * @return Returns currently triggered interrupts
 *****************************************************************************/
uint16_t DVK_getInterruptFlags(void)
{
  return DVK_readRegister(BC_INTFLAG);
}
