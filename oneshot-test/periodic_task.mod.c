#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
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
	{ 0x9385048a, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x15692c87, __VMLINUX_SYMBOL_STR(param_ops_int) },
	{ 0xf46a8afb, __VMLINUX_SYMBOL_STR(stop_rt_timer) },
	{ 0xe57a5e63, __VMLINUX_SYMBOL_STR(rt_reset_irq_to_sym_mode) },
	{ 0xddfc5116, __VMLINUX_SYMBOL_STR(rt_task_delete) },
	{ 0xe00b12ca, __VMLINUX_SYMBOL_STR(rt_set_periodic_mode) },
	{ 0x8fb588d6, __VMLINUX_SYMBOL_STR(rt_task_make_periodic) },
	{ 0x33e581ab, __VMLINUX_SYMBOL_STR(start_rt_timer) },
	{ 0xe2e67707, __VMLINUX_SYMBOL_STR(nano2count) },
	{ 0xa05a4b6b, __VMLINUX_SYMBOL_STR(rt_assign_irq_to_cpu) },
	{ 0xeb73855b, __VMLINUX_SYMBOL_STR(rt_set_oneshot_mode) },
	{ 0x8ba7e585, __VMLINUX_SYMBOL_STR(rt_set_runnable_on_cpus) },
	{ 0x9f46ced8, __VMLINUX_SYMBOL_STR(__sw_hweight64) },
	{ 0x2d37342e, __VMLINUX_SYMBOL_STR(cpu_online_mask) },
	{ 0x912ce8f2, __VMLINUX_SYMBOL_STR(rt_task_init) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0x22040e90, __VMLINUX_SYMBOL_STR(count2nano) },
	{ 0x287ce940, __VMLINUX_SYMBOL_STR(rt_get_time) },
	{ 0xa6e8109e, __VMLINUX_SYMBOL_STR(rt_task_wait_period) },
	{ 0x9713066a, __VMLINUX_SYMBOL_STR(rt_get_cpu_time_ns) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=rtai_sched,rtai_hal";


MODULE_INFO(srcversion, "388199E4D35E534E787AED4");
