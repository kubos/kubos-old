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

tECP_Error enable_line_handler(uint8_t line);

DBusHandlerResult message_handler(DBusConnection * connection,
                                  DBusMessage * message, void * user_data);
static tECP_Context context;

int main()
{
    tECP_Error       err = ECP_E_NOERR;
    int              i;
    int              initialized = 0;
    eps_power_status status;

    do
    {
        if (ECP_E_NOERR != (err = ECP_Init(&context, POWER_MANAGER_INTERFACE,
                                           message_handler)))
        {
            printf("Error %d calling ECP_Init()\n", err);
            break;
        }

        initialized = 1;

        if (ECP_E_NOERR != on_enable_line(&context, &enable_line_handler))
        {
            printf("Error registering enable line callback\n");
        }

        /* Now loop for (at most) 15 seconds, looking for a message */
        for (i = 0; (i < 15) && (err == ECP_E_NOERR); i++)
        {
            printf("Sending power status\n");
            DBusMessage * message;
            eps_get_power_status(&status);
            format_power_status_message(status, &message);
            ECP_Broadcast(&context, message);
            err = ECP_Loop(&context, 1000);
        }

        if (err != ECP_E_NOERR)
        {
            printf("Error %d calling ECP_Loop()\n", err);
            break;
        }
    } while (0);

    if (1 == initialized)
    {
        if (ECP_E_NOERR != (err = ECP_Destroy(&context)))
        {
            printf("Error %d calling ECP_Destroy()\n", err);
        }
    }

    if (ECP_E_NOERR == err)
    {
        return (0);
    }
    else
    {
        return (2);
    }
}

DBusHandlerResult message_handler(DBusConnection * connection,
                                  DBusMessage * message, void * user_data)
{
    if (ECP_E_NOERR == ECP_Handle_Message(&context, message))
    {
        return DBUS_HANDLER_RESULT_HANDLED;
    }

    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

tECP_Error enable_line_handler(uint8_t line)
{
    printf("Enable line..\n");
    eps_enable_power_line(line);
}