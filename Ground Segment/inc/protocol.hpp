#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP

#include "packet.hpp"
#include <optional>
#include <vector>
#include <cstdint>

namespace altair {

class Protocol {
public:

    /// Computes the checksum for a given packet.
    static std::vector<uint8_t> pack(const Packet& pkt);

    /// Unpacks a raw byte buffer into a Packet structure.
    static std::optional<Packet> unpack(const uint8_t* buffer, uint16_t len);
};

} //namespace altair

#endif // PROTOCOL_HPP
