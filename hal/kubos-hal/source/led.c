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

#include <stdlib.h>

#include "kubos-hal/led.h"

void k_led_init(int led_pin)
{
    k_gpio_init(led_pin, K_GPIO_OUTPUT, K_GPIO_PULL_NONE);
}

void k_led_init_all(void)
{
    for (int i = 0; i < K_LED_COUNT; i++) {
        k_led_init(k_led_at(i));
    }
}

void k_led_on(int pin)
{
    k_gpio_write(pin, 1);
}

void k_led_off(int pin)
{
    k_gpio_write(pin, 0);
}

int k_led_at(int i)
{
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

const char * k_led_get_desc(int led_pin)
{
#ifdef K_LED_RED
    if (led_pin == K_LED_RED) {
        return "red";
    }
#endif
#ifdef K_LED_GREEN
    if (led_pin == K_LED_GREEN) {
        return "green";
    }
#endif
#ifdef K_LED_BLUE
    if (led_pin == K_LED_BLUE) {
        return "blue";
    }
#endif
#ifdef K_LED_ORANGE
    if (led_pin == K_LED_ORANGE) {
        return "orange";
    }
#endif
    return NULL;
}
