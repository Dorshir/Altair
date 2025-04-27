/**
 * @file dht.h
 * @brief Interface for communicating with a DHT temperature and humidity sensor.
 *
 * Provides structures and functions to initialize a DHT sensor and retrieve
 * temperature and humidity data using GPIO and timer peripherals.
 */

#ifndef INC_DHT_H_
#define INC_DHT_H_

#include "usart.h"  // For GPIO and timer typedefs

/**
 * @brief Structure holding a parsed DHT sensor reading.
 */
typedef struct DHTData {
	uint8_t m_humidity_int;   /**< Integer part of humidity */
	uint8_t m_humidity_frac;  /**< Fractional part of humidity */
	uint8_t m_temp_int;       /**< Integer part of temperature */
	uint8_t m_temp_frac;      /**< Fractional part of temperature */
	uint8_t m_checksum;       /**< Raw checksum from the sensor */
} DHTData;

/**
 * @brief Handle structure for a DHT sensor instance.
 */
typedef struct DHT {
	uint16_t m_GPIO_Pin;           /**< Data pin used for communication */
	GPIO_TypeDef *m_GPIOx;         /**< GPIO port */
	TIM_HandleTypeDef* m_timer;    /**< Timer used for signal timing */
} DHT;

/**
 * @brief Result codes for DHT operations.
 */
typedef enum DHTResult {
	DHT_OK,                 /**< Operation successful */
	DHT_NULLPTR_ERROR,      /**< Null pointer provided */
	DHT_INVALID_CHECKSUM,   /**< Data received with invalid checksum */
	DHT_VALID_CHECKSUM,     /**< Data received with valid checksum (alternate use) */
	DHT_FAILURE             /**< Generic failure */
} DHTResult;

/**
 * @brief Creates and initializes a new DHT instance.
 *
 * @param _pin GPIO pin number.
 * @param _port GPIO port.
 * @param _timer Timer handle for timing operations.
 * @return Pointer to a DHT structure.
 */
DHT* DHT_Create(uint16_t _pin, GPIO_TypeDef *_port, TIM_HandleTypeDef* _timer);

/**
 * @brief Reads temperature and humidity data from the DHT sensor.
 *
 * This function triggers the sensor, waits for response, reads the signal,
 * parses the data, and validates the checksum.
 *
 * @param _dht Pointer to a DHT sensor instance.
 * @param _data Output structure for the sensor reading.
 * @return Result code indicating success or failure reason.
 */
DHTResult DHT_ReadData(DHT* _dht, DHTData* _data);

#endif /* INC_DHT_H_ */
