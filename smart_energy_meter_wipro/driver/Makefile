obj-m += smartmeter_driver.o

KDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
	@if [ ! -d "$(KDIR)" ]; then echo "ERROR: kernel build tree not found: $(KDIR)"; exit 2; fi
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
