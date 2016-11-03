#ifdef YOTTA_CFG_TELEMETRY

#include <telemetry/telemetry.h>
#include "telemetry-beacon/beacon.h"
#include <csp/csp.h>

void telemetry_update_beacon(telem_data data)
{
    if (data.source.dest_flag & TELEMETRY_BEACON_FLAG)
    {
        // telem_beacon[data.source.beacon_id] = data;
        telem_beacon[data.source.source_id % TELEMETRY_NUM_BEACON] = data;
        // printf("BEACON:%d:%d:%d\r\n", data.source.source_id, data.timestamp, data.data);
    }
}

CSP_DEFINE_TASK(beacon_rx_thread)
{
    int running = 1;
    csp_socket_t *socket = csp_socket(CSP_SO_NONE);
    csp_conn_t *conn;
    csp_packet_t *packet;

    csp_bind(socket, TELEMETRY_HEALTH_PORT);
    csp_listen(socket, 5);
    telem_data data;

    while(running) {
        if( (conn = csp_accept(socket, 10000)) == NULL ) {
            continue;
        }

        while( (packet = csp_read(conn, 100)) != NULL ) {
            switch( csp_conn_dport(conn) ) {
                case TELEMETRY_HEALTH_PORT:
                    data = *((telem_data*)packet->data);
                    csp_buffer_free(packet);
                    telemetry_update_beacon(data);
                    break;
                default:
                    csp_service_handler(conn, packet);
                    break;
            }
        }

        csp_close(conn);
    }
}

CSP_DEFINE_TASK(beacon_thread)
{
    telem_data data;
    int i;
    while(1)
    {
        csp_sleep_ms(YOTTA_CFG_TELEMETRY_BEACON_INTERVAL);
        for (i = 0; i < TELEMETRY_NUM_BEACON; i++)
        {
            data = telem_beacon[i];
            printf("BEACON:%d:%d:%d\r\n", data.source.source_id, data.timestamp, data.data);
        }        
    }
}

#endif