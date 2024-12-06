#include "gpio.h"

#include <asm/io.h>
#include <linux/bitops.h>
#include <linux/module.h>

#include <dma.h>
#include <log.h>

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

int __init gpio_init( void ) {
#if defined( DEBUG )
	LOG( KERN_DEBUG, "GPIO mapping IO memory into kernel virtual address space." );
#endif // DEBUG
	gpio_mem = ( u8* ) ioremap( BCM2836_IO_MEM_START + GPIO_OFFSET, GPIO_SIZE );
	if ( !gpio_mem ) {
		LOG( KERN_ERR, "GPIO failed to map IO memory." );
		return GPIO_ERR_IO_MAP_FAIL;
	}

	return 0;
}

void __exit gpio_exit( void ) {
	if ( gpio_mem ) {
#if defined( DEBUG )
		LOG( KERN_DEBUG, "GPIO unmapping IO memory from kernel virtual address space." );
#endif // DEBUG
		iounmap( gpio_mem );
		gpio_mem = ( u8* ) 0;
	}
}

void gpio_set_pin_mode( unsigned int pin, unsigned int mode ) {
#if defined( DEBUG )
	LOG( KERN_DEBUG, "GPIO setting mode for pin %d to 0x%02X.", pin, mode );
#endif // DEBUG
	void __iomem* const addr = gpio_mem + GPIO_GPFSEL0
		+ ( ( ( unsigned int ) ( ( pin % 53 ) / 10 ) ) << 2 );
	const int bit = ( pin % 10 ) * 3;
	dma_clr_flags32( addr, 0x07 << bit );
	dma_set_flags32( addr, mode << bit );
}

void gpio_set_pin_low( unsigned int pin ) {
#if defined( DEBUG )
	LOG( KERN_DEBUG, "GPIO setting level for pin %d to low.", pin );
#endif // DEBUG
	dma_set_flags32( gpio_mem + GPIO_GPCLR0 + ( ( ( pin % 53 ) >> 5 ) << 2 ), BIT( pin & 0x1F ) );
}

void gpio_set_pin_high( unsigned int pin ) {
#if defined( DEBUG )
	LOG( KERN_DEBUG, "GPIO setting level for pin %d to high.", pin );
#endif // DEBUG
	dma_set_flags32( gpio_mem + GPIO_GPSET0 + ( ( ( pin % 53 ) >> 5 ) << 2 ), BIT( pin & 0x1F ) );
}

unsigned int gpio_get_pin_level( unsigned int pin ) {
#if defined( DEBUG )
	LOG( KERN_DEBUG, "GPIO getting level for pin %d.", pin );
#endif // DEBUG
	return dma_get_flags32( gpio_mem + GPIO_GPLEV0 + ( ( ( pin % 53 ) >> 5 ) << 2 ),
		BIT( pin & 0x1F ) ) > 0;
}

EXPORT_SYMBOL( gpio_set_pin_mode );
EXPORT_SYMBOL( gpio_set_pin_low );
EXPORT_SYMBOL( gpio_set_pin_high );
EXPORT_SYMBOL( gpio_get_pin_level );

