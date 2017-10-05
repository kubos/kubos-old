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
 * @defgroup Telecommand_Packet Structures and APIs for Telecommand Packet
 * @addtogroup Telecommand_Packet
 * @{
 */

#pragma once

#include "radio-controller/packet.h"

/**
 * Telecommand Packet Sequence Control Structure
 */
typedef struct
{
    uint flags : 2;
    uint count : 14;
} telecommand_sequence;

/**
 * Packet Overhead is data contained in the packet
 * other than the actual payload.
 * In this case we have...
 * - packet_id
 * - packet_sequence
 * - three bytes for data field header
 * - two bytes for data field error control
 */
#define PACKET_OVERHEAD \
    sizeof(packet_id) + sizeof(telecommand_sequence) + 3 + 2

/**
 * Structure modeling the Telecommand Data Field Header
 * Documented in Section 2.1.2 of Swiss Space Center document
 * "Satellite Control Software TM & TC Packets Definition Recommendation"
 */
typedef struct
{
    /** CCSDS Secondary Header Flag - Set to 0 */
    uint ccsds_secondary_header : 1;
    /** TP Packet PUS Version Number - Set to 1 */
    uint tc_packet_version : 3;
    /**
     * ACK field -
     * ---1 acceptance of packet by app
     * --1- start of execution
     * -1-- progress of execution
     * 1--- completion of execution
     */
    uint ack : 4;
    /** Indicates the service this packet relates to */
    uint service_type : 8;
    /** Used with the service_type to indicate type of service request */
    uint service_subtype : 8;
} telecommand_data_header;

/**
 * Telecommand Packet Data Field Structure
 */
typedef struct
{
    /** Telecommand data field header - Fixed at 24 bits */
    telecommand_data_header header;
    uint8_t *               payload;
    uint16_t                error_control;
} telecommand_data;

typedef struct
{
    /** Packet ID structure */
    packet_id id;
    /** Packet sequence control structure */
    telecommand_sequence sequence;
    /** Number of octets contained in the data field */
    uint16_t data_length;
    /** Packet data field structure */
    telecommand_data data;
} telecommand_packet;

/**
 * Parser for telecommand packets. Data blob -> telecommand_packet.
 * @param[in] buffer Data to parse into packet
 * @param[out] packet Telecommand structure to fill with data
 * @return bool true if successful, otherwise false
 */
bool telecommand_parse(const uint8_t * buffer, telecommand_packet * packet);

/**
 * Examines telecommand packet payload and
 * sends off appropriate message to system.
 * @param[in] p packet to examine
 */
void telecommand_run(telecommand_packet p);

/**
 * Parses telecommand_packet out of buffer and hands
 * off packet payload to appropriate destination.
 * @param[in] buffer Data to parse packet from
 * @param[in] len Length of buffer
 */
void telecommand_process(const char * buffer, size_t len);

/**
 * Debug display of telecommand_packet.
 * Prints out current values of all members.
 * @param[in] p Packet to display
 */
void telecommand_debug(telecommand_packet p);

/* @} */
