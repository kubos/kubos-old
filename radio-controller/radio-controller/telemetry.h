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
 * @defgroup Telemetry_Packet Telemetry packet structures and apis.
 * @addtogroup Telemetry_Packet
 * @{
 */
/**
 * @note This file is still WIP. Will address further in
 * a radio TX task.
 */

#pragma once

#include <stdint.h>

typedef struct
{
    /** Value hard coded to 11 */
    uint grouping_flags : 2;
    /** Sequence count from source APID */
    uint source_seq_count : 14;
} telemetry_sequence;

typedef struct
{
    uint      padding : 1;
    uint      version : 3;
    uint      padding2 : 4;
    uint      service_type : 8;
    uint      service_subtype : 8;
    long long time : 40;
} telemetry_data_header;

typedef struct
{
    telemetry_data_header header;
    uint8_t               data[6];
    uint16_t              error_control;
} telemetry_data;

typedef struct
{
    /** Packet ID structure */
    packet_id id;
    /** Telemetry packet sequence control */
    telemetry_sequence sequence;
    /** Number of octects contained in the data field */
    uint16_t data_length;
    /** Telemetry packet data field */
    telemetry_data data;
} telemetry_packet;

/* @} */
