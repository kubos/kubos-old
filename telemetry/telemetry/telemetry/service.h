#ifndef SERVICE_H
#define SERVICE_H

#include <stdint.h>
#include <csp/arch/csp_thread.h>

typedef struct
{
    uint8_t source_id;
    uint8_t in_beacon;
} telem_source;

typedef struct
{
    telem_source source;
    uint16_t timestamp;
    uint16_t data;
} telem_data;

static telem_data telem_beacon[YOTTA_CFG_TELEMETRY_BEACON_NUM_SOURCES];

void telemetry_submit(telem_data data);
void telemetry_log_data(telem_data);

CSP_DEFINE_TASK(beacon_thread);

#ifdef YOTTA_CFG_TELEMETRY
#define BEACON_THREAD   csp_thread_handle_t beacon_handle; \
                        csp_thread_create(beacon_thread, "BEACON", 1000, NULL, 0, &beacon_handle);
#else
#define BEACON_THREAD
#endif


#endif