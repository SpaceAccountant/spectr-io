#ifndef _SPECTRE_IO_SPI_H
#define _SPECTRE_IO_SPI_H

#include <linux/bitops.h>
#include <linux/init.h>
#include <linux/jiffies.h>

#include <dma.h>

#define SPI_ERR_IO_MAP_FAIL	-1
#define SPI_ERR_HW_TIMEOUT	-2

#define SPI_CHIP0	0
#define SPI_CHIP1	SPI_CS_CSL

// CLK rest low, CLK transition at middle of data bit
#define SPI_MODE0	0
// CLK rest low, CLK transition at beginning of data bit
#define SPI_MODE1	SPI_CS_CPHA
// CLK rest high, CLK transition at middle of data bit
#define SPI_MODE2	SPI_CS_CPOL
// CLK rest high, CLK transition at beginning of data bit
#define SPI_MODE3	( SPI_CS_CPHA | SPI_CS_CPOL )

// The timeout for the SPI hardware in milliseconds.
extern unsigned int spi_hw_timeout;

/**
 * Initializes the SPI subsystem.
 * 
 * @returns Zero on success; a negative error code on failure.
 * 
 */
int __init spi_init( void );

/**
 * Destorys the SPI subsystem.
 * 
 */
void __exit spi_exit( void );

/**
 * Sets the system clock divider the SPI bus will use.
 * 
 * @param div The divider.
 * 
 */
#define spi_set_clk_div( div ) {									\
	dma_write16( spi_mem + SPI_CLK, ( div ) );							\
}

/**
 * Sets the chip select line for SPI bus operations.
 * 
 * @param cs The chip.
 * 
 */
#define spi_select_chip( chip ) {									\
	dma_clr_flags32( spi_mem + SPI_CS, SPI_CS_CSMASK );						\
	dma_set_flags32( spi_mem + SPI_CS, ( chip ) & SPI_CS_CSMASK );					\
}

/**
 * Sets the clock phase and polrity of the SPI bus.
 * 
 * @param mode The mode defining the clock phase and polarity to use.
 * 
 */
#define spi_set_mode( mode ) {										\
	dma_clr_flags32( spi_mem + SPI_CS, SPI_CS_MODEMASK );						\
	dma_set_flags32( spi_mem + SPI_CS, ( mode ) & SPI_CS_MODEMASK );				\
}

/**
 * Enables reading from the SPI bus.
 *
 */
#define spi_enable_reads() {										\
	dma_set_flags32( spi_mem + SPI_CS, SPI_CS_REN );						\
}

/**
 * Disables reading from the SPI bus.
 *
 */
#define spi_disable_reads() {										\
	dma_clr_flags32( spi_mem + SPI_CS, SPI_CS_REN );						\
}

/**
 * Begins an SPI bus data transfer.
 * 
 */
#define spi_begin_transfer() {										\
	dma_set_flags32( spi_mem + SPI_CS, SPI_CS_CLEAR_TX						\
					 | SPI_CS_CLEAR_RX						\
					 | SPI_CS_TA );							\
}

/**
 * Reads a byte from the SPI bus.
 *
 * @param byte The byte.
 *
 */
int spi_read_byte( u8* byte );

/**
 * Reads data from the SPI bus.
 * 
 * @param len The number of bytes to read.
 * @param data The data buffer to read to.
 * 
 */
size_t spi_read( ssize_t len, u8* data );

/**
 * Writes a byte to the SPI bus.
 *
 * @param byte The byte.
 *
 */
int spi_write_byte( u8 byte );

/**
 * Writes data to the SPI bus.
 * 
 * @param len The number of bytes to write.
 * @param data The data buffer to write from.
 * 
 */
size_t spi_write( ssize_t len, const u8* data );

/**
 * Synchronizes the program with the SPI bus.
 *
 */
int spi_await_transfer( void );

/**
 * Ends an SPI bus data transfer.
 * 
 */
#define spi_end_transfer() {										\
	dma_clr_flags32( spi_mem + SPI_CS, SPI_CS_TA );							\
}

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

extern u8* spi_mem;

#endif // _SPECTRE_IO_SPI_H

