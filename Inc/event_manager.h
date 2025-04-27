/**
 * @file event_manager.h
 * @brief Manages system event reporting, notification, and logging.
 *
 * The event manager handles creation and queuing of events, updates system indicators
 * (e.g., RGB, buzzer), and forwards event logs to the logger module.
 */

#ifndef INC_EVENT_MANAGER_H_
#define INC_EVENT_MANAGER_H_

#include "config.h"
#include <stdint.h>

/**
 * @brief Structure representing a system event.
 */
typedef struct {
	Event_Source_t m_source;                        /**< Source of the event (e.g., monitoring, system) */
	Event_Type_t m_type;                            /**< Type of event (e.g., ERROR, NORMAL, SAFE) */
	char m_timestamp[EVENT_TIMESTAMP_MAX];          /**< Timestamp of the event (formatted string) */
	char m_description[EVENT_DESCRIPTION_MAX];      /**< Description of the event */
} Event_t;

/**
 * @brief FreeRTOS task that processes and logs events.
 *
 * Receives events from the internal event queue, updates indicators
 * (RGB LED, buzzer), timestamps them, and sends them to the logger queue.
 *
 * @param _arg Unused task argument.
 */
void EventManager_Task(void* _arg);

/**
 * @brief Reports a new event with a formatted string description.
 *
 * This function sends an event to the internal queue for processing and logging.
 *
 * @param _src Source of the event.
 * @param _type Type of event.
 * @param _description Null-terminated description string.
 * @return 1 on success, 0 if queue is not initialized or full.
 */
int EventManager_ReportString(Event_Source_t _src, Event_Type_t _type, const char* _description);

#endif /* INC_EVENT_MANAGER_H_ */
