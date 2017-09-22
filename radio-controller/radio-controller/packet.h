#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <uv.h>

#define PACKET_SIZE 14

typedef struct
{
    uint version : 3;
    uint type : 1;
    uint data_field_header : 1;
    uint app_id : 11;
} packet_id;

typedef struct
{
    uint flags : 2;
    uint count : 14;
} packet_sequence;

/**
 * Packet Overhead is data contained in the packet
 * other than the actual payload.
 * In this case we have...
 * - packet_id
 * - packet_sequence
 * - three bytes for data field header
 * - two bytes for data field error control
 */
#define PACKET_OVERHEAD sizeof(packet_id) + sizeof(packet_sequence) + 3 + 2

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
} data_field_header;

typedef struct
{
    /** Telecommand data field header */
    data_field_header header;
    uint8_t  payload[2];
    uint16_t error_control;
} packet_data;

typedef struct
{
    /** Packet ID structure */
    packet_id id;
    /** Packet sequence control structure */
    packet_sequence sequence;
    /** Number of octets contained in the data field */
    uint16_t data_length;
    /** Packet data field structure */
    packet_data data;
} packet;

bool packet_parse(const uint8_t * buffer, packet * packet);

void packet_format(uint8_t * buffer, packet packet);

void packet_process(char * buffer, size_t len);

void packet_debug(packet p);
