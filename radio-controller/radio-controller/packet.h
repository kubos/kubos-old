#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <uv.h>

#define PACKET_SIZE 18

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
