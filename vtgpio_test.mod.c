#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xa8d562a, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0xfe990052, __VMLINUX_SYMBOL_STR(gpio_free) },
	{ 0xf20dabd8, __VMLINUX_SYMBOL_STR(free_irq) },
	{ 0xed312e64, __VMLINUX_SYMBOL_STR(gpiod_unexport) },
	{ 0xf3e4b6b3, __VMLINUX_SYMBOL_STR(kernel_kobj) },
	{ 0xd6b8e852, __VMLINUX_SYMBOL_STR(request_threaded_irq) },
	{ 0x9073e5cd, __VMLINUX_SYMBOL_STR(gpiod_to_irq) },
	{ 0xedb0ada4, __VMLINUX_SYMBOL_STR(gpiod_export) },
	{ 0x5083d52b, __VMLINUX_SYMBOL_STR(gpiod_set_debounce) },
	{ 0x47229b5c, __VMLINUX_SYMBOL_STR(gpio_request) },
	{ 0x615ee1ed, __VMLINUX_SYMBOL_STR(kobject_put) },
	{ 0x8f7a5832, __VMLINUX_SYMBOL_STR(sysfs_create_group) },
	{ 0x329a3aca, __VMLINUX_SYMBOL_STR(kobject_create_and_add) },
	{ 0x91715312, __VMLINUX_SYMBOL_STR(sprintf) },
	{ 0x24f0071c, __VMLINUX_SYMBOL_STR(gpiod_direction_input) },
	{ 0xc1f68d9f, __VMLINUX_SYMBOL_STR(gpiod_direction_output_raw) },
	{ 0xb1e00ad4, __VMLINUX_SYMBOL_STR(gpio_to_desc) },
	{ 0x84b183ae, __VMLINUX_SYMBOL_STR(strncmp) },
	{ 0x2e5810c6, __VMLINUX_SYMBOL_STR(__aeabi_unwind_cpp_pr1) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xb1ad28e0, __VMLINUX_SYMBOL_STR(__gnu_mcount_nc) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "999F43B1CF93D1CB9B24264");
