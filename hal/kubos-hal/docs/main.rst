KubOS HAL
=========

KubOS-HAL is the primary hardware abstraction layer (HAL) for KubOS. One
of our goals is to simplify development when it comes to interfacing
with your MCU(s). This module provides a unified set of functions for
interfacing with hardware which will work the same across all of our
supported MCUs.

API Documentation:
~~~~~~~~~~~~~~~~~~

-  @subpage GPIO
-  `I2C <docs/i2c.md>`__
-  @subpage SDIO
-  `SPI <docs/spi.md>`__
-  `UART <docs/uart.md>`__

Hardware Interfaces:
~~~~~~~~~~~~~~~~~~~~

-  `STM32F4 <@ref%20kubos-hal-stm32f4-main>`__
-  `MSP430F5529 <@ref%20kubos-hal-msp430f5529-main>`__
