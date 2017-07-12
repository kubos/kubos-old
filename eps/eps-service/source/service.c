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

#include "eps-service/service.h"
#include <eps-api/api.h>
#include <eps-events/events.h>
#include <kubos-events/kubos-events.h>
#include <string.h>

/**
 * Queue power level reading onto internal queue
 */
void queue_power_level(uint32_t power_level)
{
}

/**
 * Dequeue power level reading from internal queue
 */
void dequeue_power_level(uint32_t * power_level)
{
}

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
    while (1)
    {
        /**
         * eps_get_power_level blocks until new reading is ready??
         */
        eps_get_power_level(&power_level);
        queue_power_level(power_level);
    }
}

/**
 * Thread for processing power level readings and associated events
 *
 * - Take power level reading from queue
 * - For each relevant event type
 *   - Determine if interest has been registered
 *   - Determine if parameters are met
 *   - Send back message if conditions are good
 * - Thread should exit on service exit
 */
void power_level_process_thread(void)
{
    uint32_t power_level;
    while (1)
    {       
        dequeue_power_level(&power_level);
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
    init_event_service(EVENT_EPS);

    service_register_event(EVENT_EPS_ON_POWER_LEVEL);
    service_register_event(EVENT_EPS_ON_POWER_READING);

    event_req_t event;

    while (service_fetch_event(&event))
    {
        if (!strcmp(event.event_key, EVENT_EPS_ON_POWER_LEVEL))
        {
            /**
             * - Add new trigger for eps power level
             *   - Needs to carry over event parameter
             */
        }
        if (!strcmp(event.event_key, EVENT_EPS_ON_POWER_READING))
        {
            /**
             * - Add new trigger for eps power reading
             */
        }
    }
}