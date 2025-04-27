/*
 * protocol.c
 *
 *  Created on: Apr 6, 2025
 *      Author: HOME
 */


#include "protocol.h"
#include "config.h"

#include <string.h>
#include <stdio.h>

/* --- Public Functions --- */

uint16_t Protocol_Pack(uint8_t packet_id, const uint8_t* payload, uint8_t payload_len, uint8_t* out_buffer)
{
    if (payload_len > PROTO_MAX_PAYLOAD_LEN) return 0;

    out_buffer[0] = 1 + payload_len;
    out_buffer[1] = packet_id;
    memcpy(&out_buffer[2], payload, payload_len);

    uint8_t crc = out_buffer[0] ^ out_buffer[1];
    for (uint8_t i = 0; i < payload_len; ++i)
        crc ^= payload[i];

    out_buffer[2 + payload_len] = crc;
    out_buffer[3 + payload_len] = PROTO_END_BYTE;

    return 4 + payload_len;
}


int Protocol_Unpack(const uint8_t* buffer, uint16_t len, ProtocolPacket_t* out_packet)
{
    if (!buffer || !out_packet) {
        return 0;
    }

    uint8_t payload_len = buffer[0] - 1;
    uint8_t expected_len = 1 + 1 + payload_len + 1 + 1;

    if (len < expected_len) {
    	printf("[UNPACK] Received length: %u, Expected: %u\r\n", len, expected_len);
        return -1;
    }

    uint8_t packet_id = buffer[1];
    const uint8_t* payload = &buffer[2];
    uint8_t crc = buffer[2 + payload_len];
    uint8_t end_byte = buffer[3 + payload_len];

    if (end_byte != PROTO_END_BYTE) {
        return -2;
    }

    uint8_t computed_crc = buffer[0] ^ packet_id;
    for (uint8_t i = 0; i < payload_len; ++i)
        computed_crc ^= payload[i];

    if (crc != computed_crc) {
        return -3;
    }

    out_packet->length = buffer[0];
    out_packet->packet_id = packet_id;
    out_packet->payload_len = payload_len;
    memcpy(out_packet->payload, payload, payload_len);
    out_packet->checksum = crc;
    out_packet->valid = 1;

    return 1;
}
