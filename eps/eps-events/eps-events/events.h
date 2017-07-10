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

#ifndef EPS_EVENTS_H
#define EPS_EVENTS_H

#include <kubos-events/kubos-events.h>
#include <stdint.h>

/**
 * Source key for identifying all EPS related events
 */
#define EVENT_EPS "EVENT_EPS"

/**
 * Event key for events tied to a specific power level reading
 */
#define EVENT_EPS_ON_POWER_LEVEL "EVENT_EPS_ON_POWER_LEVEL"

/**
 * Event key for events tied to all power level readings
 */
#define EVENT_EPS_ON_POWER_READING  "EVENT_EPS_ON_POWER_READING"

/**
 * EPS Event - Triggered when power level hits set threshold
 */
void onPowerLevel(uint32_t power_level, event_callback callback);

/**
 * EPS Event - Triggered on each power level reading
 */
void onPowerReading(event_callback callback);

#endif