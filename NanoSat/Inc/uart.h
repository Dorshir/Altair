/**
 * @file uart.h
 * @brief UART communication interface using STM32 HAL and FreeRTOS.
 *
 * Provides non-blocking UART transmission with semaphore-based synchronization.
 * Uses `huart2` as the communication channel and supports interrupt-based TX completion.
 */

#ifndef INC_UART_H_
#define INC_UART_H_

#include <stdint.h>
#include "stm32l4xx_hal.h"

/**
 * @brief Initializes UART transmission resources.
 *
 * Creates the semaphore used for guarding UART TX operations.
 * Must be called before using `UART_SendBytes()`.
 */
void UART_Init();

/**
 * @brief Sends a buffer of bytes via UART using interrupt-based transmission.
 *
 * Blocks until the UART is ready, then transmits data asynchronously.
 * Transmission completion is signaled via a FreeRTOS semaphore.
 *
 * @param _data Pointer to the data buffer to send.
 * @param _len Length of the data buffer.
 */
void UART_SendBytes(uint8_t const* _data, uint16_t _len);

#endif /* INC_UART_H_ */
