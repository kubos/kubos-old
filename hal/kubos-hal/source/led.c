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

void k_led_init_all(void)
{
    for (int i = 0; i < K_LED_COUNT; i++) {
        k_led_init(K_LED_AT(i));
    }
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
