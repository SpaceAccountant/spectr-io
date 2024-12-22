#ifndef _SPECTRE_IO_I2C_H
#define _SPECTRE_IO_I2C_H

#include <linux/bitops.h>
#include <linux/init.h>

#define I2C_ERR_IO_MAP_FAIL	-1	// Mapping IO memory into kernel virtual memory failed.
#define I2C_ERR_HW_TIMEOUT	-2	// The configured hardware timeout was reached during and
					// operation.
#define I2C_ERR_NO_RESPONSE	-3	// No I2C device acknowleged the address.
#define I2C_ERR_CLK_TIMEOUT	-4	// The addressed I2C device held the clock signal low for
					// longer than the configured clock timeout.

// The max number of milliseconds to wait for a hardware operation.
extern unsigned int i2c1_hw_timeout;

/**
 * Initializes the I2C1 subsystem.
 *
 * @returns Zero on success; a negative error code on failure.
 *
 */
extern int __init i2c1_init( void );

/**
 * Destroys the I2C1 subsystem.
 *
 */
extern void __exit i2c1_exit( void );

/**
 * Sets the clock divider of the I2C1 bus.
 *
 * @param div The divider.
 *
 */
void i2c1_set_clk_div( unsigned short clk_div );

/**
 * Sets the peripheral address of the I2C1 bus.
 *
 * @param addr The address.
 *
 */
void i2c1_set_addr( unsigned char addr );

/**
 * Reads a register from the I2C1 bus.
 *
 * @param reg The register.
 * @param len The length of the register data to read in bytes.
 * @param data The buffer to read data into.
 *
 * @returns The number of bytes read.
 *
 */
size_t i2c1_read_register( unsigned char reg, ssize_t len, u8* data );

/**
 * Reads data from the I2C1 bus.
 *
 * @param len The length of the trasaction in bytes.
 * @param data The buffer to read data into.
 *
 * @returns The number of bytes read.
 *
 */
size_t i2c1_read( ssize_t len, u8* data );

/**
 * Writes data to the I2C1 bus.
 *
 * @param len The length of the trasaction in bytes.
 * @param data The buffer to send data from.
 *
 * @returns The number of bytes written.
 *
 */
size_t i2c1_write( ssize_t len, const u8* data );

#endif // _SPECTRE_IO_I2C_H

