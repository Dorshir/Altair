#include "protocol.hpp"
#include "protocol_defs.hpp"

#include <iomanip>
#include <cstring>
#include <iostream>

namespace altair {

static uint8_t computeChecksum(uint8_t length,
                               uint8_t packetId,
                               const uint8_t* payload,
                               uint8_t payloadLen)
{
    uint8_t cs = length ^ packetId;
    for (uint8_t i = 0; i < payloadLen; ++i) {
        cs ^= payload[i];
    }
    return cs;
}

std::vector<uint8_t> Protocol::pack(const Packet& pkt)
{
    const uint8_t payloadLen = static_cast<uint8_t>(pkt.payload.size());
    const uint8_t length     = payloadLen + 1;

    std::vector<uint8_t> out;
    out.reserve(length + 3);        // 1(L) + length + 1(CRC) + 1(END)

    out.push_back(length);          // [0] Length
    out.push_back(pkt.packetId);    // [1] Packet‑ID
    out.insert(out.end(),
               pkt.payload.begin(),
               pkt.payload.end());  // [2..] Payload

    uint8_t crc = computeChecksum(length,
                                  pkt.packetId,
                                  out.data() + 2,
                                  payloadLen);
    out.push_back(crc);             // CRC
    out.push_back(Packet::END_BYTE);

    return out;
}

std::optional<Packet> Protocol::unpack(const uint8_t* buffer, uint16_t len) {
    if (!buffer || len < 4) {
        return std::nullopt;
    }

    uint8_t length = buffer[0];
    uint8_t payloadLen = length - 1;
    uint16_t expected = 1 + 1 + payloadLen + 1 + 1;


    if (len < expected) {
        return std::nullopt;
    }

    if (buffer[expected-1] != Packet::END_BYTE) {
        return std::nullopt;
    }

    uint8_t crc = buffer[expected-2];
    uint8_t calcCrc = computeChecksum(length, buffer[1], 
                                    &buffer[2], payloadLen);
    
    if (crc != calcCrc) {
        return std::nullopt;
    }

    Packet pkt;
    pkt.length = length;
    pkt.packetId = buffer[1];
    pkt.payload.assign(buffer + 2, buffer + 2 + payloadLen);
    pkt.checksum = crc;
    return pkt;
}

} // namespace altair