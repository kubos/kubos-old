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

#ifdef TARGET_LIKE_ISIS

#include <kubos-hal-iobc/supervisor.h>
#include <telemetry/telemetry.h>
#include <csp/arch/csp_time.h>

#include "publishers.h"

#define SUPERVISOR_TELEMETRY_INTERVAL 10

CSP_DEFINE_TASK(supervisor_publisher)
{
    while (1)
    {
        supervisor_housekeeping_t info;
        if (supervisor_get_housekeeping(&info))
        {
            telemetry_publish((telemetry_packet) {
                .data = info.fields.iobc_uptime,
                .timestamp = csp_get_ms(),
                .source = TELEMETRY_PUBLISHERS_IOBC_UPTIME
            });

            telemetry_publish((telemetry_packet) {
                .data = info.fields.supervisor_uptime,
                .timestamp = csp_get_ms(),
                .source = TELEMETRY_PUBLISHERS_SUPERVISOR_UPTIME
            });

            telemetry_publish((telemetry_packet) {
                .data = info.fields.iobc_reset_count,
                .timestamp = csp_get_ms(),
                .source = TELEMETRY_PUBLISHERS_IOBC_RESET_COUNT
            });

            telemetry_publish((telemetry_packet) {
                .data = info.fields.adc_data[0],
                .timestamp = csp_get_ms(),
                .source = TELEMETRY_PUBLISHERS_IOBC_ADC_DATA_0
            });

            telemetry_publish((telemetry_packet) {
                .data = info.fields.adc_data[1],
                .timestamp = csp_get_ms(),
                .source = TELEMETRY_PUBLISHERS_IOBC_ADC_DATA_1
            });

            telemetry_publish((telemetry_packet) {
                .data = info.fields.adc_data[2],
                .timestamp = csp_get_ms(),
                .source = TELEMETRY_PUBLISHERS_IOBC_ADC_DATA_2
            });

            telemetry_publish((telemetry_packet) {
                .data = info.fields.adc_data[3],
                .timestamp = csp_get_ms(),
                .source = TELEMETRY_PUBLISHERS_IOBC_ADC_DATA_3
            });

            telemetry_publish((telemetry_packet) {
                .data = info.fields.adc_data[4],
                .timestamp = csp_get_ms(),
                .source = TELEMETRY_PUBLISHERS_IOBC_ADC_DATA_4
            });

            telemetry_publish((telemetry_packet) {
                .data = info.fields.adc_data[5],
                .timestamp = csp_get_ms(),
                .source = TELEMETRY_PUBLISHERS_IOBC_ADC_DATA_5
            });

            telemetry_publish((telemetry_packet) {
                .data = info.fields.adc_data[6],
                .timestamp = csp_get_ms(),
                .source = TELEMETRY_PUBLISHERS_IOBC_ADC_DATA_6
            });

            telemetry_publish((telemetry_packet) {
                .data = info.fields.adc_data[7],
                .timestamp = csp_get_ms(),
                .source = TELEMETRY_PUBLISHERS_IOBC_ADC_DATA_7
            });

            telemetry_publish((telemetry_packet) {
                .data = info.fields.adc_data[8],
                .timestamp = csp_get_ms(),
                .source = TELEMETRY_PUBLISHERS_IOBC_ADC_DATA_8
            });

            telemetry_publish((telemetry_packet) {
                .data = info.fields.adc_data[9],
                .timestamp = csp_get_ms(),
                .source = TELEMETRY_PUBLISHERS_IOBC_ADC_DATA_9
            });
        }
        sleep(SUPERVISOR_TELEMETRY_INTERVAL);
    }
}

#endif