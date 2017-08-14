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
/**
 * Implementation for each event handler registration function.
 *
 * - Each function should
 *  - Call register_handler to register with event broker
 */

#include "eps-events/events.h"
#include <kubos-events/kubos-events.h>
#include <stddef.h>
#include <stdint.h>

/**
 * EPS Event - Triggered on each power level reading
 *
 * - Performs registration for specified event
 * - Sends registration message to message broker
 *   - Spawns thread waiting on event response
 */
void on_power_reading(event_callback callback)
{
    register_handler(EVENT_EPS_ON_POWER_READING, callback);
}