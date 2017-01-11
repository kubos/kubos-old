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

#include <errno.h>
#include <stddef.h>
#include <stdio.h>

#include "kubos-hal-msp430f5529/msp430-stdio.h"
#include "kubos-hal/uart.h"

ssize_t read(int fd, void *ptr, size_t len)
{
    if (fd == STDIN_FILENO) {
        return k_uart_read(K_UART_CONSOLE, (char *) ptr, (int) len);
    }
    errno = ENOSYS;
    return len;
}

ssize_t write(int fd, const void *ptr, size_t len)
{
    if (fd == STDOUT_FILENO || fd == STDERR_FILENO) {
        return k_uart_write(K_UART_CONSOLE, (char *) ptr, (int) len);
    }

    errno = ENOSYS;
    return len;
}
