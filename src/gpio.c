#include "gpio.h"

#include <asm/io.h>
#include <linux/module.h>

#define GPIO_OFFSET	0x00200000
#define GPIO_SIZE	0x3C

u8* gpio_mem = ( u8* ) 0;

void gpio_set_pin_mode( unsigned int pin, unsigned int pinmode ) {
	void __iomem* const addr = gpio_mem + GPIO_GPFSEL0 + ( ( ( int ) ( ( pin % 53 ) / 10 ) ) << 2 );
	const int bit = ( pin % 10 ) * 3;
	dma_clr_flags32( addr, 0x07 << bit );
	dma_set_flags32( addr, ( pinmode ) << bit );
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

EXPORT_SYMBOL( gpio_mem );
EXPORT_SYMBOL( gpio_set_pin_mode );

