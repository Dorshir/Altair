/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "logger.h"
#include "logger_feeder.h"
#include "controller.h"
#include "sampler.h"
#include "date_time.h"
#include "event_manager.h"
#include "config_module.h"
#include "communicator.h"
#include "init_sys.h"


#include "ff.h"
#include "rtc.h"
#include <stdio.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

osThreadId_t initTaskHandle;
const osThreadAttr_t initTask_attributes = {
  .name = "initTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

osThreadId_t samplerTaskHandle;
const osThreadAttr_t samplerTask_attributes = {
  .name = "samplerTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

osThreadId_t notifyRxTaskHandle;
const osThreadAttr_t notifyRxTask_attributes = {
  .name = "notifyRxTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

osThreadId_t dhtTaskHandle;
const osThreadAttr_t dhtTask_attributes = {
  .name = "dhtTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};

osThreadId_t controllerTaskHandle;
const osThreadAttr_t controllerTask_attributes = {
  .name = "controllerTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

osThreadId_t loggerFeederTaskHandle;
const osThreadAttr_t loggerFeederTask_attributes = {
  .name = "loggerFeederTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

osThreadId_t loggerTaskHandle;
const osThreadAttr_t loggerTask_attributes = {
  .name = "loggerTask",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

osThreadId_t eventManagerTaskHandle;
const osThreadAttr_t eventManagerTask_attributes = {
  .name = "eventManagerTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

osThreadId_t communicatorTaskHandle;
const osThreadAttr_t communicatorTask_attributes = {
  .name = "communicatorTask",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};


TaskHandle_t notifyRxTaskFreeRTOSHandle;
TaskHandle_t g_init_task_handle;

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

extern osEventFlagsId_t g_comm_event_flags;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

void NotifyRxTask(void * arg)
{
	notifyRxTaskFreeRTOSHandle = xTaskGetCurrentTaskHandle();

	for (;;) {
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		osEventFlagsSet(g_comm_event_flags, COMM_EVT_SAMPLE);
	}
}

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */

  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */

  initTaskHandle = osThreadNew(Init_Task, NULL, &initTask_attributes);

  notifyRxTaskHandle = osThreadNew(NotifyRxTask, NULL, &notifyRxTask_attributes);

  g_init_task_handle = (TaskHandle_t)initTaskHandle;

  communicatorTaskHandle = osThreadNew(Communicator_Task, NULL, &communicatorTask_attributes);

  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

