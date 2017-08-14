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
 * Defines structures and types needed for the event interface
 */

#ifndef EVENTS_TYPES_H
#define EVENTS_TYPES_H

#include <stdint.h>

/**
 * Function pointer typedef for event callbacks
 *
 * @param buffer optional cbor encoded data
 */
/**
 * Should we consider creating a custom callback type for each exposed
 * event? If we did this then we could save the user the work of
 * decoding the information
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
    uint8_t * data;
} event_resp_t;

/**
 * Structure sent to event source as part of
 * event request mechanism
 */
typedef struct
{
    const char *    app_key;
    const char *    event_key;
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
    const char *    app_key;
    const char *    event_key;
    const uint8_t * data;
} event_pub_t;

#endif