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
 * KubOS Event-Driven Architecture Example Project
 */

#include <stdio.h>
#include <eps-events/events.h>

void run_on_power_level(const uint8_t * buffer)
{
    printf("Low power detected!");
}

void run_on_power_reading(const uint8_t * buffer)
{
    uint8_t power_reading;
    
    // decode_power_reading(buffer, &power_reading);
    printf("Received power reading %d\n", power_reading);
}


int main(void)
{
    printf("Starting EPS Example\n");

    app_init_events("eps_application");

    on_power_level(30, &run_on_power_level);
    on_power_reading(run_on_power_reading);

    app_start_event_loop();

    printf("Exiting EPS Example\n");
    return 0;
}
