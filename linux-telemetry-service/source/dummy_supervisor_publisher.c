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

#ifdef TARGET_LIKE_X86

#include <telemetry/telemetry.h>
#include <csp/arch/csp_time.h>

#include "publishers.h"

#define SUPERVISOR_TELEMETRY_INTERVAL 10

/** Length of the telemetry housekeeping request. */
#define LENGTH_TELEMETRY_HOUSEKEEPING 37

typedef union __attribute__((__packed__)) {
    /** Raw value of the version configuration bytes. */
    uint8_t raw_value;
    /** Individual enable status fields */
    struct __attribute__((__packed__)) supervisor_enable_status_fields_t
    {
                
        uint8_t
                /** OBC Power. */
                power_obc : 1, 
                /** Output power to the RTC. */
                power_rtc : 1,
                /** Supervisor mode. */
                is_in_supervisor_mode : 1, 
                : 1, : 1,
                /** RTC is busy. */
                busy_rtc : 1,
                /** RTC is doing power off. */
                power_off_rtc : 1,
                : 1;
    } /** Individual enable status fields */ fields;
} supervisor_enable_status_t;

/** The number of channels used in the Supervisor Controller. */
#define SUPERVISOR_NUMBER_OF_ADC_CHANNELS 10

/**
 * Supervisor housekeeping.
 */
typedef union __attribute__((__packed__))  {
    /** Raw value of the version configuration bytes */
    uint8_t raw_value[LENGTH_TELEMETRY_HOUSEKEEPING];
    /** Individual housekeeping fields */
    struct __attribute__((__packed__)) supervisor_housekeeping_fields_t
    {
        /** The first as always is a dummy byte. */
        uint8_t dummy;
        /** The second is a SPI Command Status. */
        uint8_t spi_command_status;
        /** Enable status of the Supervisor Controller. */
        supervisor_enable_status_t enable_status;
        /** Supervisor Controller Uptime. */
        uint32_t supervisor_uptime;
        /** IOBC Uptime as measured by Supervisor Controller. */
        uint32_t iobc_uptime;
        /** IOBC Reset Count. */
        uint32_t iobc_reset_count; 
        /** ADC Data. */
        uint16_t adc_data[SUPERVISOR_NUMBER_OF_ADC_CHANNELS];
        /** ADC Update Flag. */
        uint8_t adc_update_flag;
        /** CRC byte. */
        uint8_t crc8;
    } /** Individual housekeeping fields */ fields;
} supervisor_housekeeping_t;


CSP_DEFINE_TASK(supervisor_publisher)
{
    while (1)
    {
        supervisor_housekeeping_t info = {
            .fields.supervisor_uptime = 100,
            .fields.iobc_uptime = csp_get_ms(),
            .fields.iobc_reset_count = 0,
            .fields.adc_data[0] = 0,
            .fields.adc_data[1] = 1,
            .fields.adc_data[2] = 2,
            .fields.adc_data[3] = 3
        };
        {
            telemetry_publish((telemetry_packet) {
                .data = info.fields.iobc_uptime,
                .timestamp = csp_get_ms(),
                .source = TELEMETRY_PUBLISHERS_IOBC_UPTIME
            });

            // telemetry_publish((telemetry_packet) {
            //     .data = info.fields.supervisor_uptime,
            //     .timestamp = csp_get_ms(),
            //     .source = TELEMETRY_PUBLISHERS_SUPERVISOR_UPTIME
            // });

            // telemetry_publish((telemetry_packet) {
            //     .data = info.fields.iobc_reset_count,
            //     .timestamp = csp_get_ms(),
            //     .source = TELEMETRY_PUBLISHERS_IOBC_RESET_COUNT
            // });

            // telemetry_publish((telemetry_packet) {
            //     .data = info.fields.adc_data[0],
            //     .timestamp = csp_get_ms(),
            //     .source = TELEMETRY_PUBLISHERS_IOBC_ADC_DATA_0
            // });

            // telemetry_publish((telemetry_packet) {
            //     .data = info.fields.adc_data[1],
            //     .timestamp = csp_get_ms(),
            //     .source = TELEMETRY_PUBLISHERS_IOBC_ADC_DATA_1
            // });

            // telemetry_publish((telemetry_packet) {
            //     .data = info.fields.adc_data[2],
            //     .timestamp = csp_get_ms(),
            //     .source = TELEMETRY_PUBLISHERS_IOBC_ADC_DATA_2
            // });

            // telemetry_publish((telemetry_packet) {
            //     .data = info.fields.adc_data[3],
            //     .timestamp = csp_get_ms(),
            //     .source = TELEMETRY_PUBLISHERS_IOBC_ADC_DATA_3
            // });

            // telemetry_publish((telemetry_packet) {
            //     .data = info.fields.adc_data[4],
            //     .timestamp = csp_get_ms(),
            //     .source = TELEMETRY_PUBLISHERS_IOBC_ADC_DATA_4
            // });

            // telemetry_publish((telemetry_packet) {
            //     .data = info.fields.adc_data[5],
            //     .timestamp = csp_get_ms(),
            //     .source = TELEMETRY_PUBLISHERS_IOBC_ADC_DATA_5
            // });

            // telemetry_publish((telemetry_packet) {
            //     .data = info.fields.adc_data[6],
            //     .timestamp = csp_get_ms(),
            //     .source = TELEMETRY_PUBLISHERS_IOBC_ADC_DATA_6
            // });

            // telemetry_publish((telemetry_packet) {
            //     .data = info.fields.adc_data[7],
            //     .timestamp = csp_get_ms(),
            //     .source = TELEMETRY_PUBLISHERS_IOBC_ADC_DATA_7
            // });

            // telemetry_publish((telemetry_packet) {
            //     .data = info.fields.adc_data[8],
            //     .timestamp = csp_get_ms(),
            //     .source = TELEMETRY_PUBLISHERS_IOBC_ADC_DATA_8
            // });

            // telemetry_publish((telemetry_packet) {
            //     .data = info.fields.adc_data[9],
            //     .timestamp = csp_get_ms(),
            //     .source = TELEMETRY_PUBLISHERS_IOBC_ADC_DATA_9
            // });
        }
        sleep(SUPERVISOR_TELEMETRY_INTERVAL);
    }
}

#endif