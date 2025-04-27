/*
 * controller.c
 *
 *  Created on: Apr 2, 2025
 *      Author: HOME
 */


#include "controller.h"
#include "config.h"
#include "logger.h"
#include "sampler.h"
#include "event_manager.h"
#include "config_module.h"
#include "config.h"
#include "communicator.h"

#include "FreeRTOS.h"
#include "task.h"
#include "freertos.h"
#include "cmsis_os2.h"
#include <stdio.h>

#include <string.h>

extern osThreadId_t loggerFeederTaskHandle;
extern QueueHandle_t g_comm_event_queue;
extern EventGroupHandle_t g_comm_event_flags;

SystemMode_t s_current_mode = SYSTEM_MODE_NORMAL;

/* --- Forward Declarations --- */

static SystemMode_t EvaluateSampleMode(Sample_t const* _sample, Config_t const* _cfg);

/* --- Public Functions --- */

void Controller_Task(void* _arg)
{
    Sample_t sample;

    for (;;) {

        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        if (!Sampler_Get_Latest(&sample)) {
            printf("[Controller] Failed to retrieve latest sample\r\n");
            continue;
        }

        Config_t curr_cfg;
        Config_Get(&curr_cfg);

        SystemMode_t new_mode = EvaluateSampleMode(&sample, &curr_cfg);
        if (new_mode != s_current_mode) {

        	Event_Type_t event_type = EVENT_TYPE_INFO;
        	switch (new_mode) {
				case SYSTEM_MODE_ERROR:
					event_type = EVENT_TYPE_ENTER_ERROR;
					break;
				case SYSTEM_MODE_NORMAL:
					Sampler_SetDelay(SAMPLE_DELAY_NORMAL);
					event_type = EVENT_TYPE_ENTER_NORMAL;
					break;
				case SYSTEM_MODE_SAFE:
					Sampler_SetDelay(SAMPLE_DELAY_SAFE);
					event_type = EVENT_TYPE_ENTER_SAFE;
					break;
				default:
					break;
			}


        	char msg[EVENT_DESCRIPTION_MAX];
        	snprintf(msg, sizeof(msg),
        	         "System mode changed to %s (T=%u.%u°C, H=%u.%u%%, V=%lu, LDR=%lu)",
        	         (new_mode == SYSTEM_MODE_NORMAL) ? "NORMAL" :
        	         (new_mode == SYSTEM_MODE_ERROR) ? "ERROR" : "SAFE",
        	         sample.m_temp_int, sample.m_temp_frac,
        	         sample.m_hum_int, sample.m_hum_frac,
        	         sample.m_voltage, sample.m_ldr);

        	EventManager_ReportString(EVENT_SOURCE_MONITORING, event_type, msg);

        	s_current_mode = new_mode;

			CommunicatorMessage_t cm;
			cm.packet_id = PROTO_PKT_EVENT;
			strncpy((char*)cm.payload, msg, sizeof(cm.payload));
			cm.payload_len = strlen(msg);
			xQueueSend(g_comm_event_queue, &cm, pdMS_TO_TICKS(50));
			osEventFlagsSet(g_comm_event_flags, COMM_EVT_EVENT);
        }

        xTaskNotifyGive(loggerFeederTaskHandle);
    }
}

/* --- Static Helper Functions --- */

static SystemMode_t EvaluateSampleMode(Sample_t const* _sample, Config_t const* _cfg)
{
    if (!_sample || !_cfg) {
        return SYSTEM_MODE_ERROR;
    }

    if (_sample->m_voltage < _cfg->m_voltage_min) {
        return SYSTEM_MODE_SAFE;
    }

    if (_sample->m_temp_int > _cfg->m_temp_max) {
        return SYSTEM_MODE_ERROR;
    }

    if (_sample->m_hum_int < _cfg->m_hum_min) {
        return SYSTEM_MODE_ERROR;
    }

    if (_sample->m_ldr < _cfg->m_ldr_min) {
        return SYSTEM_MODE_ERROR;
    }

    return SYSTEM_MODE_NORMAL;
}

