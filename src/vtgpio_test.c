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
//#include <linux/time.h>

#define DEBOUNCE_TIME 0.02

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Christopher Hannon");
MODULE_DESCRIPTION("Test for sync between two emb-lins for virtual time coordination");
MODULE_VERSION("0.1");

static unsigned int gpioSIG = 7;// try 25 to see if pin broken 7; // pin for talking // gpio21 on model b+ 
static unsigned int gpioSIG2 = 8;
// gpio 21 on 2B static 
/*

 *I think 7 and 8 for banana pi 21 and 20 for raspberry pi
 * To install use insmod gpio_test.ko and lsmod to see the module
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
void pause();
void resume();

enum modes { DISABLED, ENABLED };
static enum modes mode = DISABLED;

static char vtName[6] = "vtXXX";

//static int pids[128];

void pause() {
  struct timespec seconds;
  //printk(KERN_INFO "VT-GPIO_TEST: Interrupt! (Im alive!)");
  getnstimeofday(&seconds);
  printk(KERN_INFO "VT-GPIO_TEST: TIME-RISE: %llu %llu nanoseconds",(unsigned long long)seconds.tv_sec , (unsigned long long)seconds.tv_nsec);
  num_ints ++;
  printk(KERN_INFO "VT-GPIO_TEST: Rising Edge detected");
}

void resume() {
  struct timespec seconds;
  //printk(KERN_INFO "VT-GPIO_TEST: Interrupt! (Im alive!)");
  getnstimeofday(&seconds);
  printk(KERN_INFO "VT-GPIO_TEST: TIME-FALL: %llu %llu nanoseconds",(unsigned long long)seconds.tv_sec , (unsigned long long)seconds.tv_nsec);
  num_ints ++;
  printk(KERN_INFO "VT-GPIO_TEST: Falling Edge detected");
  

  /* we have to sound the trumpets */
  /* read list of pids */
  /* kickoff kthreads to resume processes */

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
    printk(KERN_INFO "VT-GPIO_TEST: pause\n");

    /* vt has been triggered locally */
    /*   we need to quickly  
	 ---------------------------
	 change to output mode,
	 go high on gpio
    */
    
    gpio_direction_output(gpioSIG,1);
    pause();
    //printk(KERN_INFO "VT-GPIO_TEST: value of pin: %d\n", gpio_get_value(gpioSIG));

    //interrupt does not get called :(
    // 
    
  }
  else if (strncmp(buf,"unfreeze",count-1)==0) {
    mode = DISABLED;
    printk(KERN_INFO "VT-GPIO_TEST: resume\n");

    /* chgn cfg
       go low
    */
    gpio_direction_output(gpioSIG,0); 
    //printk(KERN_INFO "VT-GPIO_TEST: value of pin: %d\n", gpio_get_value(gpioSIG));
    gpio_direction_input(gpioSIG); 
    //printk(KERN_INFO "VT-GPIO_TEST: value of pin: %d\n", gpio_get_value(gpioSIG));
  }
  return count;
}

static struct kobj_attribute mode_attr = __ATTR(mode, 0660, mode_show, mode_store);

static struct attribute *vt_attrs[] = {
  &mode_attr.attr,
  NULL,
};

static struct attribute_group attr_group = {
  .name = vtName,
  .attrs = vt_attrs,
};

static struct kobject *vt_kobj;
//static struct task_struct * task

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
  gpio_direction_input(gpioSIG); // default to input to listen
  gpio_direction_input(gpioSIG2); // default to input to listen
  gpio_set_debounce(gpioSIG, DEBOUNCE_TIME);
  gpio_set_debounce(gpioSIG2, DEBOUNCE_TIME);
  gpio_export(gpioSIG, true);    // true = we should be able to change direction
  gpio_export(gpioSIG2, true);    // true = we should be able to change direction

  irqNumber = gpio_to_irq(gpioSIG);
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

static void __exit vtgpio_exit(void) {
  printk(KERN_INFO "VT-GPIO_TEST: Exiting LKM\n");
  kobject_put(vt_kobj);
  gpio_unexport(gpioSIG);
  gpio_unexport(gpioSIG2);
  
  free_irq(irqNumber, NULL);
  gpio_free(gpioSIG);
  free_irq(irqNumber2, NULL);
  gpio_free(gpioSIG2);
  printk(KERN_INFO "VT-GPIO_TEST: Successfully leaving LKM\n");
}

static irq_handler_t vtgpio_irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs) {
  
  pause();
  return (irq_handler_t) IRQ_HANDLED; // return that we all good
}

static irq_handler_t vtgpio_irq_handler_fall(unsigned int irq, void *dev_id, struct pt_regs *regs) {
  resume();
  return (irq_handler_t) IRQ_HANDLED; // return that we all good
}


module_init(vtgpio_init);
module_exit(vtgpio_exit);

/* el fin */
