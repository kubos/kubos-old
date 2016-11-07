#ifdef YOTTA_CFG_TELEMETRY

#include <telemetry/telemetry.h>
#include "telemetry-health/health.h"
#include <csp/csp.h>

static uint8_t data_count = 0;

void update(telem_data data)
{
    if (data.source.dest_flag & TELEMETRY_HEALTH_FLAG)
    {
        /* A dirty hack to fill up the whole array with data */
        health_data[data_count++ % TELEMETRY_NUM_HEALTH] = data;
    }
}

/**
 * Thread for receiving data packets from telemetry service
 */
CSP_DEFINE_TASK(health_rx_thread)
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

/**
 * Thread for performing health/limit checks on data
 */
CSP_DEFINE_TASK(health_thread)
{
    telem_data data;
    int i;
    while(1)
    {
        csp_sleep_ms(YOTTA_CFG_TELEMETRY_BEACON_INTERVAL);
        for (i = 0; i < TELEMETRY_NUM_HEALTH; i++)
        {
            data = health_data[i];
            printf("HEALTH:%d:%d:%d\r\n", data.source.source_id, data.timestamp, data.data);
            /* Perform limit check and take action here
                if (data > limit)
                    log("alert bad")
            */
        }        
    }
}

#endif