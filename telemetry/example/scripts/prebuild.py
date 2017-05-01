#!/usr/bin/python

import json
import os

json_data = []
source_format = "static telem_source %s_source = { .source_id = %d, .dest_flag = %s };\r\n"

with open("config.json") as json_file:
    json_data = json.load(json_file)

source_data = ""
beacon_counter = 0
health_counter = 0
source_counter = 0

if 'telemetry' in json_data:
    if 'sources' in json_data['telemetry']:
        num_sources = len(json_data['telemetry']['sources'])
        for source in json_data['telemetry']['sources']:
            dest_flag = 0
            if 'beacon' in json_data['telemetry']['sources'][source]:
                dest_flag = "TELEMETRY_BEACON_FLAG"
                beacon_counter += 1
            if 'health' in json_data['telemetry']['sources'][source]:
                if dest_flag == 0:
                    dest_flag = "TELEMETRY_HEALTH_FLAG"
                else:
                    dest_flag += " | TELEMETRY_HEALTH_FLAG"
                health_counter += 1
            source_data += source_format % (source, source_counter, dest_flag)
            source_counter += 1

SOURCES_HEADER = """#ifndef SOURCES_H
#define SOURCES_H

#include "telemetry/telemetry.h"
"""

SOURCES_DEFINES = """
#define TELEMETRY_NUM_BEACON %d
#define TELEMETRY_NUM_HEALTH %d
"""

SOURCES_FOOTER = """
#endif
"""

with open("yotta_modules/telemetry/telemetry/sources.h", "w") as header:
    header.write(SOURCES_HEADER)
    header.write(SOURCES_DEFINES % (beacon_counter, health_counter))
    header.write(source_data)
    header.write(SOURCES_FOOTER)