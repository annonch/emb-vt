/**
 * @file   vt.c
 * @author Christopher Hannon
 * @date   02 October 2016
 * @brief  A kernel module for enabling virtual time.
 * The sysfs entry appears at /sys/??
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>       // Required for the GPIO functions
#include <linux/kobject.h>    // Using kobjects for the sysfs bindings
#include <linux/kthread.h>    // Using kthreads for the flashing functionality

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Christopher Hannon");
MODULE_DESCRIPTION("vt");
MODULE_VERSION("0.1");



