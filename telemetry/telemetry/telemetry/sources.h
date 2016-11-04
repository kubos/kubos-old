#ifndef SOURCES_H
#define SOURCES_H

#include "telemetry/telemetry.h"

#define TELEMETRY_NUM_BEACON 3
#define TELEMETRY_NUM_HEALTH 2
static telem_source pos_x_source = { .source_id = 0, .dest_flag = TELEMETRY_BEACON_FLAG };
static telem_source pos_y_source = { .source_id = 1, .dest_flag = TELEMETRY_BEACON_FLAG };
static telem_source temp_source = { .source_id = 2, .dest_flag = TELEMETRY_HEALTH_FLAG };
static telem_source gps_source = { .source_id = 3, .dest_flag = TELEMETRY_BEACON_FLAG | TELEMETRY_HEALTH_FLAG };

#endif
