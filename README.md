# emb-vt
Kernel Module for enabling virtual time in distributed heterogeneous embedded linux environment

About: 1GB RAM 1Ghz 2 cores

Environment:

cd /home/emb-vt

git pull 

src is the source for the kernel Module

kernel contains the source for the modified kernel 


## Kernel Module

The Kernel Module is a wrapper to the virtual time kernel functionality. The module also is necessary for the distribution of the pause / unpause (freeze / unfreeze) across computers.

inside the src folder

`make` compiles the module 

`insmod vtgpio_test.ko &` loads the kernel module

`rmmod vtgpio_test` unloads the kernel module 

`lsmod` lists the kernel modules currently loaded

`dmesg` prints the kernel log  i.e.,  everything that has been written with `printk()`

`dmesg | tail` is useful or `dmesg | grep VT-GPIO`

`/sys/vt/VT7/` contains `mode`, `tdf`, `pid_01-16`

These file system entries are called kobjects or something like this, when the kernel module is loaded it creates these entries. There is a handler registered to each entry. When the value of an entry is modified, the assiciated handler is eventually run (pending OS scheduling). It should be possible to locate the handlers inside the kernel module source code. 

`mode` triggers the freeze and unfreeze routines inside the kernel module

by typing `echo $PID > /sys/vt/VT7/pid_01` you have entered a pid $PID into virtual time 

If you recieve an error can not open `/proc/$PID/dilation` in `dmesg` then likely the process does not exist or else it may be a lightweight thread which does not have file system entries for it.

try creating an infinite loop program in python with large sleeps (to keep the cpu free) as a dummy program

`cat /sys/vt/VT7/mode` should initially return `unfreeze` by typing `echo 'freeze' > /sys/vt/VT7/mode` you initiate the handler in the kernal module. to unfreeze just echo unfreeze.

tdf value stands for time dialation factor times 1000. In our case this value should just stay at the default of 1000. (changing it to 2000 would make the process think time is going 2x speed)


### Distributed part

In the case of multiple boards, there are two interrupt handlers, they are used for pause and resume ( just like /sys/vt/VT7/mode is) when the mode is changed on one board, the kernel module also sends out high signals on the pin that connects the boards. The interrupt handlers run the same routine as if their mode was changed locally. 

## Kernel modifications

the timekeeping library is modified to calculate the offset of the virtual clock to the wall clock time.

you can see the changes by running diff on the modified files vs the files found by default when installing the kernel source 

A good Description can be found in the virtual time by jiaqi Yan  paper in ACM PADS conference 2015 and in my DSSnet paper PADS conference 2016 in the implementation section 


Basically the kernel module is a wrapper that modifies the /proc/$PID/ file system and changes hardware interrupt pin voltages.


## runthrough

- app A is created
- app A is added to virtual time ( `echo $PID > /sys/vt/VT7/pid_01`)
- app A runs for a while
- app B decides that the virtual time processes should be paused ( `echo 'freeze' > /sys/vt/VT7/mode`)
- kernel module will call the handler (mode_store) function which will call freeze or unfreeze ( and change the pins for the distributed part -- board to board-- )
- the freeze/unfreeze function writes the proper attribute into the process attributes in the /proc/$PID/... file system which the kernel can then process
- the kernel module exits
- when the kernel reads the /proc/$PID/ entries ( I think when the OS scheduler schedules the proc) the kernel will freeze the app A
- App A is now frozen

- after resuming, if app A calls gettimeofday system call, it will see the wallclock time - the amount of time frozen for .

## Distributed algorithm stuff

pin arrangement

xxxxxxooooxxx
      
xxxxooooooxxx 

ooooooooooooo

grobooooooooo
