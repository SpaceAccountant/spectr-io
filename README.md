Overview
====
The SPECTR IO project is a sub-project of SPECTR. It provide a wrapper for low level IO communication. This project requires the SPECTR Common project.

Requirements
====
The current version of SPECTR and its sub-projects run on a Broadcom BCM2836 based system running Linux, specifically a Raspberry Pi 2B+.

Building
====
To build the project you can use the provided Makefile so long as you are building on the target system and have the kernel source tree under `/lib/modules/$(uname -r)/build`. If you have the source tree elsewhere just modify the KERNELDIR variable in the Makefile to reflect this. If you're cross compiling then I'm not explaining how to set it up.

You must provide an additional variable at the command line, `SPECTR_COMMON`, which points to the directory (without trailing slash) that the SPECTR Common project root is located.

Running
====
Before running the project you first should make sure there are no other persistent user-space drivers loaded for GPIO, SPI, or I2C1. If you have a driver such as `bcm2835_i2c` then leave that loaded as it's necessary for EEPROM and HDMI functionality, but drivers that create interfaces such as `/dev/gpio`, `/dev/spi0`, or `/dev/i2c1` need to be unloaded. To actually run the project after building you can simply run `insmod spectr_io.ko` as root.

Installing
====
As said in the **Running** section, you cannot use this with persistent user-space drivers. As such, you will want to not only unload these drivers, but blacklist them using configuration files for your distribution. Once done, put the module somewhere under `/lib/modules/$(uname -r)/kernel/drivers`, I recommend under `/lib/modules/$(uname -r)/kernel/drivers/spectr/io`. Then you can do modify the necessary configuration files for your distro to load it at boot.

