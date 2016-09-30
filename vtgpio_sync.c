/*
  File to talk to other r-pi

  Author: Christopher Hannon

 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>        // :)
#include <linux/gpio.h>          // gpio stuff
#include <linux/interrupt.h>     // irq  code

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Christopher Hannon");
MODULE_DESCRIPTION("Test for sync between two emb-lins for virtual time coordination");
MODULE_VERSION("0.1");

static unsigned int gpioSIG = 21; // pin for talking // gpio21 on model b+ // gpio 21 on 2B
static unsigned int active = 1;
static unsigned int last_triggered;
static unsigned int num_ints = 0;

static irq_handler_t vtgpio_irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs);

static int __init vtgpio_init(void) {
  printk(KERN_INFO "VT-GPIO_TEST: Initializing the Virtual Time GPIO_TEST LKM\n");
  if(!gpio_is_valid(gpioSIG)) {
    printk(KERN_INFO "VT-GPIO_TEST: pin not valid\n");
    return -ENODEV;
  }
  gpio_request(gpioSIG, "sysfs");   
  gpio_direction_input(gpioSig); // default to input to listen
  gpio_export(gpioSIG, true);    // true = we should be able to change direction

  irqNumber = gpio_to_irq(gpioSIG);
  printk(KERN_INFO "VT-GPIO_TEST: Input signal is mapped to IRQ: %d\n", irqNumber);

  result = request_irq(irqNumber, (irq_handler_t) vtgpio_irq_handler,
		       IRQF_TRIGGER_RISING, "vt_gpio_handler", NULL);

  printk(KERN_INFO "GPIO_TEST: The interrupt request result is %d\n", result);
  return result;
  
}

static void __exit vtgpio_exit(void) {
  printk(KERN_INFO "VT-GPIO_TEST: Exiting LKM\n");
  gpio_unexport(gpioSIG);
  free_irq(irqNumber, NULL);
  gpio_free(gpioSIG);
  printk(KERN_INFO "VT-GPIO_TEST: Successfully leaving LKM\n");
}

static irq_handler_t vtgpio_irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs) {
  printk(KERN_INFO "VT-GPIO_TEST: Interrupt! (Im alive!)");
  num_ints ++;

  // here we really want to call a system (kernel call) to use vt
  // if we stay in the kernel this should be fast as hell
  // how_to_do...?
  
  return (irq_handler_t) IRQ_HANDLED; // return that we all good
}


module_init(vtgpio_init);
module_exit(vtgpio_exit);

/* el fin */ 
