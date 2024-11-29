#ifndef _SPECTRE_IO_GPIO_H
#define _SPECTRE_IO_GPIO_H

#include <linux/bitops.h>
#include <linux/init.h>

#include <dma.h>

#define GPIO_ERR_IO_MAP_FAIL -1

#define GPIO_PINMODE_INPUT	0x00
#define GPIO_PINMODE_OUTPUT	0x01
#define GPIO_PINMODE_ALT0	0x04
#define GPIO_PINMODE_ALT1	0x05
#define GPIO_PINMODE_ALT2	0x06
#define GPIO_PINMODE_ALT3	0x07
#define GPIO_PINMODE_ALT4	0x03
#define GPIO_PINMODE_ALT5	0x02

#define GPIO_PIN_LEVEL_LOW	0
#define GPIO_PIN_LEVEL_HIGH	1

/**
 * Initializes the GPIO subsystem.
 *
 * @returns Zero on success; a negative error code on failure.
 *
 */
extern int __init gpio_init( void );

/**
 * Destroys the GPIO subsystem.
 *
 */
extern void __exit gpio_exit( void );

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
#define gpio_set_pin_low( pin ) {									\
	dma_set_flags32( gpio_mem + GPIO_GPCLR0 + ( ( ( ( pin ) % 53 ) >> 5 ) << 2 ),			\
		BIT( ( pin ) % 32 ) );									\
}

/**
 * Sets the output of a GPIO bus pin to high.
 *
 * @param pin The pin.
 *
 */
#define gpio_set_pin_high( pin ) {									\
	dma_set_flags32( gpio_mem + GPIO_GPSET0 + ( ( ( ( pin ) % 53 ) >> 5 ) << 2 ),			\
		BIT( ( pin ) % 32 ) );									\
}

/**
 * Sets the output of a GPIO bus pin to high.
 *
 * @param pin The pin.
 *
 */
#define gpio_get_pin_level( pin ) (									\
	dma_get_flags32( gpio_mem + GPIO_GPSET0 + ( ( ( ( pin ) % 53 ) >> 5 ) << 2 ),			\
		BIT( ( pin ) % 32 ) ) > 0								\
)

#define GPIO_GPFSEL0	0x00
#define GPIO_GPFSEL1	0x04
#define GPIO_GPFSEL2	0x08
#define GPIO_GPFSEL3	0x0C
#define GPIO_GPFSEL4	0x10
#define GPIO_GPFSEL5	0x14
#define GPIO_GPSET0	0x1C
#define GPIO_GPSET1	0x20
#define GPIO_GPCLR0	0x28
#define GPIO_GPCLR1	0x2C
#define GPIO_GPLEV0	0x34
#define GPIO_GPLEV1	0x38

extern u8* gpio_mem;

#endif // _SPECTRE_IO_GPIO_H

