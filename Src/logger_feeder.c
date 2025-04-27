/*
 * logger_feeder.c
 *
 *  Created on: Apr 2, 2025
 *      Author: Dor Shir
 */

#include "logger_feeder.h"
#include "sampler.h"
#include "logger.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "freertos.h"
#include <stdio.h>

extern QueueHandle_t g_logger_cmd_queue;

/* --- Public Functions --- */

void LoggerFeeder_Task(void* _arg)
{
    Sample_t sample;
    Logger_Command_t cmd;

    for (;;) {

        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        if (!Sampler_Get_Latest(&sample)) {
            printf("[LoggerFeeder] Failed to get sample\n");
            continue;
        }

        cmd.m_type = LOGGER_CMD_WRITE;

        snprintf(cmd.m_payload, sizeof(cmd.m_payload),
                 "%s,%u.%u,%u.%u,%lu,%lu\n",
                 sample.m_timestamp,
                 sample.m_temp_int, sample.m_temp_frac,
                 sample.m_hum_int, sample.m_hum_frac,
                 sample.m_voltage,
                 sample.m_ldr);


        if (xQueueSend(g_logger_cmd_queue, &cmd, pdMS_TO_TICKS(10)) != pdPASS) {
            printf("[LoggerFeeder] Failed to enqueue log command\r\n");
        }
    }
}
