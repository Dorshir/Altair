#include "date_time.h"
#include "rtc.h"
#include "usart.h"
#include "config.h"
#include <stdio.h>

static DateTime s_dt;

/* --- Forward Declarations --- */

static void DateBCDtoDEC(Date* _date, const RTC_DateTypeDef* _sDate);
static void TimeBCDtoDEC(Time* _time, const RTC_TimeTypeDef* _sTime);
static void DateDECtoBCD(const Date* _date, RTC_DateTypeDef* _sDate);
static void TimeDECtoBCD(const Time* _time, RTC_TimeTypeDef* _sTime);
static uint8_t BCDToDEC(uint8_t _bcd);
static uint8_t DECToBCD(uint8_t _dec);

/* --- Public Functions --- */

DateTime* DateTime_Create(RTC_HandleTypeDef *_hrtc)
{
	if (!_hrtc) {
		return NULL;
	}

	s_dt.m_hrtc = _hrtc;

	return &s_dt;
}


DateTime_Result DateTime_GetDate(DateTime* _datetime, Date* _date)
{
	RTC_DateTypeDef sDate;

	if (!_datetime || !_date) {
		return DATE_TIME_NULLPTR;
	}

	if (HAL_RTC_GetDate(_datetime->m_hrtc, &sDate, RTC_FORMAT_BCD) == HAL_OK) {
		DateBCDtoDEC(_date ,&sDate);
		return DATE_TIME_OK;
	} else {
		return DATE_TIME_HAL_ERROR;
	}
}


DateTime_Result DateTime_SetDate(DateTime* _datetime, const Date* _date)
{
	RTC_DateTypeDef sDate;

	if (!_datetime || !_date) {
		return DATE_TIME_NULLPTR;
	}

	DateDECtoBCD(_date, &sDate);

	if (HAL_RTC_SetDate(_datetime->m_hrtc, &sDate, RTC_FORMAT_BCD) == HAL_OK) {
		return DATE_TIME_OK;
	} else {
		return DATE_TIME_HAL_ERROR;
	}
}


DateTime_Result DateTime_GetTime(DateTime* _datetime, Time* _time)
{
	RTC_TimeTypeDef sTime;

	if (!_datetime || !_time) {
		return DATE_TIME_NULLPTR;
	}

	if (HAL_RTC_GetTime(_datetime->m_hrtc, &sTime, RTC_FORMAT_BCD) == HAL_OK) {
		TimeBCDtoDEC(_time ,&sTime);
		return DATE_TIME_OK;
	} else {
		return DATE_TIME_HAL_ERROR;
	}
}


DateTime_Result DateTime_SetTime(DateTime* _datetime, const Time* _time)
{
	RTC_TimeTypeDef sTime;

	if (!_datetime || !_time) {
		return DATE_TIME_NULLPTR;
	}

	TimeDECtoBCD(_time, &sTime);

	if (HAL_RTC_SetTime(_datetime->m_hrtc, &sTime, RTC_FORMAT_BCD) == HAL_OK) {
		return DATE_TIME_OK;
	} else {
		return DATE_TIME_HAL_ERROR;
	}
}

DateTime_Result DateTime_Time(DateTime* _datetime, Date* _date, Time* _time)
{
	DateTime_Result result;

	if (!_datetime || !_date || !_time) {
		return DATE_TIME_NULLPTR;
	}

	result = DateTime_GetTime(_datetime, _time);
	if (result == DATE_TIME_OK) {
		result = DateTime_GetDate(_datetime, _date);
	}

	return result;
}


DateTime_Result DateTime_SetAlarm(DateTime* _datetime, Date* _date, Time* _time)
{
	if (!_datetime || !_date || !_time) {
		return DATE_TIME_NULLPTR;
	}

	RTC_AlarmTypeDef sAlarm = {0};

	sAlarm.AlarmTime.Hours   = DECToBCD(_time->m_hour);
	sAlarm.AlarmTime.Minutes = DECToBCD(_time->m_minute);
	sAlarm.AlarmTime.Seconds = DECToBCD(_time->m_seconds);
	sAlarm.AlarmTime.SubSeconds = 0x0;
	sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
	sAlarm.AlarmMask = RTC_ALARMMASK_NONE;
	sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
	sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
	sAlarm.AlarmDateWeekDay = DECToBCD(_date->m_day);
	sAlarm.Alarm = RTC_ALARM_A;
	if (HAL_RTC_SetAlarm_IT(_datetime->m_hrtc, &sAlarm, RTC_FORMAT_BCD) != HAL_OK)
	{
		return DATE_TIME_HAL_ERROR;
	}

	return DATE_TIME_OK;
}


void DateTime_FormatString(char* _out_buf, size_t _buf_size)
{
	Date date;
	Time time;

	if (DateTime_Time(&s_dt, &date, &time) != DATE_TIME_OK) {
		snprintf(_out_buf, _buf_size, "0000-00-00 00:00:00");
		return;
	}

	snprintf(_out_buf, _buf_size, "%04u-%02u-%02u %02u:%02u:%02u",
			 DATE_TIME_BASE_YEAR + date.m_year,
	         date.m_month,
	         date.m_day,
	         time.m_hour,
	         time.m_minute,
	         time.m_seconds);
}

/* --- Static Helper Functions --- */

static void DateBCDtoDEC(Date* _date, const RTC_DateTypeDef* _sDate)
{
    _date->m_day = BCDToDEC(_sDate->Date);
    _date->m_month = BCDToDEC(_sDate->Month);
    _date->m_year = BCDToDEC(_sDate->Year);
	_date->m_week_day = BCDToDEC(_sDate->WeekDay);
}


static void DateDECtoBCD(const Date* _date, RTC_DateTypeDef* _sDate)
{
	_sDate->Date = DECToBCD(_date->m_day);
	_sDate->Month = DECToBCD(_date->m_month);
	_sDate->Year = DECToBCD(_date->m_year);
	_sDate->WeekDay = DECToBCD(_date->m_week_day);
}


static void TimeBCDtoDEC(Time* _time, const RTC_TimeTypeDef* _sTime)
{
    _time->m_hour = BCDToDEC(_sTime->Hours);
    _time->m_minute = BCDToDEC(_sTime->Minutes);
    _time->m_seconds = BCDToDEC(_sTime->Seconds);
}


static void TimeDECtoBCD(const Time* _time, RTC_TimeTypeDef* _sTime)
{
    _sTime->Hours = DECToBCD(_time->m_hour);
    _sTime->Minutes = DECToBCD(_time->m_minute);
    _sTime->Seconds = DECToBCD(_time->m_seconds);

    _sTime->DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    _sTime->StoreOperation = RTC_STOREOPERATION_RESET;
}


static uint8_t BCDToDEC(uint8_t _bcd)
{
    return (uint8_t)((_bcd >> 4) * 10 + (_bcd & 0x0F));
}


static uint8_t DECToBCD(uint8_t _dec)
{
    return (uint8_t)(((_dec / 10) << 4) | (_dec % 10));
}
