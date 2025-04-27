#ifndef INC_CONFIG_H_
#define INC_CONFIG_H_

#include "tim.h"
#include "adc.h"

#define ALTAIR_MAX_DELAY HAL_MAX_DELAY
#define BEACON_MS		 6000

// DHT
#define DHT_PIN        GPIO_PIN_5
#define DHT_PORT       GPIOB
#define DHT_TIMER_ADDR &htim6

// ADC
#define POT_ADC_ADDR &hadc1
#define LDR_ADC_ADDR &hadc2

// Logger
#define LOGGER_EVENT_FILENAME   "events.txt"
#define LOGGER_CMD_PAYLOAD_MAX  260
#define LOGGER_QUEUE_LENGTH     10
#define LOGGER_SYNC_INTERVAL    10
#define LOGGER_MAX_RECENT_FILES 7
typedef enum {
    LOGGER_CMD_WRITE,
	LOGGER_CMD_WRITE_EVENT,
	LOGGER_CMD_SAMPLE_REQUEST
} Logger_Command_Type_t;

// Date Time
#define DATE_TIME_BASE_YEAR     2000
#define DATE_TIME_TIMESTAMP_LEN 20

// Event Manager
#define EVENT_QUEUE_LENGTH     10
#define EVENT_DESCRIPTION_MAX  128
#define EVENT_TIMESTAMP_MAX    32
typedef enum {
    EVENT_SOURCE_MONITORING,
    EVENT_SOURCE_CONFIG,
    EVENT_SOURCE_INIT
} Event_Source_t;

typedef enum {
    EVENT_TYPE_INFO,
    EVENT_TYPE_ENTER_ERROR,
    EVENT_TYPE_ENTER_NORMAL,
    EVENT_TYPE_ENTER_SAFE
} Event_Type_t;


// System Modes
typedef enum {
    SYSTEM_MODE_NORMAL,
    SYSTEM_MODE_ERROR,
    SYSTEM_MODE_SAFE,
	EVENT_TYPE_RECOVER
} SystemMode_t;

// Configurations
#define CONFIG_TEMP_MAX        40
#define CONFIG_HUM_MIN         95
#define CONFIG_LDR_MIN         100
#define CONFIG_VOLT_MIN        2000
#define CONFIG_FLASH_ADDR      0x08080000
#define CONFIG_FLASH_PAGE      256
#define CONFIG_FLASH_BANK 	   FLASH_BANK_2

// RGB
#define RGB_RED_PIN            GPIO_PIN_7
#define RGB_RED_PORT           GPIOC
#define RGB_GREEN_PIN          GPIO_PIN_15
#define RGB_GREEN_PORT         GPIOB
#define RGB_BLUE_PIN           GPIO_PIN_14
#define RGB_BLUE_PORT  		   GPIOB

// Sample
#define SAMPLE_DELAY_NORMAL    5000
#define SAMPLE_DELAY_SAFE      (SAMPLE_DELAY_NORMAL * 2)

// Communicator
#define COMM_EVT_KEEP_ALIVE    (1 << 0)
#define COMM_EVT_EVENT         (1 << 1)
#define COMM_EVT_SAMPLE        (1 << 2)
#define COMM_EVT_SAMPLE_READY  (1 << 3)
#define COMM_EVENT_GROUP_FLAGS (COMM_EVT_KEEP_ALIVE | COMM_EVT_EVENT | COMM_EVT_SAMPLE | COMM_EVT_SAMPLE_READY)
#define COMM_QUEUE_LEN 		   10

// Protocol
#define PROTO_MAX_PAYLOAD_LEN  260
#define PROTO_MAX_PACKET_LEN   (1 + 1 + PROTO_MAX_PAYLOAD_LEN + 1 + 1)
#define PROTO_END_BYTE 		   0x55
#define PROTO_PKT_KEEP_ALIVE   0x01
#define PROTO_PKT_EVENT        0x02
#define PROTO_PKT_SAMPLE       0x03
#define PROTO_PKT_TIME_SYNC    0x04



#endif /* INC_CONFIG_H_ */
