#include <linux/module.h>
#include <linux/linkage.h>
#include <linux/sched.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/user.h>
#include <linux/elfcore.h>

#include <asm/setup.h>
#include <asm/machdep.h>
#include <asm/pgtable.h>
#include <asm/irq.h>
#include <asm/semaphore.h>

asmlinkage long long __ashrdi3 (long long, int);
extern char m68k_debug_device[];

extern void dump_thread(struct pt_regs *, struct user *);
extern int dump_fpu(elf_fpregset_t *);

/* platform dependent support */

EXPORT_SYMBOL(memcmp);
EXPORT_SYMBOL(m68k_machtype);
EXPORT_SYMBOL(m68k_cputype);
EXPORT_SYMBOL(m68k_is040or060);
EXPORT_SYMBOL(cache_push);
EXPORT_SYMBOL(cache_push_v);
EXPORT_SYMBOL(cache_clear);
EXPORT_SYMBOL(mm_vtop);
EXPORT_SYMBOL(mm_ptov);
EXPORT_SYMBOL(mm_end_of_chunk);
EXPORT_SYMBOL(m68k_debug_device);
EXPORT_SYMBOL(request_irq);
EXPORT_SYMBOL(free_irq);
EXPORT_SYMBOL(dump_fpu);
EXPORT_SYMBOL(dump_thread);
EXPORT_SYMBOL(strnlen);
EXPORT_SYMBOL(strrchr);
EXPORT_SYMBOL(strstr);

/* The following are special because they're not called
   explicitly (the C compiler generates them).  Fortunately,
   their interface isn't gonna change any time soon now, so
   it's OK to leave it out of version control.  */
EXPORT_SYMBOL_NOVERS(__ashrdi3);
EXPORT_SYMBOL_NOVERS(memcpy);
EXPORT_SYMBOL_NOVERS(memset);

EXPORT_SYMBOL_NOVERS(__down_failed);
EXPORT_SYMBOL_NOVERS(__down_failed_interruptible);
EXPORT_SYMBOL_NOVERS(__up_wakeup);