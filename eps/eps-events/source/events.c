/*
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

#include "eps-events/events.h"
#include <kubos-events/kubos-events.h>
#include <stddef.h>
#include <stdint.h>

/**
 * EPS Event - Triggered when power level hits set threshold
 *
 * - Performs registration for specified event + encoded data parameter
 * - Sends registration message to message broker
 *   - Spawns thread waiting on event response
 */
void on_power_level(uint32_t power_level, event_callback callback)
{
    uint8_t data_buffer[100];

    request_event(EVENT_EPS_ON_POWER_LEVEL, callback, data_buffer);
}

/**
 * EPS Event - Triggered on each power level reading
 *
 * - Performs registration for specified event
 * - Sends registration message to message broker
 *   - Spawns thread waiting on event response
 */
void on_power_reading(event_callback callback)
{
    request_event(EVENT_EPS_ON_POWER_LEVEL, callback, NULL);
}