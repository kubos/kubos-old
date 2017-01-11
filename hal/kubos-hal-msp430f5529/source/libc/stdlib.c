/*
 * KubOS HAL
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

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "kubos-hal-msp430f5529/msp430-stdio.h"

void atexit(void (*fn)(void)) { } // noop stub

void *calloc (size_t nelem, size_t elsize)
{
  register void *ptr;
  if (nelem == 0 || elsize == 0)
    nelem = elsize = 1;

  ptr = malloc (nelem * elsize);
  if (ptr) bzero (ptr, nelem * elsize);

  return ptr;
}
