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

/**
 * @defgroup CSP
 * @addtogroup CSP
 * @brief Internal CSP Handler API
 * @{
 */

#ifndef CSP
#define CSP

#include <csp/csp.h>

/**
 * Performs routine csp setup tasks (buffer, init, route_task) and
 * manages global init state.
 * int csp_address address to init csp instance with
 * return bool true if init is successful
 */
bool kubos_csp_init(int csp_address);

/**
 * Performs routine csp cleanup and termination tasks
 */
void kubos_csp_terminate(void);

#endif

/* @} */