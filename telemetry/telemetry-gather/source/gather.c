#ifdef YOTTA_CFG_TELEMETRY

#include "telemetry-gather/gather.h"
#include "telemetry/telemetry.h"
#include "telemetry/sources.h"

#include <csp/csp.h>
#include <csp/arch/csp_time.h>

static uint8_t data = 0;
static uint8_t temp_data = 255;
static uint16_t pos_data = 0;

/**
 * Functions for gathering data would be generated via config
 * or placed in some custom code file.
 */
void gps_telem()
{
    telem_data gps_data;
    gps_data.data = data++;
    gps_data.source = gps_source;
    gps_data.timestamp = csp_get_ms();

    telemetry_submit(gps_data);
}

void temp_telem()
{
    telem_data data;
    data.data = temp_data--;
    data.source = temp_source;
    data.timestamp = csp_get_ms();

    telemetry_submit(data);
}

void pos_x_telem()
{
    telem_data data;
    data.data = pos_data--;
    data.source = pos_x_source;
    data.timestamp = csp_get_ms();

    telemetry_submit(data);
}

CSP_DEFINE_TASK(gather_thread)
{
    while(1)
    {
        /**
         * Likewise the list of functions called in the gathering thread
         * would ideally be generated.
         */
        gps_telem();
        csp_sleep_ms(100);
        temp_telem();
        csp_sleep_ms(100);
        pos_x_telem();
        csp_sleep_ms(YOTTA_CFG_TELEMETRY_GATHER_INTERVAL);
    }
}

#endif