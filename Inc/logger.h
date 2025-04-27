/**
 * @file logger.h
 * @brief Handles logging of sensor data and system events to SD card.
 *
 * Provides a logging task and interface for writing structured data to
 * daily log files and a persistent event file. Supports event logging,
 * regular sampling logs, and sample history requests.
 */

#ifndef INC_LOGGER_H_
#define INC_LOGGER_H_

#include "config.h"
#include "FreeRTOS.h"
#include "queue.h"

/**
 * @brief Logger command structure used in the logger queue.
 */
typedef struct {
	Logger_Command_Type_t m_type;                  /**< Type of logger command */
	char m_payload[LOGGER_CMD_PAYLOAD_MAX];        /**< Payload data for the log */
} Logger_Command_t;

/**
 * @brief RTOS task responsible for handling log commands.
 *
 * Waits for logger commands via queue and writes data to SD card.
 * Supports writing sample logs, events, and processing sample history requests.
 *
 * @param _arg Unused task argument.
 */
void Logger_Task(void* _arg);

/**
 * @brief Handles a sample history request.
 *
 * Parses a request payload containing a date range, then reads and transmits
 * logged samples for each day within the specified range.
 *
 * @param payload Payload containing from-to date range in YYYYMMDD format.
 * @param len Length of the payload (should be at least 16).
 */
void Logger_HandleSampleRequest(const char* payload, uint8_t len);

#endif /* INC_LOGGER_H_ */
