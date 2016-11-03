#ifndef HEALTH_H
#define HEALTH_H

#include <stdint.h>
#include <csp/arch/csp_thread.h>

#include <telemetry/sources.h>

static telem_data health_data[TELEMETRY_NUM_HEALTH];

CSP_DEFINE_TASK(health_thread);
CSP_DEFINE_TASK(health_rx_thread);

#ifdef YOTTA_CFG_TELEMETRY
#define HEALTH_THREAD   csp_thread_handle_t health_handle; \
                        csp_thread_handle_t health_rx_handle; \
                        csp_thread_create(health_thread, "HEALTH", 50, NULL, 0, &health_handle); \
                        csp_thread_create(health_rx_thread, "HEALTH_RX", 50, NULL, 0, &health_rx_handle);
#else
#define HEALTH_THREAD
#endif


#endif