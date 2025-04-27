/**
 * @file communicator.h
 * @brief Handles UART-based communication with the ground station.
 *
 * This module provides functionality to initialize and run the communicator task,
 * manage message queues, handle protocol packet transmission and reception, and
 * respond to system events such as keep-alive, event logs, and sample requests.
 */

#ifndef INC_COMMUNICATOR_H_
#define INC_COMMUNICATOR_H_

#include "protocol.h"
#include "config.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "event_groups.h"

#include <stdint.h>

/**
 * @brief Raw received UART packet container.
 */
typedef struct {
    uint8_t raw[PROTO_MAX_PACKET_LEN]; /**< Raw packet buffer */
    uint16_t len;                      /**< Actual length of the packet */
} CommunicatorRawPacket_t;

/**
 * @brief Outgoing message structure used by application to send data via communicator.
 */
typedef struct {
    uint8_t packet_id;                 /**< Protocol packet ID */
    uint8_t payload[254];              /**< Message payload */
    uint8_t payload_len;               /**< Length of payload */
} CommunicatorMessage_t;

/**
 * @brief Initializes communicator queues, UART, and event flags.
 *
 * Must be called before using any other communicator functionality.
 */
void Communicator_Init();

/**
 * @brief Main RTOS task for the communicator.
 *
 * Waits for communication-related events and handles sending and receiving
 * packets over UART.
 *
 * @param _arg Unused task argument.
 */
void Communicator_Task(void* _arg);

/**
 * @brief Triggers a keep-alive message to be sent to the ground station.
 *
 * Called by application logic or other tasks to signal system is alive.
 */
void Communicator_SendKeepAlive();

#endif /* INC_COMMUNICATOR_H_ */
