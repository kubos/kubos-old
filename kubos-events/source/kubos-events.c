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

/**
 * Performs internal setup for events interface
 */
void init_events(const char * my_key)
{
    strncpy(MY_KEY, my_key, MAX_NAME_LEN);
}

bool register_handler(const char * event_key, event_callback cb)
{
    /**
     * - Create event listen request message
     *   - Request should have event_key and my_key
     * - Send request to event broker
     * - Call add_event_listener with event_key and cb
     */
     return false;
}

void register_event(const char * event_key)
{
    /**
     * - Creates event registration message with MY_KEY and event_key pair
     * - Send ipc message to event broker
    */
}

bool receive_event(const char * app_key, event_pub_t * event)
{
    /**
     * - Read in data blob from event broker
     * - Attempt to decode blob into event_pub_t
     */
    return false;
}


void fire_event(const char * event_key, const uint8_t * data)
{
    /**
     * - Package up ipc message
     * - Send ipc message to event broker
     */
}