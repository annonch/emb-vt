#obj-m = vtgpio_test.o
#KVERSION = $(shell uname -r)
all:
	gcc gtod.c -o gtod
	gcc gtod_loop.c -o gtod_loop
clean:
	rm gtod gtod_loop
