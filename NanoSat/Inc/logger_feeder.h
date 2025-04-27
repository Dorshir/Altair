/**
 * @file logger_feeder.h
 * @brief Feeds formatted sensor samples to the logger task.
 *
 * Collects the latest sample and pushes it to the logger queue
 * upon notification. Ensures consistent logging of sensor data.
 */

#ifndef INC_LOGGER_FEEDER_H_
#define INC_LOGGER_FEEDER_H_

/**
 * @brief RTOS task responsible for feeding sensor samples to the logger.
 *
 * Waits for a task notification, retrieves the latest sample from the sampler,
 * formats it into a CSV-style string, and sends it to the logger queue for storage.
 *
 * @param _arg Unused task argument.
 */
void LoggerFeeder_Task(void* _arg);

#endif /* INC_LOGGER_FEEDER_H_ */
