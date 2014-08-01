ARCH=arm
CROSS_COMPILE=arm-linux-gnueabihf-

# TODO - replace this with your Linux kernel location
KDIR := /home/git/odroid-3.x.y

PWD := $(shell pwd)

obj-m += udiv_bdsn.o

all:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KDIR) M=$(PWD) modules

clean:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS) -C $(KDIR) M=$(PWD) clean
