ccflags-y := -I$(src)
obj-m = vtgpio_test.o
KVERSION = $(shell uname -r)
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
