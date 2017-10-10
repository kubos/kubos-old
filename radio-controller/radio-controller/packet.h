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
 * @defgroup Packet_API High level packet related apis
 * @addtogroup Packet_API
 * @{
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <uv.h>

/**
 * Packet Identification Structure.
 * The same Packet ID struct is used by both Telecommand
 * and Telemetry packet types.
 *
 * This is effectively the first two octets of the
 * CCSCS SpacePacket Packet Primary Header.
 */
typedef struct
{
    /** Version Number. Set to 0 */
    uint version : 3;
    /** Type. Set to 1 */
    uint type : 1;
    /** Data Field Header Flag. Set to 1 */
    uint dfh : 1;
    /** Application Process ID */
    uint app_id : 11;
} packet_id;

/* @} */
