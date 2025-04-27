/*
 * sampler.c
 *
 *  Created on: Apr 1, 2025
 *      Author: HOME
 */

#include "sampler.h"
#include "dht.h"
#include "config.h"
#include "date_time.h"

#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "task.h"
#include "freertos.h"
#include "semphr.h"
#include <stdio.h>

typedef struct {

    uint32_t m_ldr_val;
    uint32_t m_volt_val;

} ADCData;


static DHTData 			 s_dht_data;
static Sample_t 		 s_latest_sample;
static int 				 g_sample_delay = SAMPLE_DELAY_NORMAL;
static SemaphoreHandle_t s_sample_mutex;
static SemaphoreHandle_t s_delay_mutex;

extern osThreadId_t 	 samplerTaskHandle;
extern osThreadId_t 	 dhtTaskHandle;
extern osThreadId_t 	 controllerTaskHandle;


/* --- Forward Declarations --- */

void static ADC_Sample(ADC_HandleTypeDef * _hadc, uint32_t _delay, uint32_t* _pval);

/* --- Public Functions --- */

void DHT_Task(void* _arg)
{
	DHT* dht = DHT_Create(DHT_PIN, DHT_PORT, DHT_TIMER_ADDR);

	for (;;) {

		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

		while(DHT_ReadData(dht, &s_dht_data) != DHT_VALID_CHECKSUM){};

		xTaskNotifyGive(samplerTaskHandle);

	}
}


void Sampler_Task(void* _arg)
{

	ADCData adc_data;

	s_sample_mutex = xSemaphoreCreateMutex();
	s_delay_mutex = xSemaphoreCreateMutex();


	for (;;) {

		ADC_Sample(LDR_ADC_ADDR, HAL_MAX_DELAY, &adc_data.m_ldr_val);
		ADC_Sample(POT_ADC_ADDR, HAL_MAX_DELAY, &adc_data.m_volt_val);

		xTaskNotifyGive(dhtTaskHandle);
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

		Sample_t new_sample;

		DateTime_FormatString(new_sample.m_timestamp, sizeof(new_sample.m_timestamp));
		new_sample.m_ldr = (adc_data.m_ldr_val * 100) / 4095;
		new_sample.m_voltage = adc_data.m_volt_val;
		new_sample.m_temp_int = s_dht_data.m_temp_int;
		new_sample.m_temp_frac = s_dht_data.m_temp_frac;
		new_sample.m_hum_int = s_dht_data.m_humidity_int;
		new_sample.m_hum_frac = s_dht_data.m_humidity_frac;


		if (xSemaphoreTake(s_sample_mutex, pdMS_TO_TICKS(10)) == pdTRUE) {
			s_latest_sample = new_sample;
			xSemaphoreGive(s_sample_mutex);
		}

		xTaskNotifyGive(controllerTaskHandle);

		vTaskDelay(g_sample_delay);

	}
}


int Sampler_Get_Latest(Sample_t* _out) {
    if (xSemaphoreTake(s_sample_mutex, pdMS_TO_TICKS(10)) == pdTRUE) {
        *_out = s_latest_sample;
        xSemaphoreGive(s_sample_mutex);
        return 1;
    }
    return 0;
}


void Sampler_SetDelay(int _delay_ms)
{
        g_sample_delay = _delay_ms;
}


int Sampler_GetDelay()
{
    int val = 0;
    if (xSemaphoreTake(s_delay_mutex, pdMS_TO_TICKS(10)) == pdTRUE) {
        val = g_sample_delay;
        xSemaphoreGive(s_delay_mutex);
    }
    return val;
}

/* --- Static Helper Functions --- */

void static ADC_Sample(ADC_HandleTypeDef * _hadc, uint32_t _delay, uint32_t* _pval)
{
	HAL_ADC_Start(_hadc);
	while (HAL_ADC_PollForConversion(_hadc, _delay) != HAL_OK) {};
	*_pval = HAL_ADC_GetValue(_hadc);
	HAL_ADC_Stop(_hadc);

}
