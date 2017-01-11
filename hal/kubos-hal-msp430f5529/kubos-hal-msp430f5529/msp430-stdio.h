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
#ifndef KUBOS_HAL_MSP430F5529_MSP430_STDIO_H
#define KUBOS_HAL_MSP430F5529_MSP430_STDIO_H

#define STDIN_FILENO  0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

#include <stddef.h>
#include <stdarg.h>

typedef size_t ssize_t;

int vdprintf(int fd, const char *format, va_list ap);
ssize_t read(int fd, void *ptr, size_t len);
ssize_t write(int fd, const void *ptr, size_t len);
void atexit(void (*fn)(void));
void *calloc (size_t nelem, size_t elsize);

#endif
