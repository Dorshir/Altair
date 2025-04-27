/**
 * @file keep_alive.h
 * @brief Periodic keep-alive mechanism for communicator task.
 *
 * Triggers a keep-alive event that is sent to the ground station
 * to indicate the system is operational.
 */

#ifndef INC_KEEP_ALIVE_H_
#define INC_KEEP_ALIVE_H_

/**
 * @brief Triggers a keep-alive event.
 *
 * Sets a high priority for the communicator task and signals
 * an event flag to send a keep-alive message to the ground station.
 * Typically called by a periodic timer.
 */
void KeepAlive_Trigger();

#endif /* INC_KEEP_ALIVE_H_ */
