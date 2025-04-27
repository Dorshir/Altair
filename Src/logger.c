/*
 * logger.c
 *
 *  Created on: Apr 2, 2025
 *      Author: Dor Shir
 */

#include "FreeRTOS.h"
#include "logger.h"
#include "ff.h"
#include "config.h"
#include "communicator.h"
#include "queue.h"

#include "cmsis_os2.h"
#include "diskio.h"

#include <time.h>
#include <string.h>
#include <stdio.h>


QueueHandle_t g_logger_cmd_queue;

static osMutexId_t sdMutexHandle = NULL;

extern QueueHandle_t g_sample_tx_queue;
extern QueueHandle_t g_comm_event_queue;
extern EventGroupHandle_t g_comm_event_flags;

/* --- Forward Declarations --- */

static void KeepOnlyRecent(int _max_files);
static void Logger_InitFilesystemMutex();

/* --- Public Functions --- */

void Logger_Task(void* _arg)
{
    Logger_Command_t cmd;
    FIL file;
    FRESULT res;
    UINT bytes_written;

    // Create queue first
    g_logger_cmd_queue = xQueueCreate(LOGGER_QUEUE_LENGTH, sizeof(Logger_Command_t));
    if (!g_logger_cmd_queue) {
        printf("[Logger] Failed to create command queue\r\n");
        vTaskSuspend(NULL);
    }

    Logger_InitFilesystemMutex();

    // Add delay to ensure SD card power-up
    osDelay(100);

    FATFS fs;
    FRESULT fs_res;
    int retry_count = 0;
    const int MAX_RETRIES = 3;

    while (retry_count < MAX_RETRIES) {
        fs_res = f_mount(&fs, "", 1);
        if (fs_res == FR_OK) {
            break;
        }
        osDelay(100);
        retry_count++;
    }


    for (;;) {

        if (xQueueReceive(g_logger_cmd_queue, &cmd, portMAX_DELAY) != pdTRUE)
            continue;


        if (cmd.m_type == LOGGER_CMD_WRITE) {

        	char filename[32];
        	int yyyy, mm, dd;

        	if (sscanf(cmd.m_payload, "%4d-%2d-%2d", &yyyy, &mm, &dd) == 3) {
        	    int yy = yyyy % 100; // Keep last 2 digits of year
        	    snprintf(filename, sizeof(filename), "%02d_%02d_%02d.TXT", yy, mm, dd);
        	} else {
        	    printf("[Logger] Failed to parse date from payload: %s\r\n", cmd.m_payload);
        	    continue;
        	}

        	osMutexAcquire(sdMutexHandle, osWaitForever);
        	res = f_open(&file, filename, FA_OPEN_ALWAYS | FA_WRITE);
        	if (res != FR_OK) {
        	    printf("[Logger sample write] f_open error: %d\r\n", res);
        	    f_close(&file);
        	    osMutexRelease(sdMutexHandle);
        	    continue;
        	}

        	// If file is newly created, it will be empty
        	if (f_size(&file) == 0) {
        		KeepOnlyRecent(LOGGER_MAX_RECENT_FILES);
        	}

        	// Move pointer to end so we append instead of overwrite
        	if (f_lseek(&file, f_size(&file)) != FR_OK) {
        	    printf("[Logger] f_lseek error\r\n");
        	    continue;
        	}

            res = f_write(&file, cmd.m_payload, strlen(cmd.m_payload), &bytes_written);
            if (res != FR_OK || bytes_written != strlen(cmd.m_payload)) {
                printf("[Logger] f_write error: %d\r\n", res);
                f_close(&file);
                continue;
            }


            f_close(&file);
            osMutexRelease(sdMutexHandle);
        }

        else if (cmd.m_type == LOGGER_CMD_WRITE_EVENT) {

        	osMutexAcquire(sdMutexHandle, osWaitForever);
			res = f_open(&file, LOGGER_EVENT_FILENAME, FA_OPEN_APPEND | FA_WRITE);
			if (res != FR_OK) {
				printf("[Logger write event] f_open error: %d\r\n", res);
				osMutexRelease(sdMutexHandle);
				continue;
			}

			res = f_write(&file, cmd.m_payload, strlen(cmd.m_payload), &bytes_written);
			if (res != FR_OK || bytes_written != strlen(cmd.m_payload)) {
				printf("[Logger] f_write error: %d\r\n", res);
				f_close(&file);
				continue;
			}

		    f_close(&file);
		    osMutexRelease(sdMutexHandle);
		}

        else if (cmd.m_type == LOGGER_CMD_SAMPLE_REQUEST) {
			Logger_HandleSampleRequest(cmd.m_payload, strlen(cmd.m_payload));
        }
    }
}

static void format_filename_from_date(char* _filename, size_t _size, struct tm const* _tm_date)
{
    int yy = _tm_date->tm_year % 100;
    int mm = _tm_date->tm_mon + 1;
    int dd = _tm_date->tm_mday;
    snprintf(_filename, _size, "%02d_%02d_%02d.TXT", yy, mm, dd);
}

static void send_lines_from_file(const char* filename)
{
    FIL file;
    osMutexAcquire(sdMutexHandle, osWaitForever);
    FRESULT fres = f_open(&file, "25_04_22.TXT", FA_READ);
    if (fres != FR_OK) {
        printf("f_open returned: %d\n", fres);
        osMutexRelease(sdMutexHandle);
        return;
    }

    char line[128];
    while (f_gets(line, sizeof(line), &file)) {
        CommunicatorMessage_t msg;
        msg.packet_id = PROTO_PKT_SAMPLE;
        strncpy((char*)msg.payload, line, sizeof(msg.payload));
        msg.payload_len = strlen(line);



        xQueueSend(g_sample_tx_queue, &msg, pdMS_TO_TICKS(50));
        osEventFlagsSet(g_comm_event_flags, COMM_EVT_SAMPLE_READY);
    }

    f_close(&file);
    osMutexRelease(sdMutexHandle);
}

void Logger_HandleSampleRequest(const char* payload, uint8_t len)
{
    if (!payload || len < 16) {
        printf("[Logger] Invalid sample request payload\r\n");
        return;
    }

    char from_date[9], to_date[9];
    memcpy(from_date, payload, 8); from_date[8] = '\0';
    memcpy(to_date, payload + 8, 8); to_date[8] = '\0';

    // Parse start date
    struct tm tm_start = {0};
    sscanf(from_date, "%4d%2d%2d", &tm_start.tm_year, &tm_start.tm_mon, &tm_start.tm_mday);
    tm_start.tm_year -= 1900;
    tm_start.tm_mon  -= 1;

    // Parse end date
    struct tm tm_end = {0};
    sscanf(to_date, "%4d%2d%2d", &tm_end.tm_year, &tm_end.tm_mon, &tm_end.tm_mday);
    tm_end.tm_year -= 1900;
    tm_end.tm_mon  -= 1;

    time_t t_start = mktime(&tm_start);
    time_t t_end   = mktime(&tm_end);

    while (t_start <= t_end) {
        struct tm* current = localtime(&t_start);

        char filename[32];
        format_filename_from_date(filename, sizeof(filename), current);

        send_lines_from_file(filename);

        current->tm_mday += 1;
        t_start = mktime(current);
    }
}

/* --- Static Helper Functions --- */

static void KeepOnlyRecent(int _max_files)
{
    DIR dir;
    FILINFO fno;
    char txt_files[16][32];
    int count = 0;

    if (f_opendir(&dir, "/") == FR_OK) {
        while (f_readdir(&dir, &fno) == FR_OK && fno.fname[0]) {
            if (strstr(fno.fname, ".TXT")) {
                strncpy(txt_files[count++], fno.fname, sizeof(txt_files[0]));
                if (count >= 16) break;
            }
        }
        f_closedir(&dir);
    }

    if (count <= _max_files) return;

    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (strcmp(txt_files[i], txt_files[j]) > 0) {
                char tmp[32];
                strcpy(tmp, txt_files[i]);
                strcpy(txt_files[i], txt_files[j]);
                strcpy(txt_files[j], tmp);
            }
        }
    }

    for (int i = 0; i < count - _max_files; i++) {
        f_unlink(txt_files[i]);
    }
}


static void Logger_InitFilesystemMutex()
{
    sdMutexHandle = osMutexNew(NULL);
    configASSERT(sdMutexHandle != NULL);

}
