/**
 * @file config_module.h
 * @brief Configuration management module for system thresholds.
 *
 * Provides an interface for initializing, retrieving, updating,
 * and persisting system configuration parameters such as temperature,
 * humidity, light, and voltage thresholds.
 */

#ifndef INC_CONFIG_MODULE_H_
#define INC_CONFIG_MODULE_H_

#include <stdint.h>

/**
 * @brief Structure holding threshold configuration parameters.
 */
typedef struct {
    uint8_t  m_temp_max;     /**< Maximum temperature threshold */
    uint8_t  m_hum_min;      /**< Minimum humidity threshold */
    uint16_t m_ldr_min;      /**< Minimum light (LDR) threshold */
    uint32_t m_voltage_min;  /**< Minimum voltage threshold */
} Config_t;

/**
 * @brief Initializes the configuration module.
 *
 * Loads configuration from flash; if unavailable, applies defaults.
 */
void Config_Init(void);

/**
 * @brief Retrieves the current configuration.
 *
 * @param[out] out Pointer to a Config_t structure to store the current config.
 */
void Config_Get(Config_t* out);

/**
 * @brief Applies and saves a new configuration.
 *
 * @param[in] in Pointer to a Config_t structure containing new config values.
 */
void Config_Set(Config_t const* in);

/**
 * @brief Resets configuration to default values and saves to flash.
 */
void Config_SetDefaults(void);

/**
 * @brief Saves current configuration to flash memory.
 *
 * @return 1 on success, 0 on failure.
 */
int Config_SaveToFlash(void);

/**
 * @brief Loads configuration from flash memory.
 *
 * @return 1 on success, 0 on failure.
 */
int Config_LoadFromFlash(void);

#endif /* INC_CONFIG_MODULE_H_ */
