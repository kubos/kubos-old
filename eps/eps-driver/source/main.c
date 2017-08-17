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
#include <stdio.h>
#include "evented-control/ecp.h"

tECP_Error func(tECP_Context * context, tECP_Channel channel,
                tECP_Message * message);

int main()
{
    tECP_Context context;
    tECP_Error   err = ECP_E_NOERR;
    tECP_Message msg;
    int          i;
    int          initialized = 0;

    do
    {
        if (ECP_E_NOERR != (err = ECP_Init(&context)))
        {
            printf("Error %d calling ECP_Init()\n", err);
            break;
        }

        initialized = 1;

        /* Send a message before calling loop. perfectly acceptable */
        msg.id = ECP_M_SYS_BEGIN;
        // msg.begin.id  = 0xCAFEB0EF;

        if (ECP_E_NOERR != (err = ECP_Broadcast(&context, ECP_C_SYS, &msg)))
        {
            printf("Error %d calling ECP_Broadcast()\n", err);
            break;
        }

        if (ECP_E_NOERR != (err = ECP_Listen(&context, ECP_C_SYS, func)))
        {
            printf("Error %d calling ECP_Listen()\n", err);
            break;
        }

        /* Now loop for (at most) 15 seconds, looking for a heartbeat */
        for (i = 0; (i < 15) && (err == ECP_E_NOERR); i++)
        {
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

tECP_Error func(tECP_Context * context, tECP_Channel channel,
                tECP_Message * message)
{
    tECP_Error   err = ECP_E_NOERR;
    tECP_Message msg;

    // if (ECP_C_SYS != channel)
    // {
    //     printf("That's weird, we received a message on channel %d\n",
    //     channel);
    //     err = TCP_E_GENERIC;
    // }
    // else
    // {
    //     switch (message.messageid)
    //     {
    //         case ECP_M_SYS_HEARTBEAT:
    //             printf("Listen to my heartbeat\n");

    //             /* You can send a message from w/i a message handler */
    //             msg.messageid = ECP_M_SYS_NULL;
    //             err           = ECP_Broadcast(&context, ECP_C_SYS, &msg);
    //             break;
    //     }
    // }

    return (err);
}