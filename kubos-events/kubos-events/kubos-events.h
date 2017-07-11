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

/**
 * Maximum length of service/source/application keys
 */
#define MAX_NAME_LEN 256

char APP_KEY[MAX_NAME_LEN];

/**
 * Function pointer typedef for event callbacks
 *
 * @param buffer optional cbor encoded data
 */
typedef void (*event_callback)(const uint8_t * buffer);

/**
 * Structure used in event firing thread
 */
typedef struct
{
    /* Callback for event */
    event_callback cb;
    /* Data to send into callback */
    const uint8_t * data;
} event_resp_t;

/**
 * Structure sent to event source as part of
 * event request mechanism
 */
typedef struct
{
    const char * app_key;
    const char * event_key;
    const uint8_t * data;
} event_req_t;

/**
 * Structure sent to event broker as part of
 * event registration process
 */
typedef struct
{
    const char * source_key;
    const char * event_key;
} event_reg_t;

/**
 * Structure sent to event broker as part of
 * event publishing process
 */
typedef struct
{
    const char * app_key;
    const char * event_key;
    const uint8_t * data;
} event_pub_t;

/**
 * Internal function for handling event interest registration.
 * - Sends message to broker for event registration/subscription
 * - Registers internal mechanism for catching return message
 *
 * @param request_key character string key identifying requester
 * @param event_key character string key identifying event
 * @param cb function to be called when event triggers
 * @param buffer cbor encoded parameter for  event trigger
 */
void request_event(const char * event_key, event_callback cb, const uint8_t * buffer);

/**
 * Creates a thread to listen for new message responses
 */
void start_event_loop(void);

/**
 * Event response listening thread
 */
void event_loop_thread(void * param);

/**
 * One-shot event firing thread
 */
void event_fire_thread(void* param);

/**
 * Adds event to internal listener/callback structure
 */
void add_event_listener(char * event_key, event_callback cb);

/**
 * Registers new source/key key pair with event message broker.
 */
void register_event(char * source_key, char * event_key);

/**
 * Fetches an event relevant to the supplied source key
 * **Blocking Call**
 *
 * @param source_key key tied to source/group of events
 * @param event event request structure
 * @return bool true if event is found, false if there is a problem
 */
bool fetch_event(char * source_key, event_req_t * event);

bool receive_event(event_pub_t * event);

void app_start_event_loop(void);

void app_handle_event(event_pub_t event);

bool app_init_events(const char * app_key);

void app_request_event(const char * event_key, event_callback cb, const uint8_t * buffer);

#endif