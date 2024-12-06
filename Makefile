ifneq ($(KERNELRELEASE),)
	EXTRA_CFLAGS := -I$(PWD)/src -I$(SPECTR_COMMON)/src
	obj-m := spectr_io.o
	spectr_io-y := src/gpio.o src/main.o src/spi.o

else
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) PWD=$(PWD) modules

clean:
	rm *.mod*
	rm src/*.o src/.*.cmd .*.cmd *.o *.ko Module.symvers modules.order

endif

