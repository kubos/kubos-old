#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <uv.h>

#define PACKET_SIZE 256

typedef struct
{
    uint8_t  version : 3;
    uint8_t  type : 1;
    uint16_t app_id : 12;
    uint8_t  seq_flag : 2;
    uint16_t seq_count : 14;
    uint16_t data_len;
} packet_header;

typedef struct
{
    packet_header header;
    uint8_t       payload[PACKET_SIZE - sizeof(packet_header)];
} packet;

bool packet_parse(const uint8_t * buffer, packet * packet);

void packet_format(uint8_t * buffer, packet packet);

void packet_process(char * buffer, size_t len);
