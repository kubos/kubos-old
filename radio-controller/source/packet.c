#include "radio-controller/packet.h"
#include <stdio.h>

bool spacepacket_parse(const uint8_t * buffer, spacepacket * packet)
{
  memcpy(packet, buffer, PACKET_SIZE);
  return true;
}

void packet_process( char * base, size_t len )
{
  printf( "In Packet Processor. w00t!\n" );
}
