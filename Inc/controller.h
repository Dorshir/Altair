/**
 * @file controller.h
 * @brief Task for monitoring sensor data and managing system mode.
 *
 * The controller task evaluates the latest sensor sample against
 * configured thresholds and updates the system mode accordingly.
 * It reports mode transitions as events and notifies the logger.
 */

#ifndef INC_CONTROLLER_H_
#define INC_CONTROLLER_H_

/**
 * @brief RTOS task that monitors sensor values and manages system state.
 *
 * This task waits for a notification, then retrieves the latest sample,
 * compares it against thresholds, and sets the system mode accordingly.
 * Mode changes trigger event reports and sampling behavior adjustments.
 *
 * @param _arg Unused task argument.
 */
void Controller_Task(void* _arg);

#endif /* INC_CONTROLLER_H_ */
