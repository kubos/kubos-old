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

#include "kubos-events/kubos-events.h"

/**
 * Performs internal setup for event service
 */
void init_event_service(const char * service_key)
{
    strncpy(SERVICE_KEY, service_key, MAX_NAME_LEN);
}

/**
 * Sends message to event broker with SERVICE_KEY and event_key
 */
void service_register_event(const char * event_key)
{
    register_event(SERVICE_KEY, event_key);
}

/**
 * Waits on next event request from service broker
 */
void service_fetch_event(event_req_t * event)
{
    fetch_event(SERVICE_KEY, event);
}

/**
 * Places event request into internal queue for event
 * handlers to access
 */
void service_handle_event_request(event_req_t event)
{

}
