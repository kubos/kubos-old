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

#include "events-broker/broker.h"
#include <kubos-events/kubos-events.h>
#include <stdio.h>

/**
 * Main for system events broker service.
 * - Dependent on some system-level IPC library TDB
 *
 * - Read in messages...
 *   - Event registration message
 *   - Event request message
 *   - Event publish message
 */
int main(void)
{
    printf("Starting events broker");
    
    while (1)
    {
        /**
         * Event Registration
         * Source -> Broker
         *
         * - Adds source_key and event_key to list of 
         *   available events
         * - Stores and associates connection with source_key
         */
        {
            event_reg_t reg;
            register_event(reg.source_key, reg.event_key);
        }

        /**
         * Event Request
         * Client -> Broker -> Source
         *
         * - Checks if event_key is available
         * - Locates source_key/connection from event_key
         * - Sends request to event source
         * - Stores app connection and associates with app_key
         */
        {
            event_req_t req;
            send_event_request(req.event_key, req.app_key, req.data);
        }

        /**
         * Event Publish
         * Source -> Broker -> Client
         *
         * - Locate connection with app_key
         * - Sends event_key and data back to app
         */
        {
            event_pub_t pub;
            send_event_data(pub.app_key, pub.event_key, pub.data);
        }
    }
}