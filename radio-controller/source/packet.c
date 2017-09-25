#include "radio-controller/packet.h"
#include <stdio.h>

bool packet_parse(const uint8_t * buffer, telecommand_packet * packet)
{
    if ((NULL != buffer) && (NULL != packet))
    {
        memcpy(packet, buffer, PACKET_SIZE);
        return true;
    }
    return false;
}

void packet_process(char * base, size_t len)
{
    printf("In Packet Processor. w00t!\n");
    telecommand_packet  p;
    uint8_t service_id, function_id;

    telecommand_packet * p_p = (telecommand_packet *) base;

    packet_parse(base, &p);

    packet_debug(p);
    packet_debug(*p_p);

    // At this point we have the payload in p.payload
    // Now we....
    // - Determine message type from payload
    // - Pass along parameters to the appropriate message abstraction method
    service_id  = p.data.payload[0];
    function_id = p.data.payload[1];
    printf("Service id %d function id %d\n", service_id, function_id);
    if (p.data.header.service_type == 8)
    {
        switch (service_id)
        {
            // Power Manager Case
            case 1:
            {
                // Enable Line function
                switch (function_id)
                {
                    case 1:
                    {
                        printf("Calling enable line\n");
                        break;
                    }
                    break;
                }
            }
        }
    }
}

void packet_debug(telecommand_packet p)
{
    printf("packet:id:version %x\n", p.id.version);
    printf("packet:id:type %x\n", p.id.type);
    printf("packet:id:dfh %x\n", p.id.data_field_header);
    printf("packet:id:app_id %x\n", p.id.app_id);
    printf("packet:sequence:flags %x\n", p.sequence.flags);
    printf("packet:sequence:count %x\n", p.sequence.count);
    printf("packet::data_length %d\n", p.data_length);
    printf("packet:data:sheader %x\n", p.data.header.ccsds_secondary_header);
    printf("packet:data:tcver %x\n", p.data.header.tc_packet_version);
    printf("packet:data:ack %x\n", p.data.header.ack);
    printf("packet:data:service_type %x\n", p.data.header.service_type);
    printf("packet:data:service_subtype %x\n", p.data.header.service_subtype);
    printf("packet:payload:service_id %x\n", p.data.payload[0]);
    printf("packet:payload:function_id %x\n", p.data.payload[1]);
}
