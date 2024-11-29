#include "gpio.h"

#include <asm/io.h>
#include <linux/bitops.h>
#include <linux/module.h>

#include <dma.h>

#define GPIO_OFFSET	0x00200000
#define GPIO_SIZE	0x3C

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

static u8* gpio_mem = ( u8* ) 0;

void gpio_set_pin_mode( unsigned int pin, unsigned int pinmode ) {
	void __iomem* const addr = gpio_mem + GPIO_GPFSEL0 + ( ( ( int ) ( ( pin % 53 ) / 10 ) ) << 2 );
	const int bit = ( pin % 10 ) * 3;
	dma_clr_flags32( addr, 0x07 << bit );
	dma_set_flags32( addr, ( pinmode ) << bit );
}

void gpio_set_pin_low( unsigned int pin ) {
	dma_set_flags32( gpio_mem + GPIO_GPCLR0 + ( ( ( pin % 53 ) >> 5 ) << 2 ), BIT( pin % 32 ) );
}

void gpio_set_pin_high( unsigned int pin ) {
	dma_set_flags32( gpio_mem + GPIO_GPSET0 + ( ( ( pin % 53 ) >> 5 ) << 2 ), BIT( pin % 32 ) );
}

unsigned int gpio_get_pin_level( unsigned int pin ) {
	return dma_get_flags32( gpio_mem + GPIO_GPLEV0 + ( ( ( pin % 53 ) >> 5 ) << 2 ),
		BIT( ( pin ) % 32 ) ) > 0;
}

int __init gpio_init( void ) {
	gpio_mem = ( u8* ) ioremap( BCM2836_IO_MEM_START + GPIO_OFFSET, GPIO_SIZE );
	if ( !gpio_mem ) {
		return GPIO_ERR_IO_MAP_FAIL;
	}

	return 0;
}

void __exit gpio_exit( void ) {
	if ( gpio_mem ) {
		iounmap( gpio_mem );
		gpio_mem = ( u8* ) 0;
	}
}

EXPORT_SYMBOL( gpio_set_pin_mode );
EXPORT_SYMBOL( gpio_set_pin_low );
EXPORT_SYMBOL( gpio_set_pin_high );
EXPORT_SYMBOL( gpio_get_pin_level );

