#include <linux/init.h>
#include <linux/module.h>

#include "gpio.h"
#include "i2c.h"
#include "spi.h"

static int __init spectre_io_init( void ) {
	int err;

	err = gpio_init();
	if ( err ) {
		return err;
	}
	err = spi_init();
	if ( err ) {
		return err;
	}
	err = i2c1_init();
	if ( err ) {
		return err;
	}

	return 0;
}

static void __exit spectre_io_exit( void ) {
	i2c1_exit();
	spi_exit();
	gpio_exit();
}

MODULE_LICENSE( "GPL" );

module_init( spectre_io_init );
module_exit( spectre_io_exit );

