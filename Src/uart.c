#include "uart.h"
#include "cmsis_os.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "semphr.h"

#define UART_TIMEOUT_MS 100

static SemaphoreHandle_t s_uart_tx_done_sem;

/* --- Public Functions --- */

void UART_Init()
{
    s_uart_tx_done_sem = xSemaphoreCreateBinary();
    xSemaphoreGive(s_uart_tx_done_sem);
}

void UART_SendBytes(uint8_t const* _data, uint16_t _len)
{
    if (!_data || _len == 0) return;

    if (xSemaphoreTake(s_uart_tx_done_sem, pdMS_TO_TICKS(UART_TIMEOUT_MS)) == pdTRUE) {
        if (HAL_UART_Transmit_IT(&huart2, (uint8_t*)_data, _len) != HAL_OK) {
            xSemaphoreGive(s_uart_tx_done_sem);
        }
    }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart == &huart2 && s_uart_tx_done_sem) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xSemaphoreGiveFromISR(s_uart_tx_done_sem, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}
