#ifndef PROTOCOL_DEFS_HPP
#define PROTOCOL_DEFS_HPP

#include <stdint.h>

namespace altair {

// Protocol Constants
constexpr uint16_t PROTO_MAX_PAYLOAD_LEN = 260;
constexpr uint16_t PROTO_MAX_PACKET_LEN = (1 + 1 + PROTO_MAX_PAYLOAD_LEN + 1 + 1);
constexpr uint8_t PROTO_END_BYTE = 0x55;

// Packet Types
constexpr uint8_t PROTO_PKT_KEEP_ALIVE = 0x01;
constexpr uint8_t PROTO_PKT_EVENT = 0x02;
constexpr uint8_t PROTO_PKT_SAMPLE = 0x03;
constexpr uint8_t PROTO_PKT_TIME_SYNC = 0x04;

} // namespace altair

#endif // PROTOCOL_DEFS_HPP