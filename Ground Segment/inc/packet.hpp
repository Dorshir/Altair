
#ifndef PACKET_HPP
#define PACKET_HPP

#include "protocol_defs.hpp"

#include <cstdint>
#include <vector>

namespace altair {

struct Packet {

  uint8_t length; 
  uint8_t packetId;
  std::vector<uint8_t> payload;
  uint8_t checksum;
  static constexpr uint8_t END_BYTE = PROTO_END_BYTE;

};

} //namespace altair

#endif // PACKET_HPP