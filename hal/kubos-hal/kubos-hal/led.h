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

#include "pins.h"
#include "kubos-hal/gpio.h"

#define K_LED_UNKNOWN -1

inline int kprv_led_at(int i) {
    if (i < 0 || i >= K_LED_COUNT) return K_LED_UNKNOWN;

#ifdef K_LED_0
    if (i == 0) return K_LED_0;
#endif
#ifdef K_LED_1
    if (i == 1) return K_LED_1;
#endif
#ifdef K_LED_2
    if (i == 2) return K_LED_2;
#endif
#ifdef K_LED_3
    if (i == 3) return K_LED_3;
#endif
}

#define K_LED_AT(i) kprv_led_at(i)

void k_led_init(int led_pin);
void k_led_init_all(void);
const char * k_led_get_desc(int led_pin);

#define K_LED_ON(pin) k_gpio_write(pin, 1)
#define K_LED_OFF(pin) k_gpio_write(pin, 0)
