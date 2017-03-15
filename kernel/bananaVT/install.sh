#!/bin/bash

DST=~/build_kernel

if [ -e $DST ]; then
        echo "Clean up previous build"
        sudo rm -rf $DST
fi

if [ ! -e bananapi-3.4.zip ]; then
        wget https://github.com/Bananian/linux-bananapi/archive/bananapi-3.4.zip
fi
echo "Step 0. Unpack kernel source"
unzip linux-bananapi-bananapi-3.4.zip
mv linux-bananapi-bananapi-3.4 $DST

# generate patch file
PATCH=VirtualTime.patch
echo "        Patch file written to ${PATCH}"
diff -rup $DST ./ > ${PATCH}
echo ""

FILES="kernel/fork.c		        \
kernel/time.c			        \
kernel/time/timekeeping.c	        \
include/linux/sched.h		        \
include/linux/init_task.h	        \
include/linux/time.h		        \
include/linux/timekeeper_internal.h     \
fs/proc/base.c			        \
Makefile		                \
build_all.sh"

echo "Step 1. Transfer modified kernel source"
for f in $FILES; do
        cp -v $f $DST/$f
done

echo "Step 2. Build new kernel"
cd $DST

sudo ./build_all.sh
