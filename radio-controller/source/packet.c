#include "radio-controller/packet.h"


bool spacepacket_parse(const uint8_t * buffer, spacepacket * packet)
{
  memcpy(packet, buffer, PACKET_SIZE);
  return true;
}
