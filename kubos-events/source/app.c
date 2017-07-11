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

#include <stdbool.h>
#include <string.h>
#include "kubos-events/kubos-events.h"
#include <kubos-core/utlist.h>

typedef struct event_cb_pair_t
{
    char event_key[MAX_NAME_LEN];
    event_callback cb;
    struct event_cb_pair_t * next;
} event_cb_pair_t;

static event_cb_pair_t * event_cb_list = NULL;

static bool app_running = true;

/**
 * Performs internal setup
 *
 * - Sets internal APP_KEY
 */
bool app_init_events(const char * app_key)
{
    strncpy(APP_KEY, app_key, MAX_NAME_LEN);
}

/**
 * Main event handling loop for applications.
 * This is intended to be run in main() after events are requested
 * and handle the rest of the application lifecycle.
 *
 * - Listen for event messages from the event broker
 *   - When event message received
 *     - Check for existing callback on event
 *     - Spawn one-shot thread for callback
 */
void app_start_event_loop(void)
{
    event_pub_t event;
    printf("Starting %s event loop\n", APP_KEY);
    while (app_running)
    {
        if (receive_event(&event))
        {
            app_handle_event(event);
        }
        else
        {
            app_running = false;
        }
    }
}

/**
 * This function does the actual handling of received event messages
 *
 * - Check for callback/handler for event
 * - Spawn one-shot thread for callback
 */
void app_handle_event(event_pub_t event)
{
    event_callback cb;
    event_resp_t event_resp;
    
    // Look up cb for event
    // event_resp.cb = get_event_callback(event);
    // memcpy(event_resp.data, event.data, sizeof(event.data));
    // start_thread(app_fire_event_thread, &event_resp);
}

/**
 * This thread handles calling the event callback without
 * blocking the event response receiving thread
 */
void app_fire_event_thread(void * param)
{
    event_resp_t resp;
    if (param != NULL)
    {
        memcpy(&resp, param, sizeof(event_resp_t));
        resp.cb(resp.data);
    }
}


/**
 * This function adds the event_key and callback to an internal
 * list for usage by the event response listener
 *
 * - Adds callback to internal list of event/cb
 */
void app_add_event_listener(char * event_key, event_callback cb)
{
    event_cb_pair_t new_event_cb;
    memcpy(new_event_cb.event_key, event_key, MAX_NAME_LEN);
    memcpy(new_event_cb.cb, cb, sizeof(event_callback));

    LL_APPEND(event_cb_list, &new_event_cb);
}