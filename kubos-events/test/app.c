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

#include <cmocka.h>
#include <kubos-events/kubos-events.h>

#define APP_KEY "TEST_APP"
#define EVENT_KEY "TEST_EVENT"

static int test_int = 0;

void callback(const uint8_t * buffer)
{
    if (buffer != NULL)
    {
        test_int = *(int*)buffer;
    }
}

static void test_event_response_no_request(void ** arg)
{
    event_pub_t event_pub = {
        .app_key = APP_KEY,
        .event_key = EVENT_KEY,
        .data = NULL
    };

    assert_false(app_handle_event(event_pub));
}

static void test_event_request_response(void ** arg)
{
    event_pub_t event_pub = {
        .app_key = APP_KEY,
        .event_key = EVENT_KEY,
        .data = NULL
    };

    event_callback cb = callback;

    app_add_event_listener(EVENT_KEY, cb);

    assert_true(app_handle_event(event_pub));

    app_cleanup_events();
}

static void test_event_request_response_callback(void ** arg)
{
    int new_int = 5;
    event_pub_t event_pub = {
        .app_key = APP_KEY,
        .event_key = EVENT_KEY,
        .data = (const uint8_t*)&new_int
    };

    event_callback cb = callback;

    app_add_event_listener(EVENT_KEY, cb);

    app_handle_event(event_pub);

    assert_int_equal(test_int, 5);

    app_cleanup_events();
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_event_response_no_request),
        cmocka_unit_test(test_event_request_response),
        cmocka_unit_test(test_event_request_response_callback),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}