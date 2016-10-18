/* mbed Microcontroller Library
 * Copyright (c) 2015 ARM Limited
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
#ifndef MBED_TARGET_CONFIG_H
#define MBED_TARGET_CONFIG_H

// Minar platform configuration

#define MINAR_PLATFORM_TIME_BASE  2000
#define MINAR_PLATFORM_MINIMUM_SLEEP 1

#define MODULE_SIZE_SPI         3
#define MODULE_SIZE_SERIAL      6

// Transaction queue size for each peripheral
#define TRANSACTION_QUEUE_SIZE_SPI   16

#endif
