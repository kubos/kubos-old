/*
 * Copyright (C) 2017 Kubos Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "gom-eps-api/eps.h"
#include <stddef.h>
#include <string.h>
#include <stdio.h>

static eps_power_state power_state = {
    .line_1 = 1
};

static eps_battery_state battery_state = {
    .voltage = 3.5
};

eps_err eps_enable_power_line(uint16_t line)
{
    printf("EPS enable line %d\n", line);
    return EPS_NO_ERR;
}

eps_err eps_disable_power_line(uint16_t line)
{
    printf("EPS disable line %d\n", line);
    return EPS_NO_ERR;
}

eps_err eps_get_power_status(eps_power_state * state)
{
    if (state != NULL)
    {
        memcpy(state, &power_state, sizeof(eps_power_state));
    }
    return EPS_NO_ERR;
}

eps_err eps_get_battery_state(eps_battery_state * state)
{
    if (state != NULL)
    {
        memcpy(state, &battery_state, sizeof(eps_battery_state));
    }
    return EPS_NO_ERR;
}