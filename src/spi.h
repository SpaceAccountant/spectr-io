#ifndef _SPECTR_IO_SPI_H
#define _SPECTR_IO_SPI_H

#include <linux/init.h>
#include <linux/types.h>

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
void spi_set_clk_div( u16 div );

/**
 * Sets the chip select line for SPI bus operations.
 * 
 * @param cs The chip.
 * 
 */
void spi_select_chip( u8 chip );

/**
 * Sets the clock phase and polrity of the SPI bus.
 * 
 * @param mode The mode defining the clock phase and polarity to use.
 * 
 */
void spi_set_mode( u8 mode );

/**
 * Enables reading from the SPI bus.
 *
 */
void spi_enable_reads( void );

/**
 * Disables reading from the SPI bus.
 *
 */
void spi_disable_reads( void );

/**
 * Begins an SPI bus data transfer.
 * 
 */
void spi_begin_transfer( void );

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
void spi_end_transfer( void );

#endif // _SPECTR_IO_SPI_H

