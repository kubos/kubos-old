#ifdef YOTTA_CFG_TELEMETRY

#include <telemetry/telemetry.h>
#include "telemetry-health/health.h"
#include <csp/csp.h>

void update(telem_data data)
{
    if (data.source.dest_flag & TELEMETRY_HEALTH_FLAG)
    {
        // telem_beacon[data.source.beacon_id] = data;
        health_data[data.source.source_id % TELEMETRY_NUM_HEALTH] = data;
        // printf("HEALTH:%d:%d:%d\r\n", data.source.source_id, data.timestamp, data.data);
    }
}

CSP_DEFINE_TASK(health_rx_thread)
{
    int running = 1;
    csp_socket_t *socket = csp_socket(CSP_SO_NONE);
    csp_conn_t *conn;
    csp_packet_t *packet;

    csp_bind(socket, TELEMETRY_BEACON_PORT);
    csp_listen(socket, 5);
    telem_data data;

    while(running) {
        if( (conn = csp_accept(socket, 10000)) == NULL ) {
            continue;
        }

        while( (packet = csp_read(conn, 100)) != NULL ) {
            switch( csp_conn_dport(conn) ) {
                case TELEMETRY_BEACON_PORT:
                    data = *((telem_data*)packet->data);
                    csp_buffer_free(packet);
                    update(data);
                    break;
                default:
                    csp_service_handler(conn, packet);
                    break;
            }
        }

        csp_close(conn);
    }
}

CSP_DEFINE_TASK(health_thread)
{
    telem_data data;
    int i;
    while(1)
    {
        csp_sleep_ms(YOTTA_CFG_TELEMETRY_BEACON_INTERVAL);
        for (i = 0; i < TELEMETRY_NUM_BEACON; i++)
        {
            data = health_data[i];
            printf("HEALTH:%d:%d:%d\r\n", data.source.source_id, data.timestamp, data.data);
        }        
    }
}

#endif