#include "spi.h"

#include <asm/io.h>
#include <linux/bitops.h>
#include <linux/jiffies.h>
#include <linux/module.h>

#include <dma.h>

#include "gpio.h"

#define SPI_OFFSET	0x00204000
#define SPI_SIZE	0x18

#define SPI_CS		0x00
#define SPI_FIFO	0x04
#define SPI_CLK		0x08

#define SPI_CS_CSL	BIT(  0 )
#define SPI_CS_CSH	BIT(  1 )
#define SPI_CS_CPHA	BIT(  2 )
#define SPI_CS_CPOL	BIT(  3 )
#define SPI_CS_CLEAR_TX	BIT(  4 )
#define SPI_CS_CLEAR_RX	BIT(  5 )
#define SPI_CS_CSPOL	BIT(  6 )
#define SPI_CS_TA	BIT(  7 )
#define SPI_CS_DMAEN	BIT(  8 )
#define SPI_CS_INTD	BIT(  9 )
#define SPI_CS_INTR	BIT( 10 )
#define SPI_CS_ADCS	BIT( 11 )
#define SPI_CS_REN	BIT( 12 )
#define SPI_CS_LEN	BIT( 13 )
#define SPI_CS_DONE	BIT( 16 )
#define SPI_CS_RXD	BIT( 17 )
#define SPI_CS_TXD	BIT( 18 )
#define SPI_CS_RXR	BIT( 19 )
#define SPI_CS_RXF	BIT( 20 )
#define SPI_CS_CSPOL0	BIT( 21 )
#define SPI_CS_CSPOL1	BIT( 22 )
#define SPI_CS_CSPOL2	BIT( 23 )
#define SPI_CS_DMA_LEN	BIT( 24 )
#define SPI_CS_LEN_LONG	BIT( 25 )

#define SPI_CS_CSMASK	( SPI_CS_CSL | SPI_CS_CSH )
#define SPI_CS_MODEMASK	( SPI_CS_CPHA | SPI_CS_CPOL )

static u8* spi_mem = ( u8* ) 0;

unsigned int spi_hw_timeout = 1000;

void spi_set_clk_div( unsigned short div ) {
	dma_write16( spi_mem + SPI_CLK, div );
}

void spi_select_chip( unsigned int chip ) {
	dma_clr_flags32( spi_mem + SPI_CS, SPI_CS_CSMASK );
	dma_set_flags32( spi_mem + SPI_CS, ( chip ) & SPI_CS_CSMASK );
}

void spi_set_mode( unsigned int mode ) {
	dma_clr_flags32( spi_mem + SPI_CS, SPI_CS_MODEMASK );
	dma_set_flags32( spi_mem + SPI_CS, mode & SPI_CS_MODEMASK );
}

void spi_enable_reads( void ) {
	dma_set_flags32( spi_mem + SPI_CS, SPI_CS_REN );
}

void spi_disable_reads( void ) {
	dma_clr_flags32( spi_mem + SPI_CS, SPI_CS_REN );
}

void spi_begin_transfer( void ) {
	dma_set_flags32( spi_mem + SPI_CS, SPI_CS_CLEAR_TX | SPI_CS_CLEAR_RX | SPI_CS_TA );
}

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

void spi_end_transfer( void ) {
	dma_clr_flags32( spi_mem + SPI_CS, SPI_CS_TA );
}

int __init spi_init( void ) {
	gpio_set_pin_mode(  7, GPIO_PIN_MODE_ALT0 );
	gpio_set_pin_mode(  8, GPIO_PIN_MODE_ALT0 );
	gpio_set_pin_mode(  9, GPIO_PIN_MODE_ALT0 );
	gpio_set_pin_mode( 10, GPIO_PIN_MODE_ALT0 );
	gpio_set_pin_mode( 11, GPIO_PIN_MODE_ALT0 );

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

	gpio_set_pin_mode(  7, GPIO_PIN_MODE_INPUT );
	gpio_set_pin_mode(  8, GPIO_PIN_MODE_INPUT );
	gpio_set_pin_mode(  9, GPIO_PIN_MODE_INPUT );
	gpio_set_pin_mode( 10, GPIO_PIN_MODE_INPUT );
	gpio_set_pin_mode( 11, GPIO_PIN_MODE_INPUT );
}

EXPORT_SYMBOL( spi_hw_timeout );
EXPORT_SYMBOL( spi_set_clk_div );
EXPORT_SYMBOL( spi_select_chip );
EXPORT_SYMBOL( spi_set_mode );
EXPORT_SYMBOL( spi_enable_reads );
EXPORT_SYMBOL( spi_disable_reads );
EXPORT_SYMBOL( spi_begin_transfer );
EXPORT_SYMBOL( spi_read_byte );
EXPORT_SYMBOL( spi_read );
EXPORT_SYMBOL( spi_write_byte );
EXPORT_SYMBOL( spi_write );
EXPORT_SYMBOL( spi_await_transfer );
EXPORT_SYMBOL( spi_end_transfer );

