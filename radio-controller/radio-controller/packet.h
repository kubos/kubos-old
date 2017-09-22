#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <uv.h>

#define PACKET_SIZE 256

typedef struct {
  uint8_t version : 3;
  uint8_t type : 1;
  uint16_t app_id : 12;
  uint8_t seq_flag : 2;
  uint16_t seq_count : 14;
  uint16_t data_len;
} spacepacket_header;

typedef struct {
  spacepacket_header header;
  char payload[PACKET_SIZE - sizeof(spacepacket_header)];
} spacepacket;

bool spacepacket_parse(const uint8_t * buffer, spacepacket * packet);

void spacepacket_format(uint8_t * buffer, spacepacket packet);

void packet_process(char * buffer, size_t len );
