#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <uv.h>

#define PACKET_SIZE 14

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
    uint version : 3;
    uint type : 1;
    uint data_field_header : 1;
    uint app_id : 11;
} packet_id;

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
} telecommand_data_header;

/**
 * Telecommand Packet Data Field Structure
 */
typedef struct
{
    /** Telecommand data field header - Fixed at 24 bits */
    union {
        telecommand_data_header header;
        uint32_t buf : 24;
    };
    uint8_t  payload[2];
    uint16_t error_control;
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


typedef struct
{
  /** Value hard coded to 11 */
  uint grouping_flags : 2;
  /** Sequence count from source APID */
  uint source_seq_count : 14;
} telemetry_sequence;

typedef struct
{
  uint padding : 1;
  uint version : 3;
  uint padding2 : 4;
  uint service_type : 8;
  uint service_subtype : 8;
  long long time : 40;
} telemetry_data_header;

typedef struct
{
  telemetry_data_header header;
  uint8_t data[2];
  uint16_t error_control;
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

bool packet_parse(const uint8_t * buffer, telecommand_packet * packet);

void packet_format(uint8_t * buffer, telecommand_packet packet);

void packet_process(char * buffer, size_t len);

void packet_debug(telecommand_packet p);
