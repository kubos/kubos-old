/*
 * KubOS HAL
 * Copyright (C) 2017 Kubos Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/**
 * @defgroup LED
 * @addtogroup LED
 * @{
 */
/**
 * @brief KubOS-HAL LED Interface
 * @author kubos.co
 */

#ifndef K_LED_H
#define K_LED_H

#include "pins.h"
#include "kubos-hal/gpio.h"

#define K_LED_UNKNOWN -1

/**
 * Setup and initialize an on-board LEDs
 *
 * @param led_pin The pin number of the LED (usually K_LED_0 - K_LED_<n>)
 */
void k_led_init(int led_pin);

/**
 * Setup and initialize all on-board LEDs
 */
void k_led_init_all(void);

/**
 * Turn an LED on
 *
 * @param led_pin The pin number of the LED (usually K_LED_0 - K_LED_<n>)
 */
void k_led_on(int led_pin);

/**
 * Turn an LED off
 *
 * @param led_pin The pin number of the LED (usually K_LED_0 - K_LED_<n>)
 */
void k_led_off(int led_pin);

/**
 * Get the LED pin number for a specific LED index (useful for iterating)
 * @param index a value in the range [0, K_LED_COUNT-1].
 * @return The pin number of the LED, or K_LED_UNKNOWN (-1) if the index is
 *         outside the valid range.
 */
int k_led_at(int index);

/**
 * Get a description of an LED
 *
 * @param led_pin The pin number of the LED (usually K_LED_0 - K_LED_<n>)
 * @return A string description of the LED i.e. "red", or NULL if the passed in
 *         led_pin is unknown.
 */
const char * k_led_get_desc(int led_pin);

#endif // K_LED_H
