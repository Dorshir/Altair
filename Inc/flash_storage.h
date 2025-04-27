/**
 * @file flash_storage.h
 * @brief Interface for storing and retrieving configuration data in flash memory.
 *
 * Provides simple functions for writing to and reading from internal flash memory.
 * Used for persisting system configuration data across resets.
 */

#ifndef INC_FLASH_STORAGE_H_
#define INC_FLASH_STORAGE_H_

#include <stdint.h>

/**
 * @brief Writes data to internal flash memory.
 *
 * Flash memory must be erased before writing. This function handles unlocking,
 * erasing, and programming the specified flash region.
 *
 * @param _flash_addr Target flash address to write to.
 * @param _bank Flash memory bank number.
 * @param _page Flash memory page number.
 * @param _data Pointer to data buffer to write.
 * @param _len Length of data in bytes.
 * @return 1 on success, 0 on failure.
 */
int Flash_WriteConfig(uint32_t _flash_addr, uint32_t _bank, uint32_t _page, uint8_t* _data, uint32_t _len);

/**
 * @brief Reads data from internal flash memory.
 *
 * Copies bytes from flash memory into the provided destination buffer.
 *
 * @param _flash_addr Flash address to read from.
 * @param _dest Destination buffer.
 * @param _len Number of bytes to read.
 * @return 1 on success, 0 on failure.
 */
int Flash_ReadConfig(uint32_t _flash_addr, uint8_t* _dest, uint32_t _len);

#endif /* INC_FLASH_STORAGE_H_ */
