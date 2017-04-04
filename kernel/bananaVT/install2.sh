#!/bin/bash

DST=/home/build_kernel/bananapi-kernel

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

