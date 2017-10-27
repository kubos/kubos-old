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

#include <eps-api/eps.h>
#include <evented-control/ecp.h>
#include <evented-control/messages.h>
#include <stdio.h>

KECPStatus enable_line_handler(uint8_t line);

int main()
{
    KECPStatus      err = ECP_OK;
    int             i;
    int             initialized = 0;
    eps_power_state status;
    ecp_context     context;

    do
    {
        if (ECP_OK != (err = ecp_init(&context, POWER_MANAGER_INTERFACE)))
        {
            fprintf(stderr, "Error %d calling ecp_init()\n", err);
            break;
        }

        if (ECP_OK != on_enable_line(&context, &enable_line_handler))
        {
            fprintf(stderr, "Error registering enable line callback\n");
            break;
        }

        /* Now loop for (at most) 15 seconds, looking for a message */
        for (i = 0; err == ECP_NOERR; i++)
        {
            printf("Sending power status\n");
            DBusMessage * message;
            eps_get_power_status(&status);
            format_power_status_message(status, &message);
            ecp_send(&context, message);
            err = ecp_loop(&context, 1000);
        }

        if (err != ECP_OK)
        {
            fprintf(stderr, "Error %d calling ecp_loop()\n", err);
        }
    } while (0);

    if (ECP_OK != (err = ecp_destroy(&context)))
    {
        fprintf(stderr, "Error %d calling ECP_Destroy()\n", err);
    }

    return err;
}

KECPStatus enable_line_handler(uint8_t line)
{
    printf("Enable line..\n");
    eps_enable_power_line(line);
}
