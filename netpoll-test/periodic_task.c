#include <linux/kernel.h> /* decls needed for kernel modules */
#include <linux/module.h> /* decls needed for kernel modules */
#include <linux/version.h>        /* LINUX_VERSION_CODE, KERNEL_VERSION() */

// used by netpoll
#include <linux/init.h>
#include <linux/netpoll.h>
#include <linux/inet.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/in.h>
#include <linux/mutex.h>

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
static RTIME period_ns = 200*1000*1000;  /* 100ms timer period, in nanoseconds */
static RTIME period_count;     /* requested timer period, in counts */
static RTIME timer_period_count;     /* actual timer period, in counts */
static RTIME expected;  

static int counter = 0;
int start_timer = 0;

static struct netpoll* np = NULL;
static struct netpoll np_t;
static char* LOCALIP = "192.168.165.212";
static char* DSTIP = "192.168.165.209";
static int LOCALPORT = 6665;
static int REMOTEPORT = 6666;

void init_netpoll(void)
{
    np_t.name = "LRNG";
    strlcpy(np_t.dev_name, "eth0", IFNAMSIZ);
    np_t.local_ip.ip = in_aton(LOCALIP);
    np_t.remote_ip.ip = in_aton(DSTIP);
    np_t.local_port = LOCALPORT;
    np_t.remote_port = REMOTEPORT;
    memset(np_t.remote_mac, 0xff, ETH_ALEN);
    netpoll_setup(&np_t);
    np = &np_t;
}

void netpoll_send(char* buf)
{          
    int len = strlen(buf);
    netpoll_send_udp(np, buf, len);
}       

void thread_fun(long arg)
{
  int diff = 0;

  while (1) {
      expected += timer_period_count;
      if(!rt_task_wait_period()) {
          if(!timer_mode) {
              diff = (int) count2nano(rt_get_time() - expected);
              printk("diff = %d\n", diff);
              netpoll_send("testest");
          }
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
  
  init_netpoll();

  retval =
    rt_task_init(&task,       /* pointer to our task structure */
                 thread_fun, /* the actual timer function */
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
  rt_assign_irq_to_cpu(TIMER_8254_IRQ, TIMER_TO_CPU);
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
      rt_reset_irq_to_sym_mode(TIMER_8254_IRQ);
      stop_rt_timer();
  }

  retval = rt_task_delete(&task);

  if (0 !=  retval) {
      printk("periodic task: error stopping task\n");
  }

  printk("counter = %d\n", counter);
  return;
}
