# Makefile

# デバイス
DEVROOT = /dev
DEVNAME = $(DEVROOT)/chardev

# コマンド
INSTALL = install -cp
RM = rm -rf
CHMOD = chmod
MKNOD = mknod
INSMOD = insmod
LSMOD = lsmod
RMMOD = rmmod

# カーネルオブジェクト
KOBJ = char

SRCS = chardev.c ring.c log.c
obj-m := $(KOBJ).o
$(KOBJ)-objs := $(SRCS:.c=.o)
clean-files := *.o *.ko *.mod.[co] modules.order

INC   := /usr/src/kernels/$(shell uname -r)/include
KDIR  := /lib/modules/$(shell uname -r)/build
PWD   := $(shell pwd)
MINOR := 0 1

.PHONY: all
all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

.PHONY: clean
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

.PHONY: insmod
insmod:
	@test "`$(LSMOD) | grep $(KOBJ)`x" = "x" || $(RMMOD) $(KOBJ)
	@$(INSMOD) $(KOBJ).ko
	@$(LSMOD) | grep $(KOBJ)
	@for i in $(MINOR); do                     \
        $(RM) $(DEVNAME)$$i;                  \
        major=`awk -v mod="chardev" '$$2==mod {print $$1}' /proc/devices`; \
        $(MKNOD) $(DEVNAME)$$i c $$major $$i; \
        $(CHMOD) 0666 $(DEVNAME)$$i; done
	@ls $(DEVNAME)*

.PHONY: help
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... insmod"
	@echo "... help"

