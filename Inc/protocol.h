/**
 * @file protocol.h
 * @brief Custom UART communication protocol for packet packing and unpacking.
 *
 * Defines the structure and utilities for encoding and decoding protocol packets,
 * used for communication between embedded systems and external interfaces.
 *
 * ## Packet Format
 * Each packet follows the structure:
 *
 * | Byte Index | Field        | Description                          |
 * |------------|--------------|--------------------------------------|
 * | 0          | Length       | Total number of bytes (excluding end byte) |
 * | 1          | Packet ID    | Type of message                      |
 * | 2..N-3     | Payload      | Actual data (up to PROTO_MAX_PAYLOAD_LEN) |
 * | N-2        | Checksum     | Simple checksum over Length + ID + Payload |
 * | N-1        | End Byte     | Fixed end-of-packet marker (e.g., 0xFF)     |
 *
 * The `length` byte defines the size of the packet from `packet_id` to `checksum` (inclusive).
 * The `end byte` is not included in the `length`.
 */

#ifndef INC_PROTOCOL_H_
#define INC_PROTOCOL_H_

#include "config.h"
#include <stdint.h>

/**
 * @brief Structure representing a decoded protocol packet.
 */
typedef struct {
    uint8_t length;                                 /**< Total packet length (excluding end byte) */
    uint8_t packet_id;                              /**< Type identifier of the packet */
    uint8_t payload[PROTO_MAX_PAYLOAD_LEN];         /**< Raw payload data */
    uint8_t payload_len;                            /**< Length of the payload */
    uint8_t checksum;                               /**< Packet checksum */
    uint8_t valid;                                  /**< Flag indicating whether the packet is valid (1 = valid, 0 = invalid) */
} ProtocolPacket_t;

/**
 * @brief Packs a payload into a protocol-compliant byte stream.
 *
 * Adds framing, checksum, and header information to the payload.
 *
 * @param packet_id ID of the packet type.
 * @param payload Pointer to the payload data.
 * @param payload_len Length of the payload in bytes.
 * @param out_buffer Buffer to store the encoded packet.
 * @return Total length of the encoded packet, or 0 on error.
 */
uint16_t Protocol_Pack(uint8_t packet_id, const uint8_t* payload, uint8_t payload_len, uint8_t* out_buffer);

/**
 * @brief Unpacks and validates a received byte buffer into a protocol packet.
 *
 * Extracts header, payload, and validates the checksum.
 *
 * @param buffer Input buffer containing the raw packet data.
 * @param len Length of the input buffer.
 * @param out_packet Output structure to hold the parsed packet.
 * @return 1 if successfully unpacked and valid, 0 otherwise.
 */
int Protocol_Unpack(const uint8_t* buffer, uint16_t len, ProtocolPacket_t* out_packet);

#endif /* INC_PROTOCOL_H_ */
