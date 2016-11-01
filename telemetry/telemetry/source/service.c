#ifdef YOTTA_CFG_TELEMETRY

#include "telemetry/service.h"

void telemetry_update_beacon(telem_data data)
{
    if (data.source.in_beacon)
    {
        telem_beacon[data.source.source_id] = data;
    }
}

void telemetry_submit(telem_data data)
{
    telemetry_update_beacon(data);
    telemetry_log_data(data);
}

void telemetry_log_data(telem_data data)
{
    printf("TELEM:%d:%d:%d\r\n", data.source.source_id, data.timestamp, data.data);
}


CSP_DEFINE_TASK(beacon_thread)
{
    telem_data data;
    int i;
    while(1)
    {
        csp_sleep_ms(YOTTA_CFG_TELEMETRY_BEACON_INTERVAL);
        for (i = 0; i < YOTTA_CFG_TELEMETRY_BEACON_NUM_SOURCES; i++)
        {
            data = telem_beacon[i];
            printf("BEACON:%d:%d:%d\r\n", data.source.source_id, data.timestamp, data.data);
        }        
    }
}

#endif