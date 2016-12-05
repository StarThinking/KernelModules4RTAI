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

int timer_mode = 0;
RTAI_MODULE_PARM(timer_mode, int);
MODULE_PARM_DESC(timer_mode, "timer running mode: 0-oneshot, 1-periodic");

static RT_TASK task;      /* we'll fill this in with our task */
static RTIME period_ns = 100000000;  // 100ms /* timer period, in nanoseconds */
static RTIME period_count;     /* requested timer period, in counts */
static RTIME timer_period_count;     /* actual timer period, in counts */
static RTIME expected;  

static int counter = 0;
int start_timer = 1;

void thread_func(long arg)
{
  int diff = 0;
  RTIME t, svt;
  svt = rt_get_cpu_time_ns();

  while (1) {
      expected += timer_period_count;
      if(!rt_task_wait_period()) {
        if(!timer_mode) {
              diff = (int) count2nano(rt_get_time() - expected);
        } else {
            diff = (int) ((t = rt_get_cpu_time_ns()) - svt - period_ns);
            svt = t;
        }
        printk("diff = %d ns\n", diff);
      } else {
          printk("rt_task_wait_period() != 0 \n");
      }
  }

  /* we'll never get here */
  return;
}

int init_module(void)
{
  int retval;                   /* we look at our return values */

  printk("timer_node is %s\n", timer_mode ? "periodic" : "oneshot");

  retval =
    rt_task_init(&task,       /* pointer to our task structure */
                 thread_func, /* the actual timer function */
                 0,             /* initial task parameter; we ignore */
                 1024,          /* 1-K stack is enough for us */
                 RT_LOWEST_PRIORITY, /* lowest is fine for our 1 task */
                 0,             /* uses floating point; we don't */
                 0);            /* signal handler; we don't use signals */
  if (0 != retval) {
    printk("rt_task_init error\n");
    return retval;
  }

  rt_set_runnable_on_cpus(&task, RUN_ON_CPUS);
  if (timer_mode) { 
      rt_set_periodic_mode();
  } else {
      rt_set_oneshot_mode();
  }
  rt_assign_irq_to_cpu(RTAI_APIC_TIMER_IPI, TIMER_TO_CPU);
  //rt_assign_irq_to_cpu(TIMER_8254_IRQ, TIMER_TO_CPU);
  period_count = nano2count(period_ns);
  timer_period_count = start_rt_timer(period_count);

  expected = rt_get_time() + period_count;
  retval =
    rt_task_make_periodic(&task, /* pointer to our task structure */
                          /* start one cycle from now */
                          expected,
                          timer_period_count); /* recurring period */
  if (0 != retval) {
    printk("rt_task_make_periodic error\n");
    return retval;
  }

  return 0;
}

void cleanup_module(void)
{
  int retval;

  if (start_timer) {
      rt_reset_irq_to_sym_mode(RTAI_APIC_TIMER_IPI);
      stop_rt_timer();
  }

  retval = rt_task_delete(&task);

  if (0 !=  retval) {
      printk("periodic task: error stopping task\n");
  }

  printk("counter = %d\n", counter);
  return;
}
