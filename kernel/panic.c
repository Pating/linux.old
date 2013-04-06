/*
 *  linux/kernel/panic.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 */

/*
 * This function is used through-out the kernel (including mm and fs)
 * to indicate a major problem.
 */
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/reboot.h>
#include <linux/notifier.h>
#include <linux/init.h>
#include <linux/sysrq.h>
#include <linux/interrupt.h>

asmlinkage void sys_sync(void);	/* it's really int */
extern void unblank_console(void);
extern int C_A_D;

int panic_timeout = 0;

struct notifier_block *panic_notifier_list = NULL;

void __init panic_setup(char *str, int *ints)
{
	if (ints[0] == 1)
		panic_timeout = ints[1];
}

NORET_TYPE void panic(const char * fmt, ...)
{
	static char buf[1024];
	va_list args;
#ifdef CONFIG_ARCH_S390
        unsigned long caller = (unsigned long) __builtin_return_address(0);
#endif

	va_start(args, fmt);
	vsprintf(buf, fmt, args);
	va_end(args);
	printk(KERN_EMERG "Kernel panic: %s\n",buf);
	if (current == task[0])
		printk(KERN_EMERG "In swapper task - not syncing\n");
	else if (in_interrupt())
		printk(KERN_EMERG "In interrupt handler - not syncing\n");
	else
		sys_sync();

	unblank_console();

#ifdef __SMP__
	smp_send_stop();
#endif

	notifier_call_chain(&panic_notifier_list, 0, NULL);

	if (panic_timeout > 0)
	{
		/*
	 	 * Delay timeout seconds before rebooting the machine. 
		 * We can't use the "normal" timers since we just panicked..
	 	 */
		printk(KERN_EMERG "Rebooting in %d seconds..",panic_timeout);
		mdelay(panic_timeout*1000);
		/*
		 *	Should we run the reboot notifier. For the moment Im
		 *	choosing not too. It might crash, be corrupt or do
		 *	more harm than good for other reasons.
		 */
		machine_restart(NULL);
	}
#ifdef __sparc__
	{
		extern int stop_a_enabled;
		/* Make sure the user can actually press L1-A */
		stop_a_enabled = 1;
		printk("Press L1-A to return to the boot prom\n");
	}
#endif
#ifdef CONFIG_ARCH_S390
        disabled_wait(caller);
#endif
	sti();
	for(;;) {
		CHECK_EMERGENCY_SYNC
	}
}
