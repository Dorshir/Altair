/*
 * dht.c
 *
 *  Created on: Mar 19, 2025
 *      Author: HOME
 */

#include "dht.h"
#include "usart.h"
#include "tim.h"
#include <stdio.h>

#define BIT_DECISION_TRESHOLD 50
#define MCU_START_SIGNAL_PERIOD 18000
#define DHT_START_RESPONSE_PERIOD 40
#define DHT_RESPONSE_PERIOD 80


static struct DHT s_dht = {0};
static uint8_t dht_initialized = 0;


/* --- Forward Declarations --- */

static void DHT_Set_As_Input();
static void DHT_Set_As_Output();
static void Delay_us(uint16_t us);
static uint8_t DHT_ReadBit(DHT* _dht);
static uint8_t DHT11_ReadByte(DHT* _dht);

/* --- Public Functions --- */

DHT* DHT_Create(uint16_t _pin, GPIO_TypeDef *_port, TIM_HandleTypeDef *_timer) {

	if (dht_initialized) {
		return NULL;
	}

	s_dht.m_GPIO_Pin = _pin;
	s_dht.m_GPIOx = _port;
	s_dht.m_timer = _timer;

	dht_initialized = 1;

	return &s_dht;
}

static void DHT_Start() {

	DHT_Set_As_Output();

	HAL_GPIO_WritePin(s_dht.m_GPIOx, s_dht.m_GPIO_Pin, GPIO_PIN_RESET);
	Delay_us(MCU_START_SIGNAL_PERIOD);

	HAL_GPIO_WritePin(s_dht.m_GPIOx, s_dht.m_GPIO_Pin, GPIO_PIN_SET);
	Delay_us(DHT_START_RESPONSE_PERIOD);

	DHT_Set_As_Input();
}


DHTResult DHT_ReadData(DHT* _dht, DHTData* _data) {

	uint8_t rh_int, rh_dec, temp_int, temp_dec, checksum;

    if (_dht == NULL || _data == NULL) {
        return DHT_NULLPTR_ERROR;
    }

	DHT_Start();

    while (HAL_GPIO_ReadPin(_dht->m_GPIOx, _dht->m_GPIO_Pin) == GPIO_PIN_RESET){};

    uint32_t timeout = 100;
	while (HAL_GPIO_ReadPin(_dht->m_GPIOx, _dht->m_GPIO_Pin) == GPIO_PIN_SET && timeout > 0) {
		Delay_us(1);
		timeout--;
	}
	if (timeout == 0) {
		return 0;
	}

	rh_int = DHT11_ReadByte(_dht);
	rh_dec = DHT11_ReadByte(_dht);
	temp_int = DHT11_ReadByte(_dht);
	temp_dec = DHT11_ReadByte(_dht);
	checksum = DHT11_ReadByte(_dht);

	if (checksum == (rh_int + rh_dec + temp_int + temp_dec)) {

		_data->m_humidity_int = rh_int;
		_data->m_humidity_frac = rh_dec;
		_data->m_temp_int = temp_int;
		_data->m_temp_frac = temp_dec;
		_data->m_checksum = checksum;

		return DHT_VALID_CHECKSUM;
	} else {
		return DHT_INVALID_CHECKSUM;
	}

	return DHT_FAILURE;
}


/* --- Static Helper Functions --- */

static uint8_t DHT_ReadBit(DHT* _dht) {

    while (HAL_GPIO_ReadPin(_dht->m_GPIOx, _dht->m_GPIO_Pin) == GPIO_PIN_RESET);
    Delay_us(BIT_DECISION_TRESHOLD);
    return (HAL_GPIO_ReadPin(_dht->m_GPIOx, _dht->m_GPIO_Pin) == GPIO_PIN_SET) ? 1 : 0;
}


static uint8_t DHT11_ReadByte(DHT* _dht) {

    uint8_t value = 0;
    for (int i = 0; i < 8; i++) {
        value <<= 1;
        value |= DHT_ReadBit(_dht);

        uint32_t timeout = 100;
        while (HAL_GPIO_ReadPin(_dht->m_GPIOx, _dht->m_GPIO_Pin) == GPIO_PIN_SET && timeout > 0) {
        	Delay_us(1);
            timeout--;
        }
        if (timeout == 0) {
            return 0;
        }
    }
    return value;
}


static void Delay_us(uint16_t us) {

    HAL_TIM_Base_Start(s_dht.m_timer);
    __HAL_TIM_SET_COUNTER(s_dht.m_timer, 0);
    while (__HAL_TIM_GET_COUNTER(s_dht.m_timer) < us);
    HAL_TIM_Base_Stop(s_dht.m_timer);
}


static void DHT_Set_As_Input() {

	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = s_dht.m_GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(s_dht.m_GPIOx, &GPIO_InitStruct);
}


static void DHT_Set_As_Output() {

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = s_dht.m_GPIO_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(s_dht.m_GPIOx, &GPIO_InitStruct);
}



