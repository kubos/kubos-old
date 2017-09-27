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

#include <arpa/inet.h>
#include <evented-control/ecp.h>
#include <evented-control/messages.h>
#include <stdio.h>

#include "radio-controller/packet.h"
#include "radio-controller/telecommand.h"

#define ID_VERSION_OFFSET 0
#define ID_VERSION_MASK 0b11100000
#define ID_VERSION_SHIFT 5

#define ID_TYPE_OFFSET 0
#define ID_TYPE_MASK 0b00010000
#define ID_TYPE_SHIFT 4

#define ID_HEADER_OFFSET 0
#define ID_HEADER_MASK 0b00001000
#define ID_HEADER_SHIFT 3;

#define ID_APID1_OFFSET 0
#define ID_APID1_MASK 0b00000111
#define ID_APID1_SHIFT 8

#define ID_APID2_OFFSET 1

#define SEQ_FLAGS_OFFSET 2
#define SEQ_FLAGS_MASK 0b11000000
#define SEQ_FLAGS_SHIFT 6

#define SEQ_COUNT_OFFSET 2
#define SEQ_COUNT_MASK 0b00111111
#define SEQ_COUNT_SHIFT 8

#define SEQ_COUNT2_OFFSET 3

#define DATA_LENGTH1_OFFSET 4
#define DATA_LENGTH2_OFFSET 5

#define DATA_HEAD_FLAG_OFFSET 6
#define DATA_HEAD_FLAG_MASK 0b10000000
#define DATA_HEAD_FLAG_SHIFT 7

#define DATA_HEAD_VER_OFFSET 6
#define DATA_HEAD_VER_MASK 0b01110000
#define DATA_HEAD_VER_SHIFT 4

#define DATA_HEAD_ACK_OFFSET 6
#define DATA_HEAD_ACK_MASK 0b00001111

#define DATA_HEAD_SER_TYPE_OFFSET 7
#define DATA_HEAD_SER_SUB_OFFSET 8

#define DATA_PAYLOAD_OFFSET 9

#define DATA_ERROR1_OFFSET 9
#define DATA_ERROR2_OFFSET 10

bool telecommand_parse(const uint8_t * buffer, telecommand_packet * packet)
{
    if ((NULL != buffer) && (NULL != packet))
    {
        packet->id.version = (*(buffer + ID_VERSION_OFFSET) & ID_VERSION_MASK)
                             >> ID_VERSION_SHIFT;
        packet->id.type
            = (*(buffer + ID_TYPE_OFFSET) & ID_TYPE_MASK) >> ID_TYPE_SHIFT;
        packet->id.data_field_header
            = (*(buffer + ID_HEADER_OFFSET) & ID_HEADER_MASK)
              >> ID_HEADER_SHIFT;

        packet->id.data_field_header = (buffer[0] & ID_HEADER_MASK) >> 3;
        packet->id.app_id
            = ((*(buffer + ID_APID1_OFFSET) & ID_APID1_MASK) << ID_APID1_SHIFT)
              | *(buffer + ID_APID2_OFFSET);

        packet->sequence.flags = (*(buffer + SEQ_FLAGS_OFFSET) & SEQ_FLAGS_MASK)
                                 >> SEQ_FLAGS_SHIFT;
        packet->sequence.count
            = (*(buffer + SEQ_COUNT_OFFSET) & SEQ_COUNT_MASK) << SEQ_COUNT_SHIFT
              | *(buffer + SEQ_COUNT2_OFFSET);

        packet->data_length = *(buffer + DATA_LENGTH1_OFFSET) << 8
                              | *(buffer + DATA_LENGTH2_OFFSET);

        packet->data.header.ccsds_secondary_header
            = (*(buffer + DATA_HEAD_FLAG_OFFSET) & DATA_HEAD_FLAG_MASK)
              >> DATA_HEAD_FLAG_SHIFT;
        packet->data.header.tc_packet_version
            = (*(buffer + DATA_HEAD_VER_OFFSET) & DATA_HEAD_VER_MASK)
              >> DATA_HEAD_VER_SHIFT;
        packet->data.header.ack
            = (*(buffer + DATA_HEAD_ACK_OFFSET) & DATA_HEAD_ACK_MASK);
        packet->data.header.service_type
            = *(buffer + DATA_HEAD_SER_TYPE_OFFSET);
        packet->data.header.service_subtype
            = *(buffer + DATA_HEAD_SER_SUB_OFFSET);

        packet->data.payload = malloc(sizeof(uint8_t) * packet->data_length);
        for (int i = 0; i < packet->data_length; i++)
        {
            packet->data.payload[i] = *(buffer + i + DATA_PAYLOAD_OFFSET);
        }

        packet->data.error_control
            = (*(buffer + packet->data_length + DATA_ERROR1_OFFSET) << 8)
              | *(buffer + packet->data_length + DATA_ERROR2_OFFSET);

        return true;
    }
    return false;
}

void telecommand_run(telecommand_packet p)
{
    uint8_t service_id, function_id;
    service_id  = p.data.payload[0];
    function_id = p.data.payload[1];

    tECP_Context context;

    if (ECP_NOERR != ECP_Init(&context, "org.KubOS.client"))
    {
        printf("Error with ECP_Init\n");
    }

    switch (service_id)
    {
        /** Power Manager **/
        case 1:
        {
            switch (function_id)
            {
                /** Enable Rail **/
                case 1:
                {
                    printf("Calling enable line\n");
                    if (ECP_NOERR != enable_line(&context, 1))
                    {
                        printf("Error with enable_line\n");
                    }
                    break;
                }
            }
            break;
        }
    }

    for (int i = 0; i < 15; i++)
    {
        //        ECP_Loop(&context, 1000);
    }

    ECP_Destroy(&context);
}

void telecommand_process(char * base, size_t len)
{
    printf("In Packet Processor. w00t!\n");
    telecommand_packet p;

    telecommand_parse(base, &p);
    telecommand_run(p);
}

void telecommand_debug(telecommand_packet p)
{
    printf("packet:id:version %d\n", p.id.version);
    printf("packet:id:type %d\n", p.id.type);
    printf("packet:id:dfh %d\n", p.id.data_field_header);
    printf("packet:id:app_id %d\n", p.id.app_id);
    printf("packet:sequence:flags %d\n", p.sequence.flags);
    printf("packet:sequence:count %d\n", p.sequence.count);
    printf("packet::data_length %d\n", p.data_length);
    printf("packet:data:sheader %d\n", p.data.header.ccsds_secondary_header);
    printf("packet:data:tcver %d\n", p.data.header.tc_packet_version);
    printf("packet:data:ack %d\n", p.data.header.ack);
    printf("packet:data:service_type %d\n", p.data.header.service_type);
    printf("packet:data:service_subtype %d\n", p.data.header.service_subtype);
    printf("packet:data:error_control %d\n", p.data.error_control);

    /**
    uint8_t buffer[PACKET_SIZE];
    memcpy(buffer, &p, PACKET_SIZE);
    for (int i = 0; i < PACKET_SIZE; i++) printf("0x%02x ", buffer[i]);
    printf("\n");
    **/
}
