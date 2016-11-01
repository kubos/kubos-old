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

#include <telemetry/service.h>
#include <telemetry-gather/gather.h>

#include <csp/csp.h>
#include <csp/arch/csp_thread.h>

#ifdef TARGET_LIKE_FREERTOS
#include "kubos-hal/uart.h"
#endif

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

    BEACON_THREAD;
    GATHER_THREAD;

    #ifdef TARGET_LIKE_FREERTOS
    vTaskStartScheduler();
    #endif

    /* Wait for program to terminate (ctrl + c) */
    while(1) {
    	csp_sleep_ms(1000000);
    }

    return 0;
}