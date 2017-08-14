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

#ifndef KUBOS_EVENTS_H
#define KUBOS_EVENTS_H

#include <stdint.h>
#include <stdbool.h>
#include "kubos-events/types.h"

/**
 * Maximum length of service/source/application keys
 */
#define MAX_NAME_LEN 256

/**
 * Unique string identifying current process
 * in the event system
 */
char MY_KEY[MAX_NAME_LEN];

/**
 * Internal function for handling event interest registration.
 * - Sends message to broker for event registration/subscription
 * - Registers internal mechanism for catching return message
 *
 * @param event_key character string key identifying event
 * @param cb function to be called when event triggers
 * @param buffer cbor encoded parameter for  event trigger
 */
bool register_handler(const char * event_key, event_callback cb);

/**
 * Adds event to internal listener/callback structure
 */
void add_event_listener(char * event_key, event_callback cb);

/**
 * Registers new source/key key pair with event message broker.
 */
void register_event(const char * event_key);

/**
 * Sends IPC message to event broker
 *
 * - Message has
 *   - source_key identifying source?
 *   - event_key identifying event
 *   - data event payload
 */
void fire_event(const char * event_key, const uint8_t * data);

/**
 * Blocking read of event from event broker
 *
 * - Reads in data blog via IPC from broker
 * - Attempts to deserialize event_pub_t from blob
 * - Verifies it is for me?
 */
bool receive_event(const char * app_key, event_pub_t * event);

void app_start_event_loop(void);

bool app_handle_event(event_pub_t event);

void init_events(const char * app_key);

void app_cleanup_events(void);

void app_add_event_listener(char * event_key, event_callback cb);

#endif