/*
 * KubOS RT
 * Copyright (C) 2016 Kubos Corporation
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
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

#include <telemetry/telemetry.h>
#include <telemetry/sources.h>
#include <telemetry-gather/gather.h>
#include <telemetry-beacon/beacon.h>
#include <telemetry-health/health.h>

#include <csp/csp.h>
#include <csp/arch/csp_thread.h>
#include <csp/arch/csp_time.h>

#ifdef TARGET_LIKE_FREERTOS
#include "kubos-hal/uart.h"
#endif

/**
 * Example "user created" thread gathering extra data
 * and sending it to telemetry
 */
CSP_DEFINE_TASK(pos_y_thread)
{
    static int pos_data = 2;
    while(1)
    {
        telem_data data;
        data.data = pos_data--;
        data.source = pos_y_source;
        data.timestamp = csp_get_ms();

        telemetry_submit(data);
        csp_sleep_ms(300);
    }
}


int main(void)
{
    #ifdef TARGET_LIKE_FREERTOS
    k_uart_console_init();
    #endif

    #ifdef TARGET_LIKE_MSP430
    /* Stop the watchdog. */
    WDTCTL = WDTPW + WDTHOLD;

    __enable_interrupt();
    #endif

    printf("Initialising CSP\r\n");
    csp_buffer_init(5, 20);

    /* Init CSP with address MY_ADDRESS */
    csp_init(1);

    /* Start router task with 500 word stack, OS task priority 1 */
    csp_route_start_task(50, 1);

    BEACON_THREADS;
    GATHER_THREADS;
    HEALTH_THREADS;

    csp_thread_handle_t pos_y_handle;
    csp_thread_create(pos_y_thread, "POS_Y", 50, NULL, 0, &pos_y_handle);

    #ifdef TARGET_LIKE_FREERTOS
    vTaskStartScheduler();
    #endif

    /* Wait for program to terminate (ctrl + c) */
    while(1) {
        csp_sleep_ms(1000000);
    }

    return 0;
}