/*
 * init_sys.c
 *
 *  Created on: Apr 21, 2025
 *      Author: HOME
 */


#include "init_sys.h"
#include "communicator.h"
#include "config_module.h"
#include "keep_alive.h"
#include "config.h"
#include "freertos.h"
#include "logger.h"
#include "logger_feeder.h"
#include "controller.h"
#include "sampler.h"
#include "date_time.h"
#include "event_manager.h"
#include "rtc.h"

#include "cmsis_os2.h"

#include <stdio.h>

extern osThreadId_t loggerTaskHandle;
extern osThreadId_t loggerFeederTaskHandle;
extern osThreadId_t controllerTaskHandle;
extern osThreadId_t dhtTaskHandle;
extern osThreadId_t samplerTaskHandle;
extern osThreadId_t eventManagerTaskHandle;
extern osThreadId_t notifyRxTaskHandle;

extern osThreadAttr_t loggerTask_attributes;
extern osThreadAttr_t notifyRxTask_attributes;
extern osThreadAttr_t eventManagerTask_attributes;
extern osThreadAttr_t samplerTask_attributes;
extern osThreadAttr_t dhtTask_attributes;
extern osThreadAttr_t controllerTask_attributes;
extern osThreadAttr_t loggerFeederTask_attributes;

extern TaskHandle_t notifyRxTaskFreeRTOSHandle;

osTimerId_t sw_timer_keepalive;


/* --- Public Functions --- */

void Init_Task(void *arg)
{

	Communicator_Init();

	ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    Config_Init();

    sw_timer_keepalive = osTimerNew(KeepAlive_Trigger,
                                    osTimerPeriodic, NULL, NULL);
    osTimerStart(sw_timer_keepalive, BEACON_MS);

    /* Create the rest of your application tasks */
    loggerTaskHandle       = osThreadNew(Logger_Task,       NULL, &loggerTask_attributes);
    loggerFeederTaskHandle = osThreadNew(LoggerFeeder_Task, NULL, &loggerFeederTask_attributes);
    controllerTaskHandle   = osThreadNew(Controller_Task,   NULL, &controllerTask_attributes);
    dhtTaskHandle          = osThreadNew(DHT_Task,          NULL, &dhtTask_attributes);
    samplerTaskHandle      = osThreadNew(Sampler_Task,      NULL, &samplerTask_attributes);
    eventManagerTaskHandle = osThreadNew(EventManager_Task, NULL, &eventManagerTask_attributes);

    /*  Mission accomplished – this task can retire */
    vTaskDelete(NULL);
    for (;;) {}
}

