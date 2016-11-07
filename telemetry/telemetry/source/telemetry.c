#ifdef YOTTA_CFG_TELEMETRY

#include "telemetry/telemetry.h"

#include <csp/csp.h>

void telemetry_submit(telem_data data)
{
    telemetry_log_data(data);
    telemetry_publish(data);
}

/**
 * This function represents a future "storage" destination for telemetry data.
 * Ideally this function would not exist, instead telemetry would publish data
 * to storage in the same way it publishes data to beacon and health
 */
void telemetry_log_data(telem_data data)
{
    printf("TELEM:%d:%d:%d\r\n", data.source.source_id, data.timestamp, data.data);
}

void telemetry_publish_send(uint8_t port, telem_data data)
{
    csp_packet_t * packet;
	csp_conn_t * conn;
    packet = csp_buffer_get(10);
    if (packet == NULL) {
        /* Could not get buffer element */
        printf("Failed to get buffer element\n");
        return CSP_TASK_RETURN;
    }

    /* Connect to host HOST, port PORT with regular UDP-like protocol and 1000 ms timeout */
    conn = csp_connect(CSP_PRIO_NORM, 1, port, 1000, CSP_O_NONE);
    if (conn == NULL) {
        /* Connect failed */
        printf("Connection failed\n");
        /* Remember to free packet buffer */
        csp_buffer_free(packet);
        return CSP_TASK_RETURN;
    }

    memcpy(packet->data, &data, sizeof(telem_data));

    /* Set packet length */
    packet->length = sizeof(telem_data);

    // /* Send packet */
    if (!csp_send(conn, packet, 1000)) {
        /* Send failed */
        printf("Send failed\n");
        csp_buffer_free(packet);
    }
    
    csp_close(conn);
}


void telemetry_publish(telem_data data)
{
    /** autogen list of ports which telem should publish to */
    telemetry_publish_send(TELEMETRY_BEACON_PORT, data);
    telemetry_publish_send(TELEMETRY_HEALTH_PORT, data);
    /* Future destination for storage - to replace logging direct in telemetry layer*/
    //telemetry_publish_send(TELEMETRY_STORAGE_PORT, data);
}

#endif