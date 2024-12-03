#ifndef _SPECTRE_IO_GPIO_H
#define _SPECTRE_IO_GPIO_H

#include <linux/init.h>

#define GPIO_ERR_IO_MAP_FAIL -1

#define GPIO_PIN_MODE_INPUT	0x00
#define GPIO_PIN_MODE_OUTPUT	0x01
#define GPIO_PIN_MODE_ALT0	0x04
#define GPIO_PIN_MODE_ALT1	0x05
#define GPIO_PIN_MODE_ALT2	0x06
#define GPIO_PIN_MODE_ALT3	0x07
#define GPIO_PIN_MODE_ALT4	0x03
#define GPIO_PIN_MODE_ALT5	0x02

#define GPIO_PIN_LEVEL_LOW	0
#define GPIO_PIN_LEVEL_HIGH	1

/**
 * Initializes the GPIO subsystem.
 *
 * @returns Zero on success; a negative error code on failure.
 *
 */
int __init gpio_init( void );

/**
 * Destroys the GPIO subsystem.
 *
 */
void __exit gpio_exit( void );

/**
 * Sets the mode of a GPIO bus pin.
 *
 * @param pin The pin.
 * @param pinmode The mode.
 *
 */
void gpio_set_pin_mode( unsigned int pin, unsigned int pinmode );

/**
 * Sets the output of a GPIO bus pin to low.
 *
 * @param pin The pin.
 *
 */
void gpio_set_pin_low( unsigned int pin );

/**
 * Sets the output of a GPIO bus pin to high.
 *
 * @param pin The pin.
 *
 */
void gpio_set_pin_high( unsigned int pin );

/**
 * Sets the output of a GPIO bus pin to high.
 *
 * @param pin The pin.
 *
 * @returns The pin level.
 *
 */
unsigned int gpio_get_pin_level( unsigned int pin );

#endif // _SPECTRE_IO_GPIO_H

