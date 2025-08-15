obj-m := dma_transfer.o
KVERSION := $(shell uname -r)
KDIR := /lib/modules/$(KVERSION)/build
PWD := $(shell pwd)

# Disable all security checks
CONFIG_MODULE_SIG=n
CONFIG_MODVERSIONS=n

# Compiler flags
EXTRA_CFLAGS += -Wall -Wno-error

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean


