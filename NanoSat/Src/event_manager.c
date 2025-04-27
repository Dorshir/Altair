/*
 * event_manager.c
 *
 *  Created on: Apr 2, 2025
 *      Author: HOME
 */


#include "event_manager.h"
#include "logger.h"
#include "date_time.h"
#include "config.h"
#include "rgb.h"
#include "buzzer.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include <stdio.h>
#include <string.h>

extern QueueHandle_t g_logger_cmd_queue;

QueueHandle_t g_event_queue = NULL;

/* --- Public Functions --- */

int EventManager_ReportString(Event_Source_t _src, Event_Type_t _type, const char* _description)
{
    if (!g_event_queue) return 0;

    Event_t evt = {
        .m_source = _src,
        .m_type = _type
    };

    strncpy(evt.m_description, _description, sizeof(evt.m_description) - 1);
	evt.m_description[sizeof(evt.m_description) - 1] = '\0';

    return xQueueSend(g_event_queue, &evt, pdMS_TO_TICKS(10)) == pdPASS;
}


void EventManager_Task(void* _arg)
{
    Event_t evt;
    Logger_Command_t cmd;

    g_event_queue = xQueueCreate(EVENT_QUEUE_LENGTH, sizeof(Event_t));
    if (!g_event_queue) {
        printf("[EventManager] Failed to create event queue\r\n");
        vTaskSuspend(NULL);
    }

    RGB_SetColor(0, 1, 0);

    for (;;) {

        if (xQueueReceive(g_event_queue, &evt, portMAX_DELAY) == pdTRUE) {

            DateTime_FormatString(evt.m_timestamp, sizeof(evt.m_timestamp));

            switch (evt.m_type) {
                case EVENT_TYPE_ENTER_ERROR:
                	RGB_SetColor(1, 0, 0); // Red on
                    Buzzer_PlayAlert();
                    break;

                case EVENT_TYPE_ENTER_NORMAL:
                	RGB_SetColor(0, 1, 0); // Green on
                    Buzzer_Stop();
                    break;

                case EVENT_TYPE_ENTER_SAFE:
                	RGB_SetColor(1, 1, 1); // Yellow on
                    Buzzer_Stop();
                    break;

                default:
                    break;
            }

            cmd.m_type = LOGGER_CMD_WRITE_EVENT;
            snprintf(cmd.m_payload, sizeof(cmd.m_payload), "%s,EVENT,%s\r\n",
                     evt.m_timestamp, evt.m_description);

            xQueueSend(g_logger_cmd_queue, &cmd, pdMS_TO_TICKS(10));
            vTaskDelay(100);
        }
    }
}
