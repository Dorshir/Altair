/*
 * keep_alive.c
 *
 *  Created on: Apr 6, 2025
 *      Author: HOME
 */


#include "communicator.h"
#include "keep_alive.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os2.h"

extern osEventFlagsId_t 	g_comm_event_flags;
extern TaskHandle_t 		g_communicator_task_handle;

/* --- Public Functions --- */

void KeepAlive_Trigger()
{
    if (g_communicator_task_handle) {
        vTaskPrioritySet(g_communicator_task_handle, configMAX_PRIORITIES - 1);
    }
    osEventFlagsSet(g_comm_event_flags, COMM_EVT_KEEP_ALIVE);
}
