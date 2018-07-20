#!/bin/bash

######### Install the follwoing packages if needed
######### apt-get install build-essential u-boot-tools
######### apt-get install libusb-1.0-0 libusb-1.0-0-dev git wget fakeroot kernel-package zlib1g-dev libncurses5-dev

#echo "Step 0. download source kernel"

#rm -r /home/emb-vt/kernel/linux-bananapi
#cd /home/emb-vt/kernel
#git clone https://github.com/Bananian/linux-bananapi.git --depth 1

#FILES="kernel/fork.c		        \
#kernel/time.c			        \
#kernel/time/timekeeping.c	        \
#include/linux/sched.h		        \
#include/linux/init_task.h	        \
#include/linux/time.h		        \
#include/linux/timekeeper_internal.h     \
#include/linux/init_task.h               \
#fs/proc/base.c"

#echo "Step 1. Transfer modified kernel source"
#for f in $FILES; do
#        cp -v /home/emb-vt/kernel/bananaVT/$f /home/emb-vt/kernel/linux-bananapi/$f
#done

echo "Step 2. Build new kernel"
cd /home/emb-vt/kernel/linux-bananapi

#make sun7i_defconfig
#make menuconfig
make -j2 uImage modules

make modules_install
mount /dev/mmcblk0p1 /mnt
mv /mnt/uImage /mnt/uImage_old  # save precedent uImage, can be restored
cp arch/arm/boot/uImage /mnt
sync
######################### umount /mnt
reboot
