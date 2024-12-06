#include "spi.h"

#include <asm/io.h>
#include <linux/bitops.h>
#include <linux/jiffies.h>
#include <linux/module.h>

#include <dma.h>
#include <log.h>

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

#define SPI_CS_CS_MASK		( SPI_CS_CSL | SPI_CS_CSH )
#define SPI_CS_MODE_MASK	( SPI_CS_CPHA | SPI_CS_CPOL )

static u8* spi_mem = ( u8* ) 0;

unsigned int spi_hw_timeout = 1000;

static int spi_await_cs_flags_with_timeout( u32 flags ) {
	const unsigned long timeout = jiffies + ( spi_hw_timeout * HZ ) / 1000;
	while ( !( dma_get_flags32( spi_mem + SPI_CS, flags ) ) ) {
		if ( jiffies >= timeout ) {
			return SPI_ERR_HW_TIMEOUT;
		}
	}

	return 0;
}

int __init spi_init( void ) {
#if defined( DEBUG )
	LOG( KERN_DEBUG, "SPI setting GPIO modes for pins 7-11 to ALT0." );
#endif // DEBUG
	gpio_set_pin_mode(  7, GPIO_PIN_MODE_ALT0 );
	gpio_set_pin_mode(  8, GPIO_PIN_MODE_ALT0 );
	gpio_set_pin_mode(  9, GPIO_PIN_MODE_ALT0 );
	gpio_set_pin_mode( 10, GPIO_PIN_MODE_ALT0 );
	gpio_set_pin_mode( 11, GPIO_PIN_MODE_ALT0 );

#if defined( DEBUG )
	LOG( KERN_DEBUG, "SPI mapping IO memory into kernel virtual address space." );
#endif // DEBUG
	spi_mem = ( u8* ) ioremap( BCM2836_IO_MEM_START + SPI_OFFSET, SPI_SIZE );
	if ( !spi_mem ) {
		LOG( KERN_ERR, "SPI failed to map IO memory." );
		return SPI_ERR_IO_MAP_FAIL;
	}

#if defined( DEBUG )
	LOG( KERN_DEBUG, "SPI clearing CS register." );
#endif // DEBUG
	dma_write32( spi_mem + SPI_CS,  0x00000000 );
#if defined( DEBUG )
	LOG( KERN_DEBUG, "SPI reseting CLK register." );
#endif // DEBUG
	dma_write32( spi_mem + SPI_CLK, 0x00000000 );

	return 0;
}

void __exit spi_exit( void ) {
	if ( spi_mem ) {
#if defined( DEBUG )
		LOG( KERN_DEBUG, "SPI unmapping IO memory from kernel virtual address space." );
#endif // DEBUG
		iounmap( spi_mem );
		spi_mem = ( u8* ) 0;
	}

#if defined( DEBUG )
	LOG( KERN_DEBUG, "SPI setting GPIO modes for pins 7-11 to INPUT." );
#endif // DEBUG
	gpio_set_pin_mode(  7, GPIO_PIN_MODE_INPUT );
	gpio_set_pin_mode(  8, GPIO_PIN_MODE_INPUT );
	gpio_set_pin_mode(  9, GPIO_PIN_MODE_INPUT );
	gpio_set_pin_mode( 10, GPIO_PIN_MODE_INPUT );
	gpio_set_pin_mode( 11, GPIO_PIN_MODE_INPUT );
}

void spi_set_clk_div( u16 div ) {
#if defined( DEBUG )
	LOG( KERN_DEBUG, "SPI setting clock divider to 0x%04X.", div );
#endif // DEBUG
	dma_write16( spi_mem + SPI_CLK, div );
}

void spi_select_chip( u8 chip ) {
#if defined( DEBUG )
	LOG( KERN_DEBUG, "SPI setting chip select to 0x%02X.", chip );
#endif // DEBUG
	dma_clr_flags32( spi_mem + SPI_CS, SPI_CS_CS_MASK );
	dma_set_flags32( spi_mem + SPI_CS, chip & SPI_CS_CS_MASK );
}

void spi_set_mode( u8 mode ) {
#if defined( DEBUG )
	LOG( KERN_DEBUG, "SPI setting mode to 0x%02X.", mode );
#endif // DEBUG
	dma_clr_flags32( spi_mem + SPI_CS, SPI_CS_MODE_MASK );
	dma_set_flags32( spi_mem + SPI_CS, mode & SPI_CS_MODE_MASK );
}

void spi_enable_reads( void ) {
#if defined( DEBUG )
	LOG( KERN_DEBUG, "SPI enabling bus reads." );
#endif // DEBUG
	dma_set_flags32( spi_mem + SPI_CS, SPI_CS_REN );
}

void spi_disable_reads( void ) {
#if defined( DEBUG )
	LOG( KERN_DEBUG, "SPI disabling bus reads." );
#endif // DEBUG
	dma_clr_flags32( spi_mem + SPI_CS, SPI_CS_REN );
}

void spi_begin_transfer( void ) {
#if defined( DEBUG )
	LOG( KERN_DEBUG, "SPI beginning transfer." );
#endif // DEBUG
	dma_set_flags32( spi_mem + SPI_CS, SPI_CS_CLEAR_TX | SPI_CS_CLEAR_RX | SPI_CS_TA );
}

int spi_read_byte( u8* byte ) {
	int err;

#if defined( DEBUG )
	LOG( KERN_DEBUG, "SPI reading byte from bus." );
#endif // DEBUG
	err = spi_await_cs_flags_with_timeout( SPI_CS_RXD );
	if ( err ) {
		goto spi_read_err;
	}

	*byte = dma_read8( spi_mem + SPI_FIFO );

	return 0;

spi_read_err:
	switch ( err ) {
	case SPI_ERR_HW_TIMEOUT:
		LOG( KERN_ERR, "SPI hardware timout on RXD." );
		break;
	}
	return SPI_ERR_HW_TIMEOUT;
}

size_t spi_read( ssize_t len, u8* data ) {
	int err;

#if defined( DEBUG )
	LOG( KERN_DEBUG, "SPI reading %d bytes from bus.", len );
#endif // DEBUG
	size_t i = 0;
	for ( ; i < len; i++ ) {
		err = spi_await_cs_flags_with_timeout( SPI_CS_RXD );
		if ( err ) {
			goto spi_read_err;
		}

		data[i] = dma_read8( spi_mem + SPI_FIFO );
	}

	return i;

spi_read_err:
	switch ( err ) {
	case SPI_ERR_HW_TIMEOUT:
		LOG( KERN_ERR, "SPI hardware timout on RXD." );
		break;
	}
	return SPI_ERR_HW_TIMEOUT;
}

int spi_write_byte( u8 byte ) {
	int err;

#if defined( DEBUG )
	LOG( KERN_DEBUG, "SPI writing byte 0x%02X to bus.", byte );
#endif // DEBUG
	err = spi_await_cs_flags_with_timeout( SPI_CS_TXD );
	if ( err ) {
		goto spi_write_err;
	}

	dma_write8( spi_mem + SPI_FIFO, byte );

	return 0;

spi_write_err:
	switch ( err ) {
	case SPI_ERR_HW_TIMEOUT:
		LOG( KERN_ERR, "SPI hardware timout on TXD." );
		break;
	}
	return SPI_ERR_HW_TIMEOUT;
}

size_t spi_write( ssize_t len, const u8* data ) {
	int err;

#if defined( DEBUG )
	LOG( KERN_DEBUG, "SPI writing %d bytes to bus.", len );
#endif // DEBUG
	size_t i = 0;
	for ( ; i < len; i++ ) {
		err = spi_await_cs_flags_with_timeout( SPI_CS_TXD );
		if ( err ) {
			goto spi_write_err;
		}

		dma_write8( spi_mem + SPI_FIFO, data[i] );
	}

	return i;

spi_write_err:
	switch ( err ) {
	case SPI_ERR_HW_TIMEOUT:
		LOG( KERN_ERR, "SPI hardware timout on TXD." );
		break;
	}
	return SPI_ERR_HW_TIMEOUT;
}

int spi_await_transfer( void ) {
	int err;

#if defined( DEBUG )
	LOG( KERN_DEBUG, "SPI current thread synchronizing with SPI bus." );
#endif // DEBUG
	err = spi_await_cs_flags_with_timeout( SPI_CS_DONE );
	if ( err ) {
		goto spi_done_err;
	}

	return 0;

spi_done_err:
	switch ( err ) {
	case SPI_ERR_HW_TIMEOUT:
		LOG( KERN_ERR, "SPI hardware timout on DONE." );
		break;
	}
	return SPI_ERR_HW_TIMEOUT;
}

void spi_end_transfer( void ) {
#if defined( DEBUG )
	LOG( KERN_DEBUG, "SPI ending transfer." );
#endif // DEBUG
	dma_clr_flags32( spi_mem + SPI_CS, SPI_CS_TA );
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

