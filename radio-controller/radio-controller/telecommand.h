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
    uint8_t                 payload[7];
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


bool telecommand_parse(const uint8_t * buffer, telecommand_packet * packet);

void telecommand_format(uint8_t * buffer, telecommand_packet packet);

void telecommand_process(char * buffer, size_t len);

void telecommand_debug(telecommand_packet p);
