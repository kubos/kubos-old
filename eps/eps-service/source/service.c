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
 * Implementation of an event driven EPS service
 *
 * - Serves up two events
 *   - ON_POWER_LEVEL - Triggered on supplied power level
 *   - ON_POWER_READING - Triggered on all power readings
 * - Maintains internal power level queue to separate reading
 *   the power level from servicing events
 */

#include "eps-service/service.h"
#include <eps-api/api.h>
#include <eps-events/events.h>
#include <kubos-events/kubos-events.h>
#include <string.h>

/**
 * Maximum number of event requests
 */
#define MAX_EVENT_REQ 50

static int service_running = true;

/**
 * Thread for producing power level readings
 *
 * - Read in power level from eps api
 * - Queue up power level reading for processing elsewhere
 * - Thread should exit on service exit
 */
void power_level_read_thread(void)
{
    uint32_t power_level;
    uint8_t data_buffer[256];
    while (1)
    {
        /**
         * eps_get_power_level blocks until new reading is ready??
         */
        eps_get_power_level(&power_level);
        // serialize_int(power_level, data_buffer);
        fire_event(EVENT_EPS_ON_POWER_READING, data_buffer);
    }
}


/**
 * Main for event driven/producing service
 *
 * - Register all exposed events with event broker
 * - Start reading/processing threads
 * - Fetch new event registration from event broker
 * - For each event registration
 *   - Check if event key matches known events
 *   - Register interest in matched event
 *
 * - TODO - handle immediate events
 */
int main(void)
{
    init_events(EVENT_EPS);

    register_event(EVENT_EPS_ON_POWER_READING);

    /**
     * We are no longer processing event handler requests
     * but we probably still want the ability to consume
     * messages/requests and react to them. 
     
     * We also need a main loop to continue to run while
     * background data gathering is happening.
     */

    while (service_running)
    {
        // fetch_message() ??
        sleep(1);
    }
}