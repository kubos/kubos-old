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

#include <stdlib.h>
#include <string.h>

#include "kubos-events/kubos-events.h"

void request_event(const char * event_key, event_callback cb, const uint8_t * buffer)
{
    /**
     * - Create and send event listen request message to ??
     *   - Request should have event_key and buffer
     * - Call add_event_listener with event_key and cb
     */
}

void start_event_loop(void)
{
    /**
     * - Spawn off event_loop_thread
     */
}

void event_loop_thread(void * param)
{
    while (1)
    {
        /**
         * - Wait for an event response
         * - Process event response
         *   - Look at event type
         *   - Find callback associated with event
         *   - Spawn one-shot thread to fire callback
         */
    }
}

void register_event(char * source_key, char * event_key)
{
    /**
     * - Creates and sends event registration message to event broker
         - Needs to send source_key and event_key pair
    */
}

bool fetch_event(char * source_key, event_req_t * event)
{
    
}

bool receive_event(event_pub_t * event)
{
    /**
     * - Read in data blob from event broker
     * - Attempt to decode blob into event_pub_t
     */
    return false;
}