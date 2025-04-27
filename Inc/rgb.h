/**
 * @file rgb.h
 * @brief RGB LED control interface.
 *
 * Provides a function to set the color of an RGB LED by specifying
 * the on/off state of each color channel.
 */

#ifndef INC_RGB_H_
#define INC_RGB_H_

#include <stdint.h>

/**
 * @brief Sets the RGB LED color.
 *
 * Turns each color channel (red, green, blue) on or off based on input.
 * A value of 1 enables the channel, and 0 disables it.
 *
 * @param _r Red channel state (0 or 1)
 * @param _g Green channel state (0 or 1)
 * @param _b Blue channel state (0 or 1)
 */
void RGB_SetColor(uint8_t _r, uint8_t _g, uint8_t _b);

#endif /* INC_RGB_H_ */
