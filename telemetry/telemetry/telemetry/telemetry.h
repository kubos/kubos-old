#ifndef SERVICE_H
#define SERVICE_H

#include <stdint.h>
#include <csp/arch/csp_thread.h>

typedef struct
{
    uint8_t source_id;
    uint8_t dest_flag;
} telem_source;


typedef struct
{
    telem_source source;
    uint16_t timestamp;
    uint16_t data;
} telem_data;

void telemetry_submit(telem_data data);
void telemetry_log_data(telem_data);

#define TELEMETRY_BEACON_PORT 10
#define TELEMETRY_HEALTH_PORT 11

#define TELEMETRY_BEACON_FLAG 0x1
#define TELEMETRY_HEALTH_FLAG 0x10

#endif