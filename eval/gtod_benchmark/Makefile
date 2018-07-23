#obj-m = vtgpio_test.o
#KVERSION = $(shell uname -r)
all:
	gcc -O0 gtod.c -o gtod
	gcc -O0 gtod_loop.c -o gtod_loop
clean:
	rm gtod gtod_loop
