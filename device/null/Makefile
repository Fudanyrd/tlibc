# source: <https://www.apriorit.com/dev-blog/195-simple-driver-for-linux-os>
TARGET_MODULE := null

# If we are running by kernel building system
ifneq ($(KERNELRELEASE),)
	$(TARGET_MODULE)-objs := init.o
	obj-m := $(TARGET_MODULE).o

# If we running without kernel build system
else
	BUILDSYSTEM_DIR := /mnt/d/raw-pj/linux/linux-6.11.2
	PWD:=$(shell pwd)

# run kernel build system to make module
all : 
	$(MAKE) -C $(BUILDSYSTEM_DIR) M=$(PWD) modules

# run kernel build system to cleanup in current directory
clean:
	$(MAKE) -C $(BUILDSYSTEM_DIR) M=$(PWD) clean

.PHONY: install
install: install.sh $(TARGET_MODULE).ko
	./install.sh $(TARGET_MODULE)
endif
