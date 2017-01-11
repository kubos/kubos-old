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

#include "kubos-hal/led.h"
#include "kubos-hal/gpio.h"

void k_led_init(int led_pin)
{
    k_gpio_init(led_pin, K_GPIO_OUTPUT, K_GPIO_PULL_NONE);
}
