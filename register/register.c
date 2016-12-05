#include <linux/kernel.h> /* decls needed for kernel modules */
#include <linux/module.h> /* decls needed for kernel modules */
#include <linux/version.h>        /* LINUX_VERSION_CODE, KERNEL_VERSION() */

#include "rtai.h"               /* RTAI configuration switches */
#include "rtai_sched.h"         /* rt_set_periodic_mode(), start_rt_timer(),
                                   nano2count(), RT_LOWEST_PRIORITY,
                                   rt_task_init(), rt_task_make_periodic() */

#if ! defined(RT_LOWEST_PRIORITY)
#if defined(RT_SCHED_LOWEST_PRIORITY)
#define RT_LOWEST_PRIORITY RT_SCHED_LOWEST_PRIORITY
#else
#error RT_SCHED_LOWEST_PRIORITY not defined
#endif
#endif

#include <linux/errno.h>  /* EINVAL, ENOMEM */

MODULE_LICENSE("GPL");

#define TIMER_TO_CPU 3          // < 0  || > 1 to maintain a symmetric processed timer.
#define RUNNABLE_ON_CPUS 3      // 1: on cpu 0 only, 2: on cpu 1 only, 3: on any;
#define RUN_ON_CPUS (num_online_cpus() > 1 ? RUNNABLE_ON_CPUS : 1)

static RTIME period_ns = 100000000;  // 100ms /* timer period, in nanoseconds */
static RTIME period_count;     /* requested timer period, in counts */
static RTIME timer_period_count;     /* actual timer period, in counts */

int start_timer = 1;
static unsigned int irq = 48;

int init_module(void)
{   
/*    rt_set_oneshot_mode();
    rt_assign_irq_to_cpu(RTAI_APIC_TIMER_IPI, TIMER_TO_CPU);
    period_count = nano2count(period_ns);
    timer_period_count = start_rt_timer(period_count);
*/
    rt_switch_domain(irq);

    return 0;
}

void cleanup_module(void)
{
    rt_switch_domain_back(irq);
/*    if (start_timer) {
        rt_reset_irq_to_sym_mode(RTAI_APIC_TIMER_IPI);
        stop_rt_timer();
    }
*/
    return;
}
