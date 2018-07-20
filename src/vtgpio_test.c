/*
    Virtual Time Kernel Module
    For Heterogeneous Distributed
	Embedded Linux Environment

    Author: Christopher Hannon
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>        // :)
#include <linux/gpio.h>          // gpio stuff
#include <linux/interrupt.h>     // irq  code
#include <linux/kobject.h>
#include <linux/kthread.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/sysfs.h>
#include <linux/fs.h>
#include <asm/segment.h>
#include <asm/uaccess.h>
#include <linux/buffer_head.h>
#include <linux/string.h>

#define TDF_STR_LEN 8
#define DEBOUNCE_TIME 0.02
#define MAX_NUM_PIDS 16

#define ftrace 1

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Christopher Hannon");
MODULE_DESCRIPTION("Sync between multiple embedded linux devices for virtual time coordination");
MODULE_VERSION("0.3");

static unsigned int gpioSIG = 7; // Using CE1 to output high or low
static unsigned int gpioSIG2 = 8;
static unsigned int gpioSIG3 = 24; // Brian: Using GPIO5 to listening for falling

/*
 * Brian: CE1 to output high or low, GPIO5 to listening for falling | 2018Mar8th
 * gpio 21 on 2B static
 *  try 25 to see if pin broken 7; // pin for talking // gpio21 on model b+
 * I think 7 and 8 for banana pi 21 and 20 for raspberry pi
 *  To install use insmod gpio_test.ko and lsmod to see the module
 * To uninstall use rmmod vtgpio_test and lsmod to confirm
 * if breaks just reboot
 * to pause write freeze/unfreeze to ....
 * /sys/vt/...
 */

unsigned int active = 1; //
//static unsigned int last_triggered;
static unsigned int num_ints = 0;
static unsigned int irqNumber;
static unsigned int irqNumber2;

static irq_handler_t vtgpio_irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs);
static irq_handler_t vtgpio_irq_handler_fall(unsigned int irq, void *dev_id, struct pt_regs *regs);

void pause(void);
void resume(void);
static int dilate_proc(int pid);
int write_proc_field(pid_t pid, char* field, char* val); // from Jiaqi's code

struct file* file_open(const char* path, int flags, int rights);
void file_close(struct file* file);
int file_read(struct file* file, unsigned long long offset, unsigned char* data, unsigned int size);
int file_write(struct file* file, unsigned long long offset, unsigned char* data, unsigned int size);
int file_sync(struct file* file);

enum modes { DISABLED, ENABLED };
static enum modes mode = DISABLED;
static int all_pids[MAX_NUM_PIDS] = {0};
enum IO{ RESUME,FREEZE,DILATE };

static int sequential_io(enum IO io);

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
static int tdf = 1000;

static char vtName[6] = "vtXXX";

void pause(void) {
  trace_printk(KERN_INFO "VT_TEST_PAUSE");
  struct timespec seconds;
  struct timespec seconds_end;

  getnstimeofday(&seconds);
  if(ftrace){
    trace_printk(KERN_INFO "VT-GPIO_TIME: TIME-RISE: %llu %llu nanoseconds",(unsigned long long)seconds.tv_sec , (unsigned long long)seconds.tv_nsec);
  }
  else {
    printk(KERN_INFO "VT-GPIO_TIME: TIME-RISE: %llu %llu nanoseconds",(unsigned long long)seconds.tv_sec , (unsigned long long)seconds.tv_nsec);
  }
  num_ints ++;
  if(ftrace) {
    trace_printk(KERN_INFO "VT-GPIO_TEST: Rising Edge detected");
  }
  else {
    printk(KERN_INFO "VT-GPIO_TEST: Rising Edge detected");
  }
  
  /* read list of pids */
  /* kickoff kthreads to resume processes */

  sequential_io(FREEZE);

  getnstimeofday(&seconds_end);
  if(ftrace) {
    trace_printk(KERN_INFO "VT-GPIO_TIME: TIME-RISE: %llu %llu nanoseconds",(unsigned long long)seconds_end.tv_sec , (unsigned long long)seconds_end.tv_nsec);
    trace_printk(KERN_INFO "VT-GPIO_TIME: TIME-PAUSE: %llu %llu nanoseconds",
	 ((unsigned long long)seconds_end.tv_sec-(unsigned long long)seconds.tv_sec) ,
	 ((unsigned long long)seconds_end.tv_nsec -(unsigned long long)seconds.tv_nsec));
  }
  else {
    printk(KERN_INFO "VT-GPIO_TIME: TIME-RISE: %llu %llu nanoseconds",(unsigned long long)seconds_end.tv_sec , (unsigned long long)seconds_end.tv_nsec);
    printk(KERN_INFO "VT-GPIO_TIME: TIME-PAUSE: %llu %llu nanoseconds",
	 ((unsigned long long)seconds_end.tv_sec-(unsigned long long)seconds.tv_sec) ,
	 ((unsigned long long)seconds_end.tv_nsec -(unsigned long long)seconds.tv_nsec));
  }
}

/** @brief Function to pause pids in VT */
void resume(void) {
  trace_printk(KERN_INFO "VT_TEST_RESUME");
  struct timespec seconds;
  struct timespec seconds_end;

  getnstimeofday(&seconds);
  if(ftrace){
    trace_printk(KERN_INFO "VT-GPIO_TIME: TIME-FALL: %llu %llu nanoseconds",(unsigned long long)seconds.tv_sec , (unsigned long long)seconds.tv_nsec);
  }
  else{
    printk(KERN_INFO "VT-GPIO_TIME: TIME-FALL: %llu %llu nanoseconds",(unsigned long long)seconds.tv_sec , (unsigned long long)seconds.tv_nsec);
  }
  num_ints ++;
  if(ftrace) {
    trace_printk(KERN_INFO "VT-GPIO_TEST: Falling Edge detected");
  }
  else{
    printk(KERN_INFO "VT-GPIO_TEST: Falling Edge detected");
  }
  /* we have to sound the trumpets */
  /* read list of pids */
  /* kickoff kthreads to resume processes */
  sequential_io(RESUME);

  getnstimeofday(&seconds_end);
  if(ftrace) {
    trace_printk(KERN_INFO "VT-GPIO_TIME: TIME-FALL: %llu %llu nanoseconds",(unsigned long long)seconds_end.tv_sec , (unsigned long long)seconds_end.tv_nsec);
    trace_printk(KERN_INFO "VT-GPIO_TIME: TIME-RESUME: %llu %llu nanoseconds",
	 ((unsigned long long)seconds_end.tv_sec-(unsigned long long)seconds.tv_sec) ,
	 ((unsigned long long)seconds_end.tv_nsec -(unsigned long long)seconds.tv_nsec));
  }
  else {
    printk(KERN_INFO "VT-GPIO_TIME: TIME-FALL: %llu %llu nanoseconds",(unsigned long long)seconds_end.tv_sec , (unsigned long long)seconds_end.tv_nsec);
    printk(KERN_INFO "VT-GPIO_TIME: TIME-RESUME: %llu %llu nanoseconds",
	 ((unsigned long long)seconds_end.tv_sec-(unsigned long long)seconds.tv_sec) ,
	 ((unsigned long long)seconds_end.tv_nsec -(unsigned long long)seconds.tv_nsec));
  }
}

/** @brief Function to add pids to VT */
static int dilate_proc(int pid) {
  int ret = 0;
  char tdf_str[TDF_STR_LEN];

  ret = sprintf(tdf_str, "%d",tdf);
  write_proc_field((pid_t)pid, "dilation", tdf_str);
  if(ftrace){
    trace_printk(KERN_INFO "VT-GPIO_TEST: Dilating %d\n",pid);
  }
  else {
    printk(KERN_INFO "VT-GPIO_TEST: Dilating %d\n",pid);
  }
  return ret;
}

/** @brief Function to add pids to VT */
static int freeze_proc(int pid) {
  int ret = 0;

  if(ftrace){
    trace_printk(KERN_INFO "VT-GPIO_TEST: Freezing %d\n",pid);
  }
  else{
    printk(KERN_INFO "VT-GPIO_TEST: Freezing %d\n",pid);
  }
  write_proc_field((pid_t)pid, "freeze", "1");
  return ret;
}
/** @brief Function to add pids to VT */
static int resume_proc(int pid) {
  int ret = 0;

  if(ftrace){
    trace_printk(KERN_INFO "VT-GPIO_TEST: Unfreezing %d\n",pid);
  }
  else{
    printk(KERN_INFO "VT-GPIO_TEST: Unfreezing %d\n",pid);
  }
  write_proc_field((pid_t)pid, "freeze", "0");
  return ret;
}

static int sequential_io(enum IO io) {
  /* Because of the break in the for loop,
   *  pids should be added to the next available
   */

  int i;
  switch(io){
  case DILATE:
    for(i=0;i<MAX_NUM_PIDS;i++){
      if(all_pids[i])
	dilate_proc(all_pids[i]);
      else
	break;
    }
    break;
  case FREEZE:
    for(i=0;i<MAX_NUM_PIDS;i++){
      if(all_pids[i])
	freeze_proc(all_pids[i]);
      else
	break;
    }
    break;
  case RESUME:
    for(i=0;i<MAX_NUM_PIDS;i++){
      if(all_pids[i])
	resume_proc(all_pids[i]);
      else
	break;
    }
    break;
  }
  return 0;
}

/* SYSFS stuff */


/** @brief A callback function to display the vt tdf */
static ssize_t tdf_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
  return sprintf(buf, "%d\n", tdf);
  return 0;
}

/** @brief A callback function to store the vt tdf */
static ssize_t tdf_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
  /*
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

/** @brief A callback function to display the vt pids */
static ssize_t pid_01_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
  return sprintf(buf, "%d\n", pid_01);
  return 0;
}

/** @brief A callback function to store the vt pids */
static ssize_t pid_01_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
  /*
   * buf is the text input from sysfs.
   * we should convert this to an integer.
   */
  int ret;

  ret = kstrtoint(buf, 10, &pid_01);
  if (ret < 0)
    return ret;
  if(pid_01){
    all_pids[0] = pid_01;
    ret = dilate_proc(pid_01);
  }
  if (ret < 0)
    return ret;
  return count;
}

/** @brief A callback function to display the vt pids */
static ssize_t pid_02_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
  return sprintf(buf, "%d\n", pid_02);
  return 0;
}

/** @brief A callback function to store the vt pids */
static ssize_t pid_02_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
  /*
   * buf is the text input from sysfs.
   * we should convert this to an integer.
   */
  int ret;

  ret = kstrtoint(buf, 10, &pid_02);
  if (ret < 0)
    return ret;
  if(pid_02){
    all_pids[1] = pid_02;
    ret = dilate_proc(pid_02);
  }
  if (ret < 0)
    return ret;
  return count;
}
/** @brief A callback function to display the vt pids */
static ssize_t pid_03_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
  return sprintf(buf, "%d\n", pid_03);
  return 0;
}

/** @brief A callback function to store the vt pids */
static ssize_t pid_03_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
  /*
   * buf is the text input from sysfs.
   * we should convert this to an integer.
   */
  int ret;

  ret = kstrtoint(buf, 10, &pid_03);
  if (ret < 0)
    return ret;
  if(pid_03){
    all_pids[2] = pid_03;
    ret = dilate_proc(pid_03);
  }
  if (ret < 0)
    return ret;
  return count;
}
/** @brief A callback function to display the vt pids */
static ssize_t pid_04_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
  return sprintf(buf, "%d\n", pid_04);
  return 0;
}

/** @brief A callback function to store the vt pids */
static ssize_t pid_04_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
  /*
   * buf is the text input from sysfs.
   * we should convert this to an integer.
   */
  int ret;

  ret = kstrtoint(buf, 10, &pid_04);
  if (ret < 0)
    return ret;
  if(pid_04){
    all_pids[3] = pid_04;
    ret = dilate_proc(pid_04);
  }
  if (ret < 0)
    return ret;
  return count;
}
/** @brief A callback function to display the vt pids */
static ssize_t pid_05_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
  return sprintf(buf, "%d\n", pid_05);
  return 0;
}

/** @brief A callback function to store the vt pids */
static ssize_t pid_05_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
  /*
   * buf is the text input from sysfs.
   * we should convert this to an integer.
   */
  int ret;

  ret = kstrtoint(buf, 10, &pid_05);
  if (ret < 0)
    return ret;
  if(pid_05){
    all_pids[4] = pid_05;
    ret = dilate_proc(pid_05);
  }
  if (ret < 0)
    return ret;
  return count;
}
/** @brief A callback function to display the vt pids */
static ssize_t pid_06_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
  return sprintf(buf, "%d\n", pid_06);
  return 0;
}

/** @brief A callback function to store the vt pids */
static ssize_t pid_06_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
  /*
   * buf is the text input from sysfs.
   * we should convert this to an integer.
   */
  int ret;

  ret = kstrtoint(buf, 10, &pid_06);
  if (ret < 0)
    return ret;
  if(pid_06){
    all_pids[5] = pid_06;
    ret = dilate_proc(pid_06);
  }
  if (ret < 0)
    return ret;
  return count;
}
/** @brief A callback function to display the vt pids */
static ssize_t pid_07_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
  return sprintf(buf, "%d\n", pid_07);
  return 0;
}

/** @brief A callback function to store the vt pids */
static ssize_t pid_07_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
  /*
   * buf is the text input from sysfs.
   * we should convert this to an integer.
   */
  int ret;

  ret = kstrtoint(buf, 10, &pid_07);
  if (ret < 0)
    return ret;
  if(pid_06){
    all_pids[6] = pid_07;
    ret = dilate_proc(pid_07);
  }
  if (ret < 0)
    return ret;
  return count;
}
/** @brief A callback function to display the vt pids */
static ssize_t pid_08_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
  return sprintf(buf, "%d\n", pid_08);
  return 0;
}

/** @brief A callback function to store the vt pids */
static ssize_t pid_08_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
  /*
   * buf is the text input from sysfs.
   * we should convert this to an integer.
   */
  int ret;

  ret = kstrtoint(buf, 10, &pid_08);
  if (ret < 0)
    return ret;
  if(pid_08){
    all_pids[7] = pid_08;
    ret = dilate_proc(pid_08);
  }
  if (ret < 0)
    return ret;
  return count;
}
/** @brief A callback function to display the vt pids */
static ssize_t pid_09_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
  return sprintf(buf, "%d\n", pid_09);
  return 0;
}

/** @brief A callback function to store the vt pids */
static ssize_t pid_09_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
  /*
   * buf is the text input from sysfs.
   * we should convert this to an integer.
   */
  int ret;

  ret = kstrtoint(buf, 10, &pid_09);
  if (ret < 0)
    return ret;
  if(pid_09){
    all_pids[8] = pid_09;
    ret = dilate_proc(pid_09);
  }
  if (ret < 0)
    return ret;
  return count;
}
/** @brief A callback function to display the vt pids */
static ssize_t pid_10_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
  return sprintf(buf, "%d\n", pid_10);
  return 0;
}

/** @brief A callback function to store the vt pids */
static ssize_t pid_10_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
  /*
   * buf is the text input from sysfs.
   * we should convert this to an integer.
   */
  int ret;

  ret = kstrtoint(buf, 10, &pid_10);
  if (ret < 0)
    return ret;
  if(pid_10){
    all_pids[9] = pid_10;
    ret = dilate_proc(pid_10);
  }
  if (ret < 0)
    return ret;
  return count;
}
/** @brief A callback function to display the vt pids */
static ssize_t pid_11_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
  return sprintf(buf, "%d\n", pid_11);
  return 0;
}

/** @brief A callback function to store the vt pids */
static ssize_t pid_11_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
  /*
   * buf is the text input from sysfs.
   * we should convert this to an integer.
   */
  int ret;

  ret = kstrtoint(buf, 10, &pid_11);
  if (ret < 0)
    return ret;
  if(pid_11){
    all_pids[10] = pid_11;
    ret = dilate_proc(pid_11);
  }
  if (ret < 0)
    return ret;
  return count;
}
/** @brief A callback function to display the vt pids */
static ssize_t pid_12_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
  return sprintf(buf, "%d\n", pid_12);
  return 0;
}

/** @brief A callback function to store the vt pids */
static ssize_t pid_12_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
  /*
   * buf is the text input from sysfs.
   * we should convert this to an integer.
   */
  int ret;

  ret = kstrtoint(buf, 10, &pid_12);
  if (ret < 0)
    return ret;
  if(pid_12){
    all_pids[11] = pid_12;
    ret = dilate_proc(pid_12);
  }
  if (ret < 0)
    return ret;
  return count;
}
/** @brief A callback function to display the vt pids */
static ssize_t pid_13_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
  return sprintf(buf, "%d\n", pid_13);
  return 0;
}

/** @brief A callback function to store the vt pids */
static ssize_t pid_13_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
  /*
   * buf is the text input from sysfs.
   * we should convert this to an integer.
   */
  int ret;

  ret = kstrtoint(buf, 10, &pid_13);
  if (ret < 0)
    return ret;
  if(pid_13){
    all_pids[12] = pid_13;
    ret = dilate_proc(pid_13);
  }
  if (ret < 0)
    return ret;
  return count;
}
/** @brief A callback function to display the vt pids */
static ssize_t pid_14_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
  return sprintf(buf, "%d\n", pid_14);
  return 0;
}

/** @brief A callback function to store the vt pids */
static ssize_t pid_14_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
  /*
   * buf is the text input from sysfs.
   * we should convert this to an integer.
   */
  int ret;

  ret = kstrtoint(buf, 10, &pid_14);
  if (ret < 0)
    return ret;
  if(pid_14){
    all_pids[13] = pid_14;
    ret = dilate_proc(pid_14);
  }
  if (ret < 0)
    return ret;
  return count;
}
/** @brief A callback function to display the vt pids */
static ssize_t pid_15_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
  return sprintf(buf, "%d\n", pid_15);
  return 0;
}

/** @brief A callback function to store the vt pids */
static ssize_t pid_15_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
  /*
   * buf is the text input from sysfs.
   * we should convert this to an integer.
   */
  int ret;

  ret = kstrtoint(buf, 10, &pid_15);
  if (ret < 0)
    return ret;
  if(pid_15){
    all_pids[14] = pid_15;
    ret = dilate_proc(pid_15);
  }
  if (ret < 0)
    return ret;
  return count;
}
/** @brief A callback function to display the vt pids */
static ssize_t pid_16_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
  return sprintf(buf, "%d\n", pid_16);
  return 0;
}

/** @brief A callback function to store the vt pids */
static ssize_t pid_16_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
  /*
   * buf is the text input from sysfs.
   * we should convert this to an integer.
   */
  int ret;

  ret = kstrtoint(buf, 10, &pid_16);
  if (ret < 0)
    return ret;
  if(pid_16){
    all_pids[15] = pid_16;
    ret = dilate_proc(pid_16);
  }
  if (ret < 0)
    return ret;
  return count;
}


/** @brief A callback function to display the vt mode */
static ssize_t mode_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
  switch(mode){
  case ENABLED: return sprintf(buf, "freeze\n");
  case DISABLED: return sprintf(buf, "unfreeze\n");
  default: return sprintf(buf, "LKM ERROR\n"); //whoops
  }
}

/** @brief A callback function to store the vt mode using enum*/
static ssize_t mode_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
  if(strncmp(buf,"freeze",count-1)==0) {
    mode = ENABLED;
    if(ftrace) {
      trace_printk(KERN_INFO "VT-GPIO_TEST: pause\n");
    }
    else{
      printk(KERN_INFO "VT-GPIO_TEST: pause\n");
    }
    /* vt has been triggered locally
     *   we need to quickly
     *	 change to output mode
     */

    gpio_direction_output(gpioSIG,1);

    /*
     * interrupt does not get called automatically
     * so we need to pause manually for the caller
     */

    // pause(); // Brian: let the pause be called by irq
    //printk(KERN_INFO "VT-GPIO_TEST: value of pin: %d\n", gpio_get_value(gpioSIG));
  }
  else if (strncmp(buf,"unfreeze",count-1)==0) {
    mode = DISABLED;
    if(ftrace){
      trace_printk(KERN_INFO "VT-GPIO_TEST: resume\n");
    }
    else{
      printk(KERN_INFO "VT-GPIO_TEST: resume\n");
    }
    /* chgn cfg
     * go low
     */
    gpio_direction_output(gpioSIG,0);
    //printk(KERN_INFO "VT-GPIO_TEST: value of pin: %d\n", gpio_get_value(gpioSIG));
    gpio_direction_input(gpioSIG);
    //printk(KERN_INFO "VT-GPIO_TEST: value of pin: %d\n", gpio_get_value(gpioSIG));
    //resume(); // this should get called throuigh the interrupt
  }
  return count;
}

/* Attributes */
static struct kobj_attribute mode_attr = __ATTR(mode, 0660, mode_show, mode_store);
static struct kobj_attribute tdf_attr = __ATTR(tdf, 0660, tdf_show, tdf_store);
static struct kobj_attribute pid_01_attr = __ATTR(pid_01, 0660, pid_01_show, pid_01_store);
static struct kobj_attribute pid_02_attr = __ATTR(pid_02, 0660, pid_02_show, pid_02_store);
static struct kobj_attribute pid_03_attr = __ATTR(pid_03, 0660, pid_03_show, pid_03_store);
static struct kobj_attribute pid_04_attr = __ATTR(pid_04, 0660, pid_04_show, pid_04_store);
static struct kobj_attribute pid_05_attr = __ATTR(pid_05, 0660, pid_05_show, pid_05_store);
static struct kobj_attribute pid_06_attr = __ATTR(pid_06, 0660, pid_06_show, pid_06_store);
static struct kobj_attribute pid_07_attr = __ATTR(pid_07, 0660, pid_07_show, pid_07_store);
static struct kobj_attribute pid_08_attr = __ATTR(pid_08, 0660, pid_08_show, pid_08_store);
static struct kobj_attribute pid_09_attr = __ATTR(pid_09, 0660, pid_09_show, pid_09_store);
static struct kobj_attribute pid_10_attr = __ATTR(pid_10, 0660, pid_10_show, pid_10_store);
static struct kobj_attribute pid_11_attr = __ATTR(pid_11, 0660, pid_11_show, pid_11_store);
static struct kobj_attribute pid_12_attr = __ATTR(pid_12, 0660, pid_12_show, pid_12_store);
static struct kobj_attribute pid_13_attr = __ATTR(pid_13, 0660, pid_13_show, pid_13_store);
static struct kobj_attribute pid_14_attr = __ATTR(pid_14, 0660, pid_14_show, pid_14_store);
static struct kobj_attribute pid_15_attr = __ATTR(pid_15, 0660, pid_15_show, pid_15_store);
static struct kobj_attribute pid_16_attr = __ATTR(pid_16, 0660, pid_16_show, pid_16_store);

/* Attribute struct */
static struct attribute *vt_attrs[] = {
  &mode_attr.attr,
  &tdf_attr.attr,
  &pid_01_attr.attr,
  &pid_02_attr.attr,
  &pid_03_attr.attr,
  &pid_04_attr.attr,
  &pid_05_attr.attr,
  &pid_06_attr.attr,
  &pid_07_attr.attr,
  &pid_08_attr.attr,
  &pid_09_attr.attr,
  &pid_10_attr.attr,
  &pid_11_attr.attr,
  &pid_12_attr.attr,
  &pid_13_attr.attr,
  &pid_14_attr.attr,
  &pid_15_attr.attr,
  &pid_16_attr.attr,
  NULL,
};

/* Attribute group */
static struct attribute_group attr_group = {
  .name = vtName,
  .attrs = vt_attrs,
};

static struct kobject *vt_kobj;
//static struct task_struct * task

/** @brief Function to initize kernel module */
static int __init vtgpio_init(void) {
  int result=0;
  int res = 0;

  printk(KERN_INFO "VT-GPIO_TEST: Initializing the Virtual Time GPIO_TEST LKM\n");
  if(!gpio_is_valid(gpioSIG)) {
    printk(KERN_INFO "VT-GPIO_TEST: pin %d not valid\n",gpioSIG);
    return -ENODEV;
  }
  if(!gpio_is_valid(gpioSIG2)) {
    printk(KERN_INFO "VT-GPIO_TEST: pin %d not valid\n",gpioSIG2);
    return -ENODEV;
  }
  if(!gpio_is_valid(gpioSIG3)) {
    printk(KERN_INFO "VT-GPIO_TEST: pin %d not valid\n",gpioSIG3);
    return -ENODEV;
  }
  sprintf(vtName, "VT%d", gpioSIG);
  vt_kobj = kobject_create_and_add("vt", kernel_kobj->parent);

  if(!vt_kobj){
    printk(KERN_ALERT "VT-GPIO_TEST: failed to create kobject\n");
    return -ENOMEM;
  }

  res = sysfs_create_group(vt_kobj, &attr_group);
  if(res){
    printk(KERN_ALERT "VT-GPIO_TEST: failed to create sysfs group\n");
    kobject_put(vt_kobj);
    return res;
  }

  gpio_request(gpioSIG, "sysfs");
  gpio_request(gpioSIG2, "sysfs");
  gpio_request(gpioSIG3, "sysfs");
  gpio_direction_input(gpioSIG); // default to input to listen
  gpio_direction_input(gpioSIG2); // default to input to listen
  gpio_direction_input(gpioSIG3); // default to input to listen
  gpio_set_debounce(gpioSIG, DEBOUNCE_TIME);
  gpio_set_debounce(gpioSIG2, DEBOUNCE_TIME);
  gpio_set_debounce(gpioSIG3, DEBOUNCE_TIME);
  gpio_export(gpioSIG, true);    // true = we should be able to change direction
  gpio_export(gpioSIG2, true);    // true = we should be able to change direction
  gpio_export(gpioSIG3, true);    // true = we should be able to change direction

  irqNumber = gpio_to_irq(gpioSIG3);
  printk(KERN_INFO "VT-GPIO_TEST: Input signal is mapped to IRQ: %d\n", irqNumber);
  irqNumber2 = gpio_to_irq(gpioSIG2);
  printk(KERN_INFO "VT-GPIO_TEST: Input signal is mapped to IRQ: %d\n", irqNumber2);

  result = request_irq(irqNumber, (irq_handler_t) vtgpio_irq_handler,
		       IRQF_TRIGGER_RISING, "vt_gpio_handler", NULL);
  printk(KERN_INFO "VT-GPIO_TEST: The interrupt rising request result is %d\n", result);

  result = request_irq(irqNumber2, (irq_handler_t) vtgpio_irq_handler_fall,
		       IRQF_TRIGGER_FALLING, "vt_gpio_handler_fall", NULL);
  printk(KERN_INFO "VT-GPIO_TEST: The interrupt rising request result is %d\n", result);

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

/** @brief handler for rising signal */
static irq_handler_t vtgpio_irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs) {
  pause();
  return (irq_handler_t) IRQ_HANDLED; // return that we all good
}

/** @brief handler for falling signal */
static irq_handler_t vtgpio_irq_handler_fall(unsigned int irq, void *dev_id, struct pt_regs *regs) {
  resume();
  return (irq_handler_t) IRQ_HANDLED; // return that we all good
}


int write_proc_field(pid_t pid, char* field, char* val) {
  int ret;
  struct file *proc_file;
  char path[PATH_MAX];
  size_t val_len = strlen(val);

  sprintf(path, "/proc/%d/%s", pid, field);
  proc_file =file_open(path,O_WRONLY,0);
  if (!proc_file){
    printk(KERN_INFO "VT-GPIO_ERROR: can not open %s\n",path);
    return -1;
  }

  ret = file_write(proc_file, 0, val, val_len);

  if (ret < 0) {
    printk(KERN_INFO "VT-GPIO_ERROR: can not write %s\n",path);
    return -1;
  }
  ret = file_sync(proc_file);
  if (ret < 0) {
    printk(KERN_INFO "VT-GPIO_ERROR: can not sync %s\n",path);
    return -1;
  }
  file_close(proc_file);

  return 0;
}

struct file* file_open(const char* path, int flags, int rights) {
  struct file* filp = NULL;
  mm_segment_t oldfs;
  int err = 0;

  oldfs = get_fs();
  set_fs(get_ds());
  filp = filp_open(path, flags, rights);
  set_fs(oldfs);
  if(IS_ERR(filp)) {
    err = PTR_ERR(filp);
    return NULL;
  }
  return filp;
}

void file_close(struct file* file) {
  filp_close(file, NULL);
}

int file_read(struct file* file, unsigned long long offset, unsigned char* data, unsigned int size) {
  mm_segment_t oldfs;
  int ret;

  oldfs = get_fs();
  set_fs(get_ds());

  ret = vfs_read(file, data, size, &offset);

  set_fs(oldfs);
  return ret;
}

int file_write(struct file* file, unsigned long long offset, unsigned char* data, unsigned int size) {
  mm_segment_t oldfs;
  int ret;

  oldfs = get_fs();
  set_fs(get_ds());

  ret = vfs_write(file, data, size, &offset);

  set_fs(oldfs);
  return ret;
}

int file_sync(struct file* file) {
  vfs_fsync(file, 0);
  return 0;
}

module_init(vtgpio_init);
module_exit(vtgpio_exit);

/* el fin */

/* # notes # */

/** @brief A callback function to display the vt pids */
/*static ssize_t pids_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
  //int i=0;
  //for(i;i<MAX_PIDS;i++) {

  // TODO IMPLEMENT               }
return 0;
}
*/
/** @brief A callback function to store the vt pids */
/*
static ssize_t pids_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {

   //c buf is the text input from sysfs.
   // we should convert this to an integer array.

  char buf2[128*6] = {0};
  static int cur_pid = 0;
  int num_count = 0;
  int i = 0;
  int result = 0;
  char *dst;
  char *ddst;

  strncpy(buf2,buf,count);

  printk(KERN_INFO "VT-GPIO_DEGUG: enter pidstore");

  dst = buf2;
  ddst = buf2;

  printk(KERN_INFO "VT-GPIO_DEGUG:  size of buffer %d ",count);

  printk(KERN_INFO "VT-GPIO_DEGUG: ddst %p ", ddst);
  printk(KERN_INFO "VT-GPIO_DEGUG: buf  %p ",buf);
  printk(KERN_INFO "VT-GPIO_DEGUG: end-ddst %p ",(ddst + count));
  printk(KERN_INFO "VT-GPIO_DEGUG: end-buf %p ",(buf + count));
  printk(KERN_INFO "VT-GPIO_DEGUG: buf %s ",(buf ));
  printk(KERN_INFO "VT-GPIO_DEGUG: ddst %s ",(ddst ));

  while(ddst < ddst) {
    ddst ++;
    if (ddst[0] == ' ') {
      result = kstrtoint(dst,(uint)10,&cur_pid);
      dst=ddst+1;

    printk(KERN_INFO "VT-GPIO_DEGUG: kstrtoint %d ",cur_pid);
    printk(KERN_INFO "VT-GPIO_DEGUG: kstrtoint res: %d ",result);
    pids[num_count] = cur_pid;
    num_count += 1;
    }
  }

  for(i=0;i<num_pids;i++) {
    printk(KERN_INFO "VT-GPIO_TEST: pid: %d \n", pids[i]);
  }

  num_pids = num_count;
  dilate_procs();
  printk(KERN_INFO "VT-GPIO_DEBUG: leaving pidstore\n");

  return count;
}

*/
