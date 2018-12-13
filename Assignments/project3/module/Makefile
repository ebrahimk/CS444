obj-m := sbd.o
KDIR := /scratch/spring2018/10/linux-yocto-3.19.2/
PWD_C := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD_C) modules
clean:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD_C) clean
