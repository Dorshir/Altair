/**
 * @file date_time.h
 * @brief RTC-based date and time management interface.
 *
 * Provides a lightweight abstraction over the STM32 HAL RTC module,
 * including setting/getting date and time, formatting, and alarms.
 */

#ifndef INC_DATE_TIME_H_
#define INC_DATE_TIME_H_

#include "main.h"
#include <stddef.h>

/**
 * @brief Wrapper for the RTC handle.
 */
typedef struct DateTime {
	RTC_HandleTypeDef *m_hrtc;  /**< Pointer to RTC handle */
} DateTime;

/**
 * @brief Structure representing time (HH:MM:SS).
 */
typedef struct Time {
	uint8_t m_hour;     /**< Hours (0-23) */
	uint8_t m_minute;   /**< Minutes (0-59) */
	uint8_t m_seconds;  /**< Seconds (0-59) */
} Time;

/**
 * @brief Structure representing date (YY-MM-DD and weekday).
 */
typedef struct Date {
	uint8_t m_year;      /**< Year (e.g., 25 for 2025) */
	uint8_t m_month;     /**< Month (1-12) */
	uint8_t m_day;       /**< Day of month (1-31) */
	uint8_t m_week_day;  /**< Weekday (1=Monday to 7=Sunday) */
} Date;

/**
 * @brief Result codes for date/time operations.
 */
typedef enum DateTime_Result {
	DATE_TIME_OK,         /**< Operation successful */
	DATE_TIME_NULLPTR,    /**< Null pointer error */
	DATE_TIME_HAL_ERROR   /**< HAL operation error */
} DateTime_Result;

/**
 * @brief Creates a new DateTime instance with the given RTC handle.
 *
 * @param _hrtc Pointer to HAL RTC handle.
 * @return Pointer to a DateTime object.
 */
DateTime* DateTime_Create(RTC_HandleTypeDef * _hrtc);

/**
 * @brief Retrieves the current date from the RTC.
 *
 * @param _datetime Pointer to DateTime instance.
 * @param _date Output pointer for date.
 * @return Result code.
 */
DateTime_Result DateTime_GetDate(struct DateTime* _datetime, Date* _date);

/**
 * @brief Sets the RTC date.
 *
 * @param _datetime Pointer to DateTime instance.
 * @param _date Pointer to date to be set.
 * @return Result code.
 */
DateTime_Result DateTime_SetDate(struct DateTime* _datetime, const Date* _date);

/**
 * @brief Retrieves the current time from the RTC.
 *
 * @param _datetime Pointer to DateTime instance.
 * @param _time Output pointer for time.
 * @return Result code.
 */
DateTime_Result DateTime_GetTime(struct DateTime* _datetime, Time* _time);

/**
 * @brief Sets the RTC time.
 *
 * @param _datetime Pointer to DateTime instance.
 * @param _time Pointer to time to be set.
 * @return Result code.
 */
DateTime_Result DateTime_SetTime(struct DateTime* _datetime, const Time* _time);

/**
 * @brief Retrieves both date and time from the RTC.
 *
 * @param _datetime Pointer to DateTime instance.
 * @param _date Output pointer for date.
 * @param _time Output pointer for time.
 * @return Result code.
 */
DateTime_Result DateTime_Time(struct DateTime* _datetime, Date* _date, Time* _time);

/**
 * @brief Sets an RTC alarm.
 *
 * @param _datetime Pointer to DateTime instance.
 * @param _date Pointer to alarm date.
 * @param _time Pointer to alarm time.
 * @return Result code.
 */
DateTime_Result DateTime_SetAlarm(DateTime* _datetime, Date* _date, Time* _time);

/**
 * @brief Formats the current date and time into a string.
 *
 * Format: "YYYY-MM-DD HH:MM:SS"
 *
 * @param _out_buf Output buffer.
 * @param _buf_size Size of the output buffer.
 */
void DateTime_FormatString(char* _out_buf, size_t _buf_size);

#endif /* INC_DATE_TIME_H_ */
