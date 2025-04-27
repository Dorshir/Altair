/*
 * config_module.c
 *
 *  Created on: Apr 2, 2025
 *      Author: HOME
 */


#include "config_module.h"
#include "flash_storage.h"
#include "config.h"

#include <string.h>

Config_t g_current_config;


/* --- Forward Declarations --- */

static void Config_Init_Defaults();

/* --- Public Functions --- */

void Config_Init(void)
{
    if (!Config_LoadFromFlash()) {
        Config_SetDefaults();
    }
}



void Config_Get(Config_t* out)
{
    if (out) {
    	*out = g_current_config;
    }
}


void Config_Set(const Config_t* in)
{
    if (in) {
    	g_current_config = *in;
        Config_SaveToFlash();
    }
}


void Config_SetDefaults(void)
{
	Config_Init_Defaults();
    Config_SaveToFlash();
}


int Config_SaveToFlash(void)
{
    return Flash_WriteConfig(CONFIG_FLASH_ADDR, CONFIG_FLASH_BANK, CONFIG_FLASH_PAGE, (uint8_t*)&g_current_config, sizeof(Config_t));
}


int Config_LoadFromFlash(void)
{
    return Flash_ReadConfig(CONFIG_FLASH_ADDR, (uint8_t*)&g_current_config, sizeof(Config_t));
}


/* Static Helper Functions */

static void Config_Init_Defaults()
{
	g_current_config.m_temp_max = CONFIG_TEMP_MAX;
	g_current_config.m_hum_min = CONFIG_HUM_MIN;
	g_current_config.m_ldr_min = CONFIG_LDR_MIN;
	g_current_config.m_voltage_min = CONFIG_VOLT_MIN;
}
