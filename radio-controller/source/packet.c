#include "radio-controller/packet.h"
#include <stdio.h>

bool packet_parse(const uint8_t * buffer, packet * packet)
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
    packet p;

    packet_parse(base, &p);

    // At this point we have the payload in p.payload
    // Now we....
    // - Determine message type from payload
    // - Pass along parameters to the appropriate message abstraction method
}
