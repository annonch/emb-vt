/**
 * Virtual Time Kernel Module
 * For Heterogeneous Distributed
 * Embedded Linux Environment
 * Author: Christopher Hannon, Brian Liu
 **/
#include <asm/segment.h>
#include <asm/uaccess.h>
#include <linux/buffer_head.h>
#include <linux/fs.h>
#include <linux/gpio.h> // gpio stuff
#include <linux/init.h>
#include <linux/interrupt.h> // irq  code
#include <linux/kernel.h>    // :)
#include <linux/kobject.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/sched.h>

#include "vtgpio.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Christopher Hannon");
MODULE_DESCRIPTION("Distributed Virtual time synchronization module");
MODULE_VERSION("0.4");

static unsigned int gpioSIG = 7;   // Using CE1 to output high or low
static unsigned int gpioSIG2 = 8;  // Listen to rising edge (pause)
static unsigned int gpioSIG3 = 24; // Listen to falling edge (resume)

/**
 * Brian: CE1 to output high or low, GPIO5 to listening for falling | 2018Mar8th
 * Deprecated:
 *     gpio 21 on 2B static
 *     try 25 to see if pin broken 7; // pin for talking // gpio21 on model b+
 *     I think 7 and 8 for banana pi 21 and 20 for raspberry pi
 *
 * To install use insmod gpio_test.ko and lsmod to see the module
 * To uninstall use rmmod vtgpio_test and lsmod to confirm
 * to pause write freeze/unfreeze to ....
 * /sys/vt/VT7/mode i.e. echo 'freeze' > /sys/vt/VT7/mode
 */
unsigned int active = 1;
static unsigned int num_ints = 0;
static unsigned int irqNumber;
static unsigned int irqNumber2;

static enum modes mode = DISABLED;
static int all_pids[MAX_NUM_PIDS] = {0};

static int sequential_io(enum IO io);
static int sequential_io_round_robin(enum IO io);

// /* PID variables for tracking processes in VT */
static int pid_01 = 0;
static int pid_02 = 0;
static int pid_03 = 0;
static int pid_04 = 0;
static int pid_05 = 0;
static int pid_06 = 0;
static int pid_07 = 0;
static int pid_08 = 0;
static int pid_09 = 0;
static int pid_10 = 0;
static int pid_11 = 0;
static int pid_12 = 0;
static int pid_13 = 0;
static int pid_14 = 0;
static int pid_15 = 0;
static int pid_16 = 0;

/* variable to ensure round robin pause/resume ops */
unsigned int round_robin = 0;

/* default time dilation factor for clocks (x1000) see various references for
 * more details */
static int tdf = 1000;
static s64 freeze_now = 0;
/* name of filesystem accessable from user space */
static char vtName[6] = "vtXXX";

/* core function for pausing */
void pause(void) {
  // tracing_on();
#ifdef BENCHMARK
  unsigned long long OHseconds;
  unsigned long long OHns;
  struct timespec seconds;
  struct timespec seconds_end;

  getnstimeofday(&seconds);

#ifndef QUIET
  VT_PRINTK("VT_TEST_PAUSE\n");
  VT_PRINTK("VT-GPIO_TIME: TIME-RISE: %llu %llu nanoseconds",
            (unsigned long long)seconds.tv_sec,
            (unsigned long long)seconds.tv_nsec);
  VT_PRINTK("VT-GPIO_TEST: Rising Edge detected");
#endif // QUIET
#endif // BENCHMARK

  /* read list of pids */
  /* kickoff kthreads to resume processes */
  num_ints++;

#ifdef ROUND_ROBIN
  sequential_io_round_robin(FREEZE);
#else
  sequential_io(FREEZE);
#endif // ROUND_ROBIN

#ifdef BENCHMARK
  getnstimeofday(&seconds_end);

  if (seconds_end.tv_nsec > seconds.tv_nsec) {
    OHseconds = seconds_end.tv_sec - seconds.tv_sec;
    OHns = seconds_end.tv_nsec - seconds.tv_nsec;
  } else {
    OHseconds = seconds_end.tv_sec - 1 - seconds.tv_sec;
    OHns = seconds_end.tv_nsec - (SEC_NSEC + seconds.tv_nsec);
  }
  printk(KERN_INFO "VT-GPIO_BENCHMARK: Pause ; %llu ; %llu ",
         ((unsigned long long)OHseconds), ((unsigned long long)OHns));
#ifndef QUIET
  VT_PRINTK("VT-GPIO_TIME: TIME-RISE: %llu %llu nanoseconds",
            (unsigned long long)seconds_end.tv_sec,
            (unsigned long long)seconds_end.tv_nsec);
  VT_PRINTK("VT-GPIO_TIME: TIME-PAUSE: %llu %llu nanoseconds",
            ((unsigned long long)seconds_end.tv_sec -
             (unsigned long long)seconds.tv_sec),
            ((unsigned long long)seconds_end.tv_nsec -
             (unsigned long long)seconds.tv_nsec));
#endif // QUIET
#endif // BENCHMARK
}

/** @brief Function to resume pids in VT */
void resume(void) {
#ifdef BENCHMARK
  unsigned long long OH_R_seconds;
  unsigned long long OH_R_ns;
  struct timespec seconds;
  struct timespec seconds_end;

  getnstimeofday(&seconds);

#ifndef QUIET
  VT_PRINTK("VT_TEST_RESUME\n");
  VT_PRINTK("VT-GPIO_TIME: TIME-FALL: %llu %llu nanoseconds",
            (unsigned long long)seconds.tv_sec,
            (unsigned long long)seconds.tv_nsec);
  VT_PRINTK("VT-GPIO_TEST: Falling Edge detected");
#endif // QUIET
#endif // BENCHMARK

  num_ints++;

#ifdef ROUND_ROBIN
  sequential_io_round_robin(RESUME);
#else
  sequential_io(RESUME);
#endif // ROUND_ROBIN

#ifdef BENCHMARK
  getnstimeofday(&seconds_end);

  if (seconds_end.tv_nsec > seconds.tv_nsec) {
    OH_R_seconds = seconds_end.tv_sec - seconds.tv_sec;
    OH_R_ns = seconds_end.tv_nsec - seconds.tv_nsec;
  } else {
    OH_R_seconds = seconds_end.tv_sec - 1 - seconds.tv_sec;
    OH_R_ns = seconds_end.tv_nsec - (SEC_NSEC + seconds.tv_nsec);
  }
  printk(KERN_INFO "VT-GPIO_BENCHMARK: Resume ; %llu ; %llu ",
         ((unsigned long long)OH_R_seconds), ((unsigned long long)OH_R_ns));
#ifndef QUIET
  VT_PRINTK("VT-GPIO_TIME: TIME-FALL: %llu %llu nanoseconds",
            (unsigned long long)seconds_end.tv_sec,
            (unsigned long long)seconds_end.tv_nsec);
  VT_PRINTK("VT-GPIO_TIME: TIME-RESUME: %llu %llu nanoseconds",
            ((unsigned long long)seconds_end.tv_sec -
             (unsigned long long)seconds.tv_sec),
            ((unsigned long long)seconds_end.tv_nsec -
             (unsigned long long)seconds.tv_nsec));
#endif // QUIET
#endif // BENCHMARK
}

/** @brief Function to add pids to VT */
static int dilate_proc(int pid) {
  int ret = 0;
  char tdf_str[TDF_STR_LEN];

  ret = sprintf(tdf_str, "%d", tdf);
  write_proc_field((pid_t)pid, "dilation", tdf_str);

  VT_PRINTK("VT-GPIO_TEST: Dilating %d\n", pid);
  return ret;
}

static int sequential_io(enum IO io) {
  /**
   * Because of the break in the for loop,
   * pids should be added to the next available
   */
  int i;
  struct timespec ts;
  s64 freeze_duration;

  switch (io) {
  case DILATE:
    for (i = 0; i < MAX_NUM_PIDS; ++i) {
      if (all_pids[i])
        dilate_proc(all_pids[i]);
      else
        break;
    }
    break;
  case FREEZE:
    for (i = 0; i < MAX_NUM_PIDS; ++i) {
      if (all_pids[i]) {
          struct task_struct *tsk = find_task_by_vpid(all_pids[i]);
          kill_pgrp(task_pgrp(tsk), SIGSTOP, 1);
      }
    }
    __getnstimeofday(&ts);
    freeze_now = timespec_to_ns(&ts);
    break;
  case RESUME:
    __getnstimeofday(&ts);
    freeze_duration = timespec_to_ns(&ts) - freeze_now;
    for (i = 0; i < MAX_NUM_PIDS; ++i) {
      if (all_pids[i]) {
        struct task_struct *tsk = find_task_by_vpid(all_pids[i]);
        tsk->freeze_past_nsec += freeze_duration;
      }
    }
    for (i = 0; i < MAX_NUM_PIDS; ++i) {
      if (all_pids[i]) {
        struct task_struct *tsk = find_task_by_vpid(all_pids[i]);
        kill_pgrp(task_pgrp(tsk), SIGCONT, 1);
      }
    }
    break;
  }
  return 0;
}

static int sequential_io_round_robin(enum IO io) {
  /**
   * Because of the break in the for loop,
   * pids should be added to the next available
   */
  int i, c = 0;
  struct timespec ts;
  s64 freeze_duration;

  switch (io) {
  case DILATE:
    for (i = 0; i < MAX_NUM_PIDS; i++) {
      if (all_pids[i])
        dilate_proc(all_pids[i]);
      else
        break;
    }
    break;
  case FREEZE:
    for (i = round_robin; c < MAX_NUM_PIDS; i = (i + 1) % MAX_NUM_PIDS, ++c) {
      if (all_pids[i]) {
        struct task_struct *tsk = find_task_by_vpid(all_pids[i]);
        kill_pgrp(task_pgrp(tsk), SIGSTOP, 1);
      }
    }
    __getnstimeofday(&ts);
    freeze_now = timespec_to_ns(&ts);
    break;
  case RESUME:
    __getnstimeofday(&ts);
    freeze_duration = timespec_to_ns(&ts) - freeze_now;
    for (i = round_robin; c < MAX_NUM_PIDS; i = (i + 1) % MAX_NUM_PIDS, ++c) {
      if (all_pids[i]) {
        struct task_struct *tsk = find_task_by_vpid(all_pids[i]);
        tsk->freeze_past_nsec += freeze_duration;
      }
    }
    for (i = round_robin; c < MAX_NUM_PIDS; i = (i + 1) % MAX_NUM_PIDS, ++c) {
      if (all_pids[i]) {
        struct task_struct *tsk = find_task_by_vpid(all_pids[i]);
        kill_pgrp(task_pgrp(tsk), SIGCONT, 1);
      }
    }
    round_robin = (round_robin + 1) % MAX_NUM_PIDS;
    break;
  }
  return 0;
}

/* SYSFS stuff */

/** @brief A callback function to display the vt tdf */
static ssize_t tdf_show(struct kobject *kobj, struct kobj_attribute *attr,
                        char *buf) {
  return sprintf(buf, "%d\n", tdf);
}

/** @brief A callback function to store the vt tdf */
static ssize_t tdf_store(struct kobject *kobj, struct kobj_attribute *attr,
                         const char *buf, size_t count) {
  /**
   * buf is the text input from sysfs.
   * we should convert this to an integer.
   */
  int ret;

  ret = kstrtoint(buf, 10, &tdf);
  if (ret < 0)
    return ret;
  /* we should overwrite any existing tdf */
  sequential_io(DILATE);
  return count;
}

static SHOW_HANDLER(01)
static SHOW_HANDLER(02)
static SHOW_HANDLER(03)
static SHOW_HANDLER(04)
static SHOW_HANDLER(05)
static SHOW_HANDLER(06)
static SHOW_HANDLER(07)
static SHOW_HANDLER(08)
static SHOW_HANDLER(09)
static SHOW_HANDLER(10)
static SHOW_HANDLER(11)
static SHOW_HANDLER(12)
static SHOW_HANDLER(13)
static SHOW_HANDLER(14)
static SHOW_HANDLER(15)
static SHOW_HANDLER(16)

static STORE_HANDLER(01)
static STORE_HANDLER(02)
static STORE_HANDLER(03)
static STORE_HANDLER(04)
static STORE_HANDLER(05)
static STORE_HANDLER(06)
static STORE_HANDLER(07)
static STORE_HANDLER(08)
static STORE_HANDLER(09)
static STORE_HANDLER(10)
static STORE_HANDLER(11)
static STORE_HANDLER(12)
static STORE_HANDLER(13)
static STORE_HANDLER(14)
static STORE_HANDLER(15)
static STORE_HANDLER(16)

/** @brief A callback function to display the vt mode */
static ssize_t mode_show(struct kobject *kobj, struct kobj_attribute *attr,
                         char *buf) {
  switch (mode) {
  case ENABLED:
    return sprintf(buf, "freeze\n");
  case DISABLED:
    return sprintf(buf, "unfreeze\n");
  default:
    return sprintf(buf, "LKM ERROR\n"); // whoops
  }
}

/** @brief A callback function to store the vt mode using enum*/
static ssize_t mode_store(struct kobject *kobj, struct kobj_attribute *attr,
                          const char *buf, size_t count) {
  if (strncmp(buf, "freeze", count - 1) == 0) {
    mode = ENABLED;
#ifndef QUIET
    VT_PRINTK("VT-GPIO_TEST: pause\n");
#endif
    /**
     * vt has been triggered locally
     * we need to quickly change to output mode
     */
    gpio_direction_output(gpioSIG, 1);

    /*
     * interrupt does not get called automatically
     * so we need to pause manually for the caller
     */
    // pause(); // Brian: let the pause be called by irq
    // printk(KERN_INFO "VT-GPIO_TEST: value of pin: %d\n",
    // gpio_get_value(gpioSIG));
  } else if (strncmp(buf, "unfreeze", count - 1) == 0) {
    mode = DISABLED;
#ifndef QUIET
    VT_PRINTK("VT-GPIO_TEST: resume\n");
#endif
    /**
     * chgn cfg, go low
     */
    gpio_direction_output(gpioSIG, 0);
    // printk(KERN_INFO "VT-GPIO_TEST: value of pin: %d\n",
    // gpio_get_value(gpioSIG));
    gpio_direction_input(gpioSIG);
    // printk(KERN_INFO "VT-GPIO_TEST: value of pin: %d\n",
    // gpio_get_value(gpioSIG)); resume(); // this should get called throuigh
    // the interrupt
  }
  return count;
}

/* Attributes */
static struct kobj_attribute mode_attr =
    __ATTR(mode, 0660, mode_show, mode_store);
static struct kobj_attribute tdf_attr = __ATTR(tdf, 0660, tdf_show, tdf_store);

DECLARE_PID_ATTR(01);
DECLARE_PID_ATTR(02);
DECLARE_PID_ATTR(03);
DECLARE_PID_ATTR(04);
DECLARE_PID_ATTR(05);
DECLARE_PID_ATTR(06);
DECLARE_PID_ATTR(07);
DECLARE_PID_ATTR(08);
DECLARE_PID_ATTR(09);
DECLARE_PID_ATTR(10);
DECLARE_PID_ATTR(11);
DECLARE_PID_ATTR(12);
DECLARE_PID_ATTR(13);
DECLARE_PID_ATTR(14);
DECLARE_PID_ATTR(15);
DECLARE_PID_ATTR(16);

/* Attribute struct */
static struct attribute *vt_attrs[] = {
    &mode_attr.attr, &tdf_attr.attr,
    &pid_01_attr.attr, &pid_02_attr.attr, &pid_03_attr.attr,
    &pid_04_attr.attr, &pid_05_attr.attr, &pid_06_attr.attr,
    &pid_07_attr.attr, &pid_08_attr.attr, &pid_09_attr.attr,
    &pid_10_attr.attr, &pid_11_attr.attr, &pid_12_attr.attr,
    &pid_13_attr.attr, &pid_14_attr.attr, &pid_15_attr.attr,
    &pid_16_attr.attr,
    NULL,
};

/* Attribute group */
static struct attribute_group attr_group = {
    .name = vtName,
    .attrs = vt_attrs,
};

static struct kobject *vt_kobj;

/** @brief handler for rising signal */
static irq_handler_t vtgpio_irq_handler(unsigned int irq, void *dev_id,
                                        struct pt_regs *regs) {
  trace_printk(KERN_INFO "rise\n");
  pause();
  return (irq_handler_t)IRQ_HANDLED; // return that we all good
}

/** @brief handler for falling signal */
static irq_handler_t vtgpio_irq_handler_fall(unsigned int irq, void *dev_id,
                                             struct pt_regs *regs) {
  trace_printk(KERN_INFO "fall\n");
  resume();
  return (irq_handler_t)IRQ_HANDLED; // return that we all good
}

/** @brief Function to initize kernel module */
static int __init vtgpio_init(void) {
  int result = 0;
  int res = 0;

  printk(KERN_INFO
         "VT-GPIO_TEST: Initializing the Virtual Time GPIO_TEST LKM\n");
  if (!gpio_is_valid(gpioSIG)) {
    printk(KERN_INFO "VT-GPIO_TEST: pin %d not valid\n", gpioSIG);
    return -ENODEV;
  }
  if (!gpio_is_valid(gpioSIG2)) {
    printk(KERN_INFO "VT-GPIO_TEST: pin %d not valid\n", gpioSIG2);
    return -ENODEV;
  }
  if (!gpio_is_valid(gpioSIG3)) {
    printk(KERN_INFO "VT-GPIO_TEST: pin %d not valid\n", gpioSIG3);
    return -ENODEV;
  }
  sprintf(vtName, "VT%d", gpioSIG);
  vt_kobj = kobject_create_and_add("vt", kernel_kobj->parent);

  if (!vt_kobj) {
    printk(KERN_ALERT "VT-GPIO_TEST: failed to create kobject\n");
    return -ENOMEM;
  }

  res = sysfs_create_group(vt_kobj, &attr_group);
  if (res) {
    printk(KERN_ALERT "VT-GPIO_TEST: failed to create sysfs group\n");
    kobject_put(vt_kobj);
    return res;
  }

  gpio_request(gpioSIG, "sysfs");
  gpio_request(gpioSIG2, "sysfs");
  gpio_request(gpioSIG3, "sysfs");
  gpio_direction_input(gpioSIG);  // default to input to listen
  gpio_direction_input(gpioSIG2); // default to input to listen
  gpio_direction_input(gpioSIG3); // default to input to listen
  gpio_set_debounce(gpioSIG, DEBOUNCE_TIME);
  gpio_set_debounce(gpioSIG2, DEBOUNCE_TIME);
  gpio_set_debounce(gpioSIG3, DEBOUNCE_TIME);
  gpio_export(gpioSIG, true);  // true = we should be able to change direction
  gpio_export(gpioSIG2, true); // true = we should be able to change direction
  gpio_export(gpioSIG3, true); // true = we should be able to change direction

  irqNumber = gpio_to_irq(gpioSIG3);
  printk(KERN_INFO "VT-GPIO_TEST: Input signal is mapped to IRQ: %d\n",
         irqNumber);
  irqNumber2 = gpio_to_irq(gpioSIG2);
  printk(KERN_INFO "VT-GPIO_TEST: Input signal is mapped to IRQ: %d\n",
         irqNumber2);

  result = request_irq(irqNumber, (irq_handler_t)vtgpio_irq_handler,
                       IRQF_TRIGGER_RISING, "vt_gpio_handler", NULL);
  printk(KERN_INFO "VT-GPIO_TEST: The interrupt rising request result is %d\n",
         result);

  result = request_irq(irqNumber2, (irq_handler_t)vtgpio_irq_handler_fall,
                       IRQF_TRIGGER_FALLING, "vt_gpio_handler_fall", NULL);
  printk(KERN_INFO "VT-GPIO_TEST: The interrupt rising request result is %d\n",
         result);

  return result;
}

/** @brief exit function to clean up */
static void __exit vtgpio_exit(void) {
  printk(KERN_INFO "VT-GPIO_TEST: Exiting LKM\n");
  kobject_put(vt_kobj);
  gpio_unexport(gpioSIG);
  gpio_unexport(gpioSIG2);
  gpio_unexport(gpioSIG3);

  free_irq(irqNumber, NULL);
  gpio_free(gpioSIG);
  gpio_free(gpioSIG3);
  free_irq(irqNumber2, NULL);
  gpio_free(gpioSIG2);
  printk(KERN_INFO "VT-GPIO_TEST: Successfully leaving LKM\n");
}

int write_proc_field(pid_t pid, char *field, char *val) {
  int ret;
  struct file *proc_file;
  char path[PATH_MAX];
  size_t val_len = strlen(val);

  sprintf(path, "/proc/%d/%s", pid, field);
  proc_file = file_open(path, O_WRONLY, 0);
  if (!proc_file) {
    printk(KERN_INFO "VT-GPIO_ERROR: can not open %s\n", path);
    return -1;
  }

  ret = file_write(proc_file, 0, val, val_len);

  if (ret < 0) {
    printk(KERN_INFO "VT-GPIO_ERROR: can not write %s\n", path);
    return -1;
  }
  ret = file_sync(proc_file);
  if (ret < 0) {
    printk(KERN_INFO "VT-GPIO_ERROR: can not sync %s\n", path);
    return -1;
  }
  file_close(proc_file);

  return 0;
}

struct file *file_open(const char *path, int flags, int rights) {
  struct file *filp = NULL;
  mm_segment_t oldfs;
  int err = 0;

  oldfs = get_fs();
  set_fs(get_ds());
  filp = filp_open(path, flags, rights);
  set_fs(oldfs);
  if (IS_ERR(filp)) {
    err = PTR_ERR(filp);
    return NULL;
  }
  return filp;
}

void file_close(struct file *file) { filp_close(file, NULL); }

int file_read(struct file *file, unsigned long long offset, unsigned char *data,
              unsigned int size) {
  mm_segment_t oldfs;
  int ret;

  oldfs = get_fs();
  set_fs(get_ds());

  ret = vfs_read(file, data, size, &offset);

  set_fs(oldfs);
  return ret;
}

int file_write(struct file *file, unsigned long long offset,
               unsigned char *data, unsigned int size) {
  mm_segment_t oldfs;
  int ret;

  oldfs = get_fs();
  set_fs(get_ds());

  ret = vfs_write(file, data, size, &offset);

  set_fs(oldfs);
  return ret;
}

int file_sync(struct file *file) {
  vfs_fsync(file, 0);
  return 0;
}

module_init(vtgpio_init);
module_exit(vtgpio_exit);
