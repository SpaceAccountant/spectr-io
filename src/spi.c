#include "spi.h"

#include <asm/io.h>
#include <linux/module.h>

#include "gpio.h"

#define SPI_OFFSET	0x00204000
#define SPI_SIZE	0x18

unsigned int spi_hw_timeout = 1000;

u8* spi_mem = ( u8* ) 0;

int spi_read_byte( u8* byte ) {
	const unsigned long timeout = jiffies + ( spi_hw_timeout * HZ ) / 1000;
	while ( !( dma_get_flags32( spi_mem + SPI_CS, SPI_CS_RXD ) ) ) {
		if ( jiffies >= timeout ) {
			goto spi_read_err;
		}
	}

	*byte = dma_read8( spi_mem + SPI_FIFO );

	return 0;

spi_read_err:
	printk( KERN_ERR "SPI: Hardware timout on RXD.\n" ); 

	return SPI_ERR_HW_TIMEOUT;
}

size_t spi_read( ssize_t len, u8* data ) {
	size_t i = 0;
	for ( ; i < len; i++ ) {
		const unsigned long timeout = jiffies + ( spi_hw_timeout * HZ ) / 1000;
		while ( !( dma_get_flags32( spi_mem + SPI_CS, SPI_CS_RXD ) ) ) {
			if ( jiffies >= timeout ) {
				goto spi_read_err;
			}
		}

		data[i] = dma_read8( spi_mem + SPI_FIFO );
	}

	return i;

spi_read_err:
	printk( KERN_ERR "SPI: Hardware timout on RXD.\n" ); 

	return SPI_ERR_HW_TIMEOUT;
}

int spi_write_byte( u8 byte ) {
	const unsigned long timeout = jiffies + ( spi_hw_timeout * HZ ) / 1000;
	while ( !( dma_get_flags32( spi_mem + SPI_CS, SPI_CS_TXD ) ) ) {
		if ( jiffies >= timeout ) {
			goto spi_write_err;
		}
	}

	dma_write8( spi_mem + SPI_FIFO, byte );

	return 0;

spi_write_err:
	printk( KERN_ERR "SPI: Hardware timout on TXD.\n" ); 

	return SPI_ERR_HW_TIMEOUT;
}

size_t spi_write( ssize_t len, const u8* data ) {
	size_t i = 0;
	for ( ; i < len; i++ ) {
		const unsigned long timeout = jiffies + ( spi_hw_timeout * HZ ) / 1000;
		while ( !( dma_get_flags32( spi_mem + SPI_CS, SPI_CS_TXD ) ) ) {
			if ( jiffies >= timeout ) {
				goto spi_write_err;
			}
		}

		dma_write8( spi_mem + SPI_FIFO, data[i] );
	}

	return i;

spi_write_err:
	printk( KERN_ERR "SPI: Hardware timout on TXD.\n" ); 

	return SPI_ERR_HW_TIMEOUT;
}

int spi_await_transfer( void ) {
	const unsigned long timeout = jiffies + ( spi_hw_timeout * HZ ) / 1000;
	while ( !( dma_get_flags32( spi_mem + SPI_CS, SPI_CS_DONE ) ) ) {
		if ( jiffies >= timeout ) {
			goto spi_done_err;
		}
	}

	return 0;

spi_done_err:
	printk( KERN_ERR "SPI: Hardware timout on DONE.\n" );

	return SPI_ERR_HW_TIMEOUT;
}

int __init spi_init( void ) {
	gpio_set_pin_mode(  7, GPIO_PINMODE_ALT0 );
	gpio_set_pin_mode(  8, GPIO_PINMODE_ALT0 );
	gpio_set_pin_mode(  9, GPIO_PINMODE_ALT0 );
	gpio_set_pin_mode( 10, GPIO_PINMODE_ALT0 );
	gpio_set_pin_mode( 11, GPIO_PINMODE_ALT0 );

	spi_mem = ( u8* ) ioremap( BCM2836_IO_MEM_START + SPI_OFFSET, SPI_SIZE );
	if ( !spi_mem ) {
		return SPI_ERR_IO_MAP_FAIL;
	}

	dma_write32( spi_mem + SPI_CS,  0x00000000 );
	dma_write32( spi_mem + SPI_CLK, 0x00000000 );

	return 0;
}

void __exit spi_exit( void ) {
	if ( spi_mem ) {
		iounmap( spi_mem );
		spi_mem = ( u8* ) 0;
	}

	gpio_set_pin_mode(  7, GPIO_PINMODE_INPUT );
	gpio_set_pin_mode(  8, GPIO_PINMODE_INPUT );
	gpio_set_pin_mode(  9, GPIO_PINMODE_INPUT );
	gpio_set_pin_mode( 10, GPIO_PINMODE_INPUT );
	gpio_set_pin_mode( 11, GPIO_PINMODE_INPUT );
}

EXPORT_SYMBOL( spi_hw_timeout );
EXPORT_SYMBOL( spi_mem );
EXPORT_SYMBOL( spi_read_byte );
EXPORT_SYMBOL( spi_read );
EXPORT_SYMBOL( spi_write_byte );
EXPORT_SYMBOL( spi_write );
EXPORT_SYMBOL( spi_await_transfer );

