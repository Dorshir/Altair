/**
 * @file init_sys.h
 * @brief System initialization task for bringing up all application components.
 *
 * Responsible for starting core tasks after system time has been synchronized.
 * Initializes communication, configuration, periodic keep-alive, and RTOS tasks.
 */

#ifndef INC_INIT_SYS_H_
#define INC_INIT_SYS_H_

/**
 * @brief Initialization task for system startup sequence.
 *
 * This task waits for the time synchronization event, initializes configuration and
 * keep-alive timer, and launches the rest of the application tasks (logger, controller,
 * sampler, etc.). Once all components are initialized, the task self-deletes.
 *
 * @param arg Unused task argument.
 */
void Init_Task(void *arg);

#endif /* INC_INIT_SYS_H_ */
