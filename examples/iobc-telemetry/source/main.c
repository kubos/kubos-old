/*
 * KubOS Linux
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

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include "telemetry/telemetry.h"
#include "telemetry/types.h"


/**
 * Temp storage of list of publishers...
 */
#define TELEMETRY_PUBLISHERS_IOBC_UPTIME 100
#define TELEMETRY_PUBLISHERS_SUPERVISOR_UPTIME 101
#define TELEMETRY_PUBLISHERS_IOBC_RESET_COUNT 102
#define TELEMETRY_PUBLISHERS_IOBC_ADC_DATA_0 103
#define TELEMETRY_PUBLISHERS_IOBC_ADC_DATA_1 104
#define TELEMETRY_PUBLISHERS_IOBC_ADC_DATA_2 105
#define TELEMETRY_PUBLISHERS_IOBC_ADC_DATA_3 106
#define TELEMETRY_PUBLISHERS_IOBC_ADC_DATA_4 107
#define TELEMETRY_PUBLISHERS_IOBC_ADC_DATA_5 108
#define TELEMETRY_PUBLISHERS_IOBC_ADC_DATA_6 109
#define TELEMETRY_PUBLISHERS_IOBC_ADC_DATA_7 110
#define TELEMETRY_PUBLISHERS_IOBC_ADC_DATA_8 111
#define TELEMETRY_PUBLISHERS_IOBC_ADC_DATA_9 112

static uint32_t iobc_uptime = 0;
static uint32_t super_uptime = 0;
static uint32_t iobc_reset_count = 0;
static float adc_data[10];

void process_telemetry(telemetry_packet packet)
{
    switch (packet.source.topic_id)
    {
        case TELEMETRY_PUBLISHERS_IOBC_UPTIME:
            iobc_uptime = packet.data.i;
            break;
        case TELEMETRY_PUBLISHERS_SUPERVISOR_UPTIME:
            super_uptime = packet.data.i;
            break;
        case TELEMETRY_PUBLISHERS_IOBC_RESET_COUNT:
            iobc_reset_count = packet.data.i;
            break;
        case TELEMETRY_PUBLISHERS_IOBC_ADC_DATA_0:
            adc_data[0] = (packet.data.i * -0.2922) + 191.97;
            break;
        case TELEMETRY_PUBLISHERS_IOBC_ADC_DATA_1:
            adc_data[1] = (packet.data.i * 4.8875855);
            break;
        case TELEMETRY_PUBLISHERS_IOBC_ADC_DATA_2:
            adc_data[2] = (packet.data.i * 4.8875855);
            break;
        case TELEMETRY_PUBLISHERS_IOBC_ADC_DATA_3:
            adc_data[3] = (packet.data.i * 2.4437928);
            break;
        case TELEMETRY_PUBLISHERS_IOBC_ADC_DATA_4:
            adc_data[4] = (packet.data.i * 2.4437928);
            break;
        case TELEMETRY_PUBLISHERS_IOBC_ADC_DATA_5:
            adc_data[5] = (packet.data.i * 2.4437928);
            break;
        case TELEMETRY_PUBLISHERS_IOBC_ADC_DATA_6:
            adc_data[6] = (packet.data.i * 0.3470186);
            break;
        case TELEMETRY_PUBLISHERS_IOBC_ADC_DATA_7:
            adc_data[7] = (packet.data.i * 0.1221896);
            break;
        case TELEMETRY_PUBLISHERS_IOBC_ADC_DATA_8:
            adc_data[8] = (packet.data.i * 0.1637341);
            break;
        case TELEMETRY_PUBLISHERS_IOBC_ADC_DATA_9:
            adc_data[9] = (packet.data.i * 4.8875855);
            break;
    }
}

void terminate(int signum)
{
    printf("Caught term..time 2 go\r\n");
}

int main(void)
{

    socket_conn connection;
    telemetry_packet incoming_packet;
    
    bool subscribe_status = false;
    bool read_status = false;
    int i = 0;

    struct sigaction action = { 0 };
    action.sa_handler = terminate;
    sigaction(SIGTERM, &action, NULL);

    // printf("Connecting to telemetry system\n");
    if(!telemetry_connect(&connection))
    {
        printf("Couldn't connect!\n");
        return -1;
    }

    // printf("Subscribing to topics...\n");
    telemetry_subscribe(&connection, TELEMETRY_PUBLISHERS_IOBC_UPTIME);
    telemetry_subscribe(&connection, TELEMETRY_PUBLISHERS_SUPERVISOR_UPTIME);
    telemetry_subscribe(&connection, TELEMETRY_PUBLISHERS_IOBC_RESET_COUNT);
    telemetry_subscribe(&connection, TELEMETRY_PUBLISHERS_IOBC_ADC_DATA_0);
    telemetry_subscribe(&connection, TELEMETRY_PUBLISHERS_IOBC_ADC_DATA_1);
    telemetry_subscribe(&connection, TELEMETRY_PUBLISHERS_IOBC_ADC_DATA_2);
    telemetry_subscribe(&connection, TELEMETRY_PUBLISHERS_IOBC_ADC_DATA_3);
    telemetry_subscribe(&connection, TELEMETRY_PUBLISHERS_IOBC_ADC_DATA_4);
    telemetry_subscribe(&connection, TELEMETRY_PUBLISHERS_IOBC_ADC_DATA_5);
    telemetry_subscribe(&connection, TELEMETRY_PUBLISHERS_IOBC_ADC_DATA_6);
    telemetry_subscribe(&connection, TELEMETRY_PUBLISHERS_IOBC_ADC_DATA_7);
    telemetry_subscribe(&connection, TELEMETRY_PUBLISHERS_IOBC_ADC_DATA_8);
    telemetry_subscribe(&connection, TELEMETRY_PUBLISHERS_IOBC_ADC_DATA_9);


    int packet_count = 0;
    while (1)
    {
        // printf("Reading telemetry data\n");
        read_status = telemetry_read(&connection, &incoming_packet);
        process_telemetry(incoming_packet);
        
        // printf("Telemetry received: %d %d\n", incoming_packet.source.topic_id, incoming_packet.data.i);
        // sleep(1);
        if (packet_count++ % 12 == 0)
        {
            printf("%d,%d,%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n",
                iobc_uptime, super_uptime, iobc_reset_count,
                adc_data[0], adc_data[1], adc_data[2], adc_data[3], adc_data[4],
                adc_data[5], adc_data[6], adc_data[7], adc_data[8], adc_data[9]);
            break;
        }
    }

    telemetry_disconnect(&connection);

    return 0;
}
