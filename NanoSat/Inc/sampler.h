/**
 * @file sampler.h
 * @brief Handles sampling of sensor data and storage of latest measurements.
 *
 * This module manages sensor sampling, including DHT temperature/humidity
 * and ADC-based LDR and voltage readings. It maintains the latest sample,
 * provides access to it, and manages sample timing via configurable delay.
 */

#ifndef INC_SAMPLER_H_
#define INC_SAMPLER_H_

#include "config.h"

/**
 * @brief Structure representing a single environmental sample.
 */
typedef struct {
    char m_timestamp[DATE_TIME_TIMESTAMP_LEN]; /**< Timestamp of sample (formatted string) */
    uint32_t m_ldr;        /**< Light sensor (LDR) raw value */
    uint32_t m_voltage;    /**< Voltage reading (e.g., from potentiometer) */
    uint8_t m_temp_int;    /**< Integer part of temperature (°C) */
    uint8_t m_temp_frac;   /**< Fractional part of temperature */
    uint8_t m_hum_int;     /**< Integer part of humidity (%) */
    uint8_t m_hum_frac;    /**< Fractional part of humidity */
} Sample_t;

/**
 * @brief RTOS task responsible for sampling all environmental sensors.
 *
 * Periodically samples ADC sensors and coordinates with the DHT task
 * to gather a complete environmental sample. Stores the most recent sample.
 *
 * @param _arg Unused task argument.
 */
void Sampler_Task(void* _arg);

/**
 * @brief RTOS task responsible for reading the DHT sensor.
 *
 * Waits for notification from the sampler task, reads DHT values,
 * and then notifies the sampler task when done.
 *
 * @param _arg Unused task argument.
 */
void DHT_Task(void* _arg);

/**
 * @brief Retrieves the most recently captured sample.
 *
 * Thread-safe access to the latest stored sample.
 *
 * @param _out Pointer to a Sample_t structure to receive the latest sample.
 * @return 1 on success, 0 on failure (e.g., mutex unavailable).
 */
int Sampler_Get_Latest(Sample_t* _out);

/**
 * @brief Sets the sampling delay interval.
 *
 * Defines how often new samples are taken by the sampler task.
 *
 * @param _delay_ms Delay in milliseconds.
 */
void Sampler_SetDelay(int _delay_ms);

/**
 * @brief Retrieves the current sampling delay.
 *
 * @return Sampling delay in milliseconds.
 */
int Sampler_GetDelay();

#endif /* INC_SAMPLER_H_ */
