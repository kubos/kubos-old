#ifndef GATHER_H
#define GATHER_H

#include <csp/arch/csp_thread.h>

void gps_telem();

CSP_DEFINE_TASK(gather_thread);

#ifdef YOTTA_CFG_TELEMETRY
#define GATHER_THREADS   csp_thread_handle_t gather_handle; \
                        csp_thread_create(gather_thread, "GATHER", 50, NULL, 0, &gather_handle);
#else
#define GATHER_THREADS
#endif

#endif
