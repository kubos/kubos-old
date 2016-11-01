#ifndef GATHER_H
#define GATHER_H

#include <csp/arch/csp_thread.h>

void gps_telem();

CSP_DEFINE_TASK(gather_thread);

#ifdef YOTTA_CFG_TELEMETRY
#define GATHER_THREAD   csp_thread_handle_t gather_handle; \
                        csp_thread_create(gather_thread, "BEACON", 1000, NULL, 0, &gather_handle);
#else
#define GATHER_THREAD
#endif

#endif