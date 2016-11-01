/**
 * The contents of this file would ideally be generated based
 * off of configuration values.
 */

#ifndef SOURCES_H
#define SOURCES_H

#include "telemetry/service.h"

static telem_source gps_source = {
    .source_id = 0,
    .in_beacon = 1
};

static telem_source temp_source = {
    .source_id = 1,
    .in_beacon = 1
};

#endif