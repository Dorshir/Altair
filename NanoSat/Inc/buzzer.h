/**
 * @file buzzer.h
 * @brief Buzzer control interface for alert tones.
 *
 * Provides functions to play and stop alert sounds using a buzzer.
 */

#ifndef INC_BUZZER_H_
#define INC_BUZZER_H_

/**
 * @brief Start playing the alert sound on the buzzer.
 *
 * Initiates a predefined tone or sequence using PWM.
 */
void Buzzer_PlayAlert();

/**
 * @brief Stop the buzzer sound.
 *
 * Disables the tone output, stopping any ongoing alert.
 */
void Buzzer_Stop();

#endif /* INC_BUZZER_H_ */
