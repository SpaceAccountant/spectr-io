#include "i2c.h"

#include <asm/io.h>
#include <linux/jiffies.h>
#include <linux/module.h>

#include "dma.h"
#include "gpio.h"

#define I2C1_OFFSET	0x00804000
#define I2C_SIZE	0x20

#define I2C_C		0x00
#define I2C_S		0x04
#define I2C_DLEN	0x08
#define I2C_A		0x0C
#define I2C_FIFO	0x10
#define I2C_DIV		0x14
#define I2C_DEL		0x18
#define I2C_CLKT	0x1C

#define I2C_C_READ	BIT(  0 )
#define I2C_C_CLEARL	BIT(  4 )
#define I2C_C_CLEARH	BIT(  5 )
#define I2C_C_ST	BIT(  7 )
#define I2C_C_INTD	BIT(  8 )
#define I2C_C_INTT	BIT(  9 )
#define I2C_C_INTR	BIT( 10 )
#define I2C_C_EN	BIT( 15 )

#define I2C_S_TA	BIT( 0 )
#define I2C_S_DONE	BIT( 1 )
#define I2C_S_TXW	BIT( 2 )
#define I2C_S_RXR	BIT( 3 )
#define I2C_S_TXD	BIT( 4 )
#define I2C_S_RXD	BIT( 5 )
#define I2C_S_TXE	BIT( 6 )
#define I2C_S_RXF	BIT( 7 )
#define I2C_S_ERR	BIT( 8 )
#define I2C_S_CLKT	BIT( 9 )

#define I2C_DEL_REDL_OFF	0
#define I2C_DEL_FEDL_OFF	16

unsigned int i2c1_hw_timeout = 1000;

static u8* i2c1_mem = ( u8* ) 0;

static int i2c1_await_flags_or_timeout( int reg, u32 flags ) {
	unsigned long timeout = jiffies + ( i2c1_hw_timeout * HZ ) / 1000;
	while ( !dma_get_flags32( i2c1_mem + reg, flags ) ) {
		if ( dma_get_flags32( i2c1_mem + I2C_S, I2C_S_ERR ) ) {
			return I2C_ERR_NO_RESPONSE;
		}
		if ( dma_get_flags32( i2c1_mem + I2C_S, I2C_S_CLKT ) ) {
			return I2C_ERR_CLK_TIMEOUT;
		}
		if ( jiffies >= timeout ) {
			return I2C_ERR_HW_TIMEOUT;
		}
	}
	return 0;
}

void i2c1_set_clk_div( unsigned short clk_div ) {
	dma_write16( i2c1_mem + I2C_DIV, clk_div );

	// Set the data delay values based on the new clock divider to prevent strange behavior
	dma_write32( i2c1_mem + I2C_DEL,
		( max( clk_div >> 4, 1 ) << I2C_DEL_FEDL_OFF )
	      | ( max( clk_div >> 2, 1 ) << I2C_DEL_REDL_OFF ) );
}

void i2c1_set_addr( unsigned char addr ) {
	dma_write8( i2c1_mem + I2C_A, addr & 0x7F );
}

size_t i2c1_read_register( unsigned char reg, ssize_t len, u8* data ) {
	int err;

	// Reset errors, clear the FIFO, and enable the BSC
	dma_set_flags32( i2c1_mem + I2C_S, I2C_S_DONE | I2C_S_ERR | I2C_S_CLKT );
	dma_set_flags32( i2c1_mem + I2C_C, I2C_C_CLEARL | I2C_C_CLEARH | I2C_C_EN );

	// Set up a write transfer of one byte to send the register ID
	dma_write16( i2c1_mem + I2C_DLEN, 1 );
	dma_clr_flags32( i2c1_mem + I2C_C, I2C_C_READ );
	dma_set_flags32( i2c1_mem + I2C_C, I2C_C_ST );

	// Wait for the transfer to start
	err = i2c1_await_flags_or_timeout( I2C_S, I2C_S_TA );
	if ( err ) {
		goto i2c_err;
	}

	// Wait for the TX FIFO to have space
	err = i2c1_await_flags_or_timeout( I2C_S, I2C_S_TXD );
	if ( err ) {
		goto i2c_err;
	}

	// Send the register ID
	dma_write8( i2c1_mem + I2C_FIFO, reg );

	// Await for the register ID transfer to finish
	err = i2c1_await_flags_or_timeout( I2C_S, I2C_S_DONE );
	if ( err ) {
		goto i2c_err;
	}

	// Set up a read transfer of len number of bytes to receive the register data
	dma_write16( i2c1_mem + I2C_DLEN, len );
	dma_set_flags32( i2c1_mem + I2C_C, I2C_C_ST | I2C_C_READ );

	// Wait for the transfer to start
	err = i2c1_await_flags_or_timeout( I2C_S, I2C_S_TA );
	if ( err ) {
		goto i2c_err;
	}

	// Read bytes until the specified number of bytes is read or the transfer finishes
	size_t i = 0;
	while ( i < len && !dma_get_flags32( i2c1_mem + I2C_S, I2C_S_DONE ) ) {
		// Await the RX FIFO to have data
		err = i2c1_await_flags_or_timeout( I2C_S, I2C_S_RXD );
		if ( err ) {
			goto i2c_err;
		}

		// Read the next byte of register data
		data[i] = dma_read8( i2c1_mem + I2C_FIFO );

		i++;
	}

	// Disable the BSC
	dma_clr_flags32( i2c1_mem + I2C_C, I2C_C_EN );

	return i;

i2c_err:
	// Disable the BSC
	dma_clr_flags32( i2c1_mem + I2C_C, I2C_C_EN );

	return err;
}

size_t i2c1_read( ssize_t len, u8* data ) {
	int err = 0;

	// Reset errors, clear the FIFO, and enable the BSC
	dma_set_flags32( i2c1_mem + I2C_S, I2C_S_DONE | I2C_S_ERR | I2C_S_CLKT );
	dma_set_flags32( i2c1_mem + I2C_C, I2C_C_CLEARL | I2C_C_CLEARH | I2C_C_EN );

	// Set up a read transfer of len number of bytes to receive the data
	dma_write16( i2c1_mem + I2C_DLEN, len );
	dma_clr_flags32( i2c1_mem + I2C_C, I2C_C_ST | I2C_C_READ );

	// Wait for the transfer to start
	err = i2c1_await_flags_or_timeout( I2C_S, I2C_S_TA );
	if ( err ) {
		goto i2c_err;
	}

	// Read bytes until the specified number of bytes is read or the transfer finishes
	size_t i = 0;
	while ( i < len && !dma_get_flags32( i2c1_mem + I2C_S, I2C_S_DONE ) ) {
		// Await the RX FIFO to have data
		err = i2c1_await_flags_or_timeout( I2C_S, I2C_S_RXD );
		if ( err ) {
			goto i2c_err;
		}

		// Read the next byte of data
		data[i] = dma_read8( i2c1_mem + I2C_FIFO );

		i++;
	}

	// Disable the BSC
	dma_clr_flags32( i2c1_mem + I2C_C, I2C_C_EN );

	return i;

i2c_err:
	// Disable the BSC
	dma_clr_flags32( i2c1_mem + I2C_C, I2C_C_EN );

	return err;
}

size_t i2c1_write( ssize_t len, const u8* data ) {
	int err = 0;

	// Reset errors, clear the FIFO, and enable the BSC
	dma_set_flags32( i2c1_mem + I2C_S, I2C_S_DONE | I2C_S_ERR | I2C_S_CLKT );
	dma_set_flags32( i2c1_mem + I2C_C, I2C_C_CLEARL | I2C_C_CLEARH | I2C_C_EN );

	// Set up a write transfer of one len number of bytes to write the data
	dma_write16( i2c1_mem + I2C_DLEN, len );
	dma_set_flags32( i2c1_mem + I2C_C, I2C_C_ST );

	// Wait for the transfer to start
	err = i2c1_await_flags_or_timeout( I2C_S, I2C_S_TA );
	if ( err ) {
		goto i2c_err;
	}

	// Write bytes until the specified number of bytes is written or the transfer finishes
	size_t i = 0;
	while ( i < len && !dma_get_flags32( i2c1_mem + I2C_S, I2C_S_DONE ) ) {
		// Await the TX FIFO to have space
		err = i2c1_await_flags_or_timeout( I2C_S, I2C_S_TXD );
		if ( err ) {
			goto i2c_err;
		}

		// Write the next byte of data
		dma_write8( i2c1_mem + I2C_FIFO, data[i] );

		i++;
	}

	// Disable the BSC
	dma_clr_flags32( i2c1_mem + I2C_C, I2C_C_EN );

	return i;

i2c_err:
	// Disable the BSC
	dma_clr_flags32( i2c1_mem + I2C_C, I2C_C_EN );

	return err;
}

int __init i2c1_init( void ) {
	gpio_set_pin_mode( 2, GPIO_PIN_MODE_ALT0 );
	gpio_set_pin_mode( 3, GPIO_PIN_MODE_ALT0 );

	i2c1_mem = ( u8* ) ioremap( BCM2836_IO_MEM_START + I2C1_OFFSET, I2C_SIZE );
	if ( !i2c1_mem ) {
		return I2C_ERR_IO_MAP_FAIL;
	}

	dma_write32( i2c1_mem + I2C_C,    0x00000000 );
	dma_write32( i2c1_mem + I2C_DIV,  0x000005DC );
	dma_write32( i2c1_mem + I2C_DEL,  0x00300030 );
	dma_write32( i2c1_mem + I2C_CLKT, 0x00000040 );

	return 0;
}

void __exit i2c1_exit( void ) {
	if ( i2c1_mem ) {
		iounmap( i2c1_mem );
		i2c1_mem = ( u8* ) 0;
	}

	gpio_set_pin_mode( 2, GPIO_PIN_MODE_INPUT );
	gpio_set_pin_mode( 3, GPIO_PIN_MODE_INPUT );
}

EXPORT_SYMBOL( i2c1_hw_timeout );
EXPORT_SYMBOL( i2c1_set_clk_div );
EXPORT_SYMBOL( i2c1_set_addr );
EXPORT_SYMBOL( i2c1_read_register );
EXPORT_SYMBOL( i2c1_read );
EXPORT_SYMBOL( i2c1_write );

