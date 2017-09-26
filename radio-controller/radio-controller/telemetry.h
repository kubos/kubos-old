
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

