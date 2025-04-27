/*
 * communicator.c
 *
 *  Created on: Apr 6, 2025
 *      Author: HOME
 */

#include "communicator.h"
#include "protocol.h"
#include "uart.h"
#include "logger.h"
#include "sampler.h"
#include "date_time.h"

#include "ff.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "event_groups.h"
#include "cmsis_os2.h"
#include "rtc.h"
#include "date_time.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define UART_RX_BUFFER_SIZE  1    // We'll receive 1 byte at a time
#define UART_RX_QUEUE_LEN    128

osEventFlagsId_t 		g_comm_event_flags;
QueueHandle_t 			g_comm_event_queue;
QueueHandle_t 			g_sample_tx_queue;
TaskHandle_t 			g_communicator_task_handle = NULL;

static QueueHandle_t 	s_sample_rx_queue;
static uint8_t 			s_uart_rx_buf[UART_RX_BUFFER_SIZE] = {0};

static uint8_t  		s_rx_packet_buf[PROTO_MAX_PACKET_LEN];
static uint16_t 		s_rx_packet_index = 0;
static uint16_t 		s_rx_expected_len = 0;
static uint16_t 		G_INDEX = 0;

extern QueueHandle_t 	g_logger_cmd_queue;
extern TaskHandle_t 	notifyRxTaskFreeRTOSHandle;
extern SystemMode_t 	s_current_mode;
extern osThreadId_t 	initTaskHandle;

/* --- Forward Declarations --- */

static void Communicator_Send(uint8_t _packet_id, uint8_t const* _payload, uint8_t _payload_len);
static void Communicator_SendKeepAliveText();
static const char* mode_to_string(SystemMode_t mode);

/* --- Public Functions --- */

void Communicator_Init()
{
    g_comm_event_flags = osEventFlagsNew(NULL);

    g_comm_event_queue = xQueueCreate(COMM_QUEUE_LEN, sizeof(CommunicatorMessage_t));
    g_sample_tx_queue = xQueueCreate(COMM_QUEUE_LEN, sizeof(CommunicatorMessage_t));
    s_sample_rx_queue = xQueueCreate(COMM_QUEUE_LEN, sizeof(CommunicatorRawPacket_t));

    UART_Init();

    __HAL_UART_FLUSH_DRREGISTER(&huart2); // Clear UART2 data register
    __HAL_UART_CLEAR_OREFLAG(&huart2);    // Clear overrun flag

}

void Communicator_Task(void* _arg)
{
	HAL_UART_Receive_IT(&huart2, s_uart_rx_buf, UART_RX_BUFFER_SIZE);
    g_communicator_task_handle = xTaskGetCurrentTaskHandle();

    DateTime* dt = DateTime_Create(&hrtc);

    for (;;) {

        // --- Wait for any kind of event ---
        uint32_t bits = osEventFlagsWait(
            g_comm_event_flags,
            COMM_EVENT_GROUP_FLAGS,
            osFlagsWaitAny,
            osWaitForever
        );

        // --- HIGH PRIORITY: Keep Alive ---
        if (bits & COMM_EVT_KEEP_ALIVE) {
			Communicator_SendKeepAliveText();
			vTaskPrioritySet(g_communicator_task_handle, osPriorityHigh);
		}

        // --- MEDIUM PRIORITY: Event Messages ---
        if ((bits & COMM_EVT_EVENT) || uxQueueMessagesWaiting(g_comm_event_queue) > 0) {
            CommunicatorMessage_t msg;
            if (xQueueReceive(g_comm_event_queue, &msg, 0) == pdTRUE) {
                Communicator_Send(msg.packet_id, msg.payload, msg.payload_len);

                if (uxQueueMessagesWaiting(g_comm_event_queue) > 0) {
                	osEventFlagsSet(g_comm_event_flags, COMM_EVT_EVENT);
                }
            }
        }

        // --- LOW PRIORITY: NEW SAMPLE REQUEST ---
        if (bits & COMM_EVT_SAMPLE) {

            ProtocolPacket_t unpacked;
            int unpack_res = Protocol_Unpack(s_rx_packet_buf, G_INDEX, &unpacked);

            G_INDEX = 0;

            if ((unpack_res > 0)) {

                switch (unpacked.packet_id) {

                case PROTO_PKT_SAMPLE: {
                    if (g_logger_cmd_queue) {
                        Logger_Command_t cmd;
                        cmd.m_type = LOGGER_CMD_SAMPLE_REQUEST;

                        memcpy(cmd.m_payload, unpacked.payload, unpacked.payload_len);
                        cmd.m_payload[unpacked.payload_len] = '\0';

                        if (xQueueSend(g_logger_cmd_queue, &cmd, pdMS_TO_TICKS(100)) != pdTRUE) {
                            printf("[Comm] Failed to enqueue sample request to logger\n");
                        }
                    }
                    break;
                }

                case PROTO_PKT_TIME_SYNC: {

					const char* p = (const char*)unpacked.payload;

					int year   = (p[0]-'0')*1000 + (p[1]-'0')*100 + (p[2]-'0')*10 + (p[3]-'0');
					int month  = (p[4]-'0')*10   + (p[5]-'0');
					int day    = (p[6]-'0')*10   + (p[7]-'0');
					int hour   = (p[8]-'0')*10   + (p[9]-'0');
					int minute = (p[10]-'0')*10  + (p[11]-'0');
					int second = (p[12]-'0')*10  + (p[13]-'0');

					Date date = {
						.m_year     = (uint8_t)(year - 2000),
						.m_month    = month,
						.m_day      = day,
						.m_week_day = 2
					};

					Time time = {
						.m_hour     = hour,
						.m_minute   = minute,
						.m_seconds  = second
					};

					DateTime_SetDate(dt, &date);
					DateTime_SetTime(dt, &time);

					if (initTaskHandle) {
						xTaskNotifyGive((TaskHandle_t)initTaskHandle);
					}
					break;
				}

                default:
                	break;
                }
            }

            s_rx_packet_index = 0;
            s_rx_expected_len = 0;
        }


        // --- LOW PRIORITY: SAMPLE RESPONSES TO SEND ---
        if (bits & COMM_EVT_SAMPLE_READY) {

            CommunicatorMessage_t response_msg;
            while (xQueueReceive(g_sample_tx_queue, &response_msg, 0) == pdTRUE) {
                Communicator_Send(response_msg.packet_id, response_msg.payload, response_msg.payload_len);
            }

            if (uxQueueMessagesWaiting(g_sample_tx_queue) > 0) {
            	osEventFlagsSet(g_comm_event_flags, COMM_EVT_SAMPLE_READY);
            }
        }
    }
}



void Communicator_SendKeepAlive()
{
    if (g_communicator_task_handle) {
        vTaskPrioritySet(g_communicator_task_handle, osPriorityNormal);
    }

    osEventFlagsSet(g_comm_event_flags, COMM_EVT_KEEP_ALIVE);
}

/* --- Static Helper Functions --- */

static const char* mode_to_string(SystemMode_t mode) {
    switch (mode) {
    case SYSTEM_MODE_NORMAL:  return "NORMAL";
    case SYSTEM_MODE_ERROR:   return "ERROR";
    case SYSTEM_MODE_SAFE:    return "SAFE";
    case EVENT_TYPE_RECOVER:  return "RECOVER";
    default:                  return "UNKNOWN";
    }
}

static void Communicator_SendKeepAliveText(void)
{
    Sample_t sample;
    char line[160];

    if (Sampler_Get_Latest(&sample) == 1) {
        // Format: "YYYY-MM-DD HH:MM:SS MODE:<MODE>, LDR:<lux>, VBAT:<mV>, T:<int>.<frac>C, H:<int>.<frac>%"
        int len = snprintf(line, sizeof(line),
            "%s MODE:%s, LDR:%lu, VBAT:%lu, T:%u.%uC, H:%u.%u%%\r\n",
			sample.m_timestamp,
            mode_to_string(s_current_mode),
            (unsigned long)sample.m_ldr,
            (unsigned long)sample.m_voltage,
            sample.m_temp_int, sample.m_temp_frac,
            sample.m_hum_int,  sample.m_hum_frac
        );
        if (len < 0) len = 0;
        else if (len >= (int)sizeof(line)) len = sizeof(line)-1;
        line[len] = '\0';
    }
    else {
        // Fallback: no sample available
        int len = snprintf(line, sizeof(line),
            "%s MODE:%s, no sample\r\n",
			sample.m_timestamp,
            mode_to_string(s_current_mode)
        );
        if (len < 0) len = 0;
        else if (len >= (int)sizeof(line)) len = sizeof(line)-1;
        line[len] = '\0';
    }

    HAL_GPIO_TogglePin(Red_Led_GPIO_Port, Red_Led_Pin);

    Communicator_Send(
        PROTO_PKT_KEEP_ALIVE,
        (uint8_t*)line,
        (uint8_t)strlen(line)
    );
}


static void Communicator_Send(uint8_t _packet_id, uint8_t const* _payload, uint8_t _payload_len)
{
    uint8_t buffer[PROTO_MAX_PACKET_LEN];
    uint16_t len = Protocol_Pack(_packet_id, _payload, _payload_len, buffer);
    if (len > 0) {
        UART_SendBytes(buffer, len);
    }
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2)
    {
        /* Rearm the reception immediately */
        if (HAL_UART_Receive_IT(&huart2, s_uart_rx_buf, UART_RX_BUFFER_SIZE) != HAL_OK)
            return;

        uint8_t rx_byte = s_uart_rx_buf[0];

        /* ---- First byte: length field ---- */
        if (s_rx_packet_index == 0) {
            /* Length sanity‑check */
            if (rx_byte == 0 || rx_byte + 3 > PROTO_MAX_PACKET_LEN) {
                /* Discard – keeps us inside the buffer */
                return;
            }

            s_rx_expected_len      = rx_byte + 3;
            s_rx_packet_buf[0]     = rx_byte;
            s_rx_packet_index      = 1;
            return;     /* Done for this byte */
        }

        /* ---- All subsequent bytes ---- */
        if (s_rx_packet_index >= PROTO_MAX_PACKET_LEN) {
            /* Defensive overflow guard */
            s_rx_packet_index = 0;
            return;
        }

        s_rx_packet_buf[s_rx_packet_index++] = rx_byte;

        /* ---- Packet complete? ---- */
        if (s_rx_packet_index == s_rx_expected_len) {
            if (s_rx_packet_buf[s_rx_packet_index - 1] == PROTO_END_BYTE) {
            	G_INDEX = s_rx_packet_index;
                BaseType_t xHigherPriorityTaskWoken = pdFALSE;
                vTaskNotifyGiveFromISR(notifyRxTaskFreeRTOSHandle, &xHigherPriorityTaskWoken);
                portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
            }
            /* Always reset for the next packet */
            s_rx_packet_index = 0;
        }
    }
}

