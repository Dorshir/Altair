/*
 * flash_storage.c
 *
 *  Created on: Apr 2, 2025
 *      Author: HOME
 */

#include "flash_storage.h"
#include "stm32l4xx_hal.h"
#include "config.h"
#include <string.h>

#define CONFIG_FLASH_MAGIC 0xA5A5A5A5

typedef struct {

	uint64_t m_magic;
    uint8_t m_data[256];

} FlashBlock_t;

/* --- Public Functions --- */

int Flash_ReadConfig(uint32_t _flash_addr, uint8_t* _dest, uint32_t _len)
{
    FlashBlock_t* block = (FlashBlock_t*)_flash_addr;

    if (block->m_magic != CONFIG_FLASH_MAGIC)
        return 0;

    memcpy(_dest, block->m_data, _len);
    return 1;
}


int Flash_WriteConfig(uint32_t _flash_addr, uint32_t _bank, uint32_t _page, uint8_t* _data, uint32_t _len)
{
    HAL_FLASH_Unlock();

    FLASH_EraseInitTypeDef erase = {
        .TypeErase = FLASH_TYPEERASE_PAGES,
        .Banks     = _bank,
        .Page      = _page,
        .NbPages   = 1
    };

    uint32_t error;
    if (HAL_FLASHEx_Erase(&erase, &error) != HAL_OK) {
        HAL_FLASH_Lock();
        return 0;
    }

    uint64_t magic64 = CONFIG_FLASH_MAGIC;
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, _flash_addr, magic64) != HAL_OK) {
        HAL_FLASH_Lock();
        return 0;
    }

    _flash_addr += sizeof(uint64_t);

    for (uint32_t i = 0; i < _len; i += 8) {
        uint64_t chunk = 0xFFFFFFFFFFFFFFFF;

        memcpy(&chunk, _data + i, (_len - i >= 8) ? 8 : (_len - i));

        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, _flash_addr, chunk) != HAL_OK) {
            HAL_FLASH_Lock();
            return 0;
        }

        _flash_addr += 8;
    }

    HAL_FLASH_Lock();
    return 1;
}


