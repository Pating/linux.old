/* $Id: time.c,v 1.2 1997/04/10 03:02:35 davem Exp $
 * time.c: UltraSparc timer and TOD clock support.
 *
 * Copyright (C) 1997 David S. Miller (davem@caip.rutgers.edu)
 *
 * Based largely on code which is:
 *
 * Copyright (C) 1996 Thomas K. Dyas (tdyas@eden.rutgers.edu)
 */

#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/param.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/interrupt.h>
#include <linux/timex.h>
#include <linux/init.h>

#include <asm/oplib.h>
#include <asm/mostek.h>
#include <asm/irq.h>
#include <asm/io.h>

struct mostek48t02 *mstk48t02_regs = 0;
struct mostek48t08 *mstk48t08_regs = 0;
struct mostek48t59 *mstk48t59_regs = 0;

static int set_rtc_mmss(unsigned long);

/* timer_interrupt() needs to keep up the real-time clock,
 * as well as call the "do_timer()" routine every clocktick
 *
 * NOTE: On SUN5 systems the ticker interrupt comes in using 2
 *       interrupts, one at level14 and one with softint bit 0.
 */
void timer_interrupt(int irq, void *dev_id, struct pt_regs * regs)
{
	/* last time the cmos clock got updated */
	static long last_rtc_update=0;

	do_timer(regs);

	/* Determine when to update the Mostek clock. */
	if (time_state != TIME_BAD && xtime.tv_sec > last_rtc_update + 660 &&
	    xtime.tv_usec > 500000 - (tick >> 1) &&
	    xtime.tv_usec < 500000 + (tick >> 1))
	  if (set_rtc_mmss(xtime.tv_sec) == 0)
	    last_rtc_update = xtime.tv_sec;
	  else
	    last_rtc_update = xtime.tv_sec - 600; /* do it again in 60 s */
}

/* Converts Gregorian date to seconds since 1970-01-01 00:00:00.
 * Assumes input in normal date format, i.e. 1980-12-31 23:59:59
 * => year=1980, mon=12, day=31, hour=23, min=59, sec=59.
 *
 * [For the Julian calendar (which was used in Russia before 1917,
 * Britain & colonies before 1752, anywhere else before 1582,
 * and is still in use by some communities) leave out the
 * -year/100+year/400 terms, and add 10.]
 *
 * This algorithm was first published by Gauss (I think).
 *
 * WARNING: this function will overflow on 2106-02-07 06:28:16 on
 * machines were long is 32-bit! (However, as time_t is signed, we
 * will already get problems at other places on 2038-01-19 03:14:08)
 */
static inline unsigned long mktime(unsigned int year, unsigned int mon,
	unsigned int day, unsigned int hour,
	unsigned int min, unsigned int sec)
{
	if (0 >= (int) (mon -= 2)) {	/* 1..12 -> 11,12,1..10 */
		mon += 12;	/* Puts Feb last since it has leap day */
		year -= 1;
	}
	return (((
	    (unsigned long)(year/4 - year/100 + year/400 + 367*mon/12 + day) +
	      year*365 - 719499
	    )*24 + hour /* now have hours */
	   )*60 + min /* now have minutes */
	  )*60 + sec; /* finally seconds */
}

/* Kick start a stopped clock (procedure from the Sun NVRAM/hostid FAQ). */
static void kick_start_clock(void)
{
	register struct mostek48t02 *regs = mstk48t02_regs;
	unsigned char sec;
	int i, count;

	prom_printf("CLOCK: Clock was stopped. Kick start ");

	/* Turn on the kick start bit to start the oscillator. */
	regs->creg |= MSTK_CREG_WRITE;
	regs->sec &= ~MSTK_STOP;
	regs->hour |= MSTK_KICK_START;
	regs->creg &= ~MSTK_CREG_WRITE;

	/* Delay to allow the clock oscillator to start. */
	sec = MSTK_REG_SEC(regs);
	for (i = 0; i < 3; i++) {
		while (sec == MSTK_REG_SEC(regs))
			for (count = 0; count < 100000; count++)
				/* nothing */ ;
		prom_printf(".");
		sec = regs->sec;
	}
	prom_printf("\n");

	/* Turn off kick start and set a "valid" time and date. */
	regs->creg |= MSTK_CREG_WRITE;
	regs->hour &= ~MSTK_KICK_START;
	MSTK_SET_REG_SEC(regs,0);
	MSTK_SET_REG_MIN(regs,0);
	MSTK_SET_REG_HOUR(regs,0);
	MSTK_SET_REG_DOW(regs,5);
	MSTK_SET_REG_DOM(regs,1);
	MSTK_SET_REG_MONTH(regs,8);
	MSTK_SET_REG_YEAR(regs,1996 - MSTK_YEAR_ZERO);
	regs->creg &= ~MSTK_CREG_WRITE;

	/* Ensure the kick start bit is off. If it isn't, turn it off. */
	while (regs->hour & MSTK_KICK_START) {
		prom_printf("CLOCK: Kick start still on!\n");
		regs->creg |= MSTK_CREG_WRITE;
		regs->hour &= ~MSTK_KICK_START;
		regs->creg &= ~MSTK_CREG_WRITE;
	}

	prom_printf("CLOCK: Kick start procedure successful.\n");
}

/* Return nonzero if the clock chip battery is low. */
static int has_low_battery(void)
{
	register struct mostek48t02 *regs = mstk48t02_regs;
	unsigned char data1, data2;

	data1 = regs->eeprom[0];	/* Read some data. */
	regs->eeprom[0] = ~data1;	/* Write back the complement. */
	data2 = regs->eeprom[0];	/* Read back the complement. */
	regs->eeprom[0] = data1;	/* Restore the original value. */

	return (data1 == data2);	/* Was the write blocked? */
}

/* XXX HACK HACK HACK, delete me soon */
static struct linux_prom_ranges XXX_sbus_ranges[PROMREG_MAX];
static int XXX_sbus_nranges;

/* Probe for the real time clock chip. */
__initfunc(static void clock_probe(void))
{
	struct linux_prom_registers clk_reg[2];
	char model[128];
	int node, sbusnd, err;

	node = prom_getchild(prom_root_node);
	sbusnd = prom_searchsiblings(node, "sbus");
	node = prom_getchild(sbusnd);

	if(node == 0 || node == -1) {
		prom_printf("clock_probe: Serious problem can't find sbus PROM node.\n");
		prom_halt();
	}

	/* XXX FIX ME */
	err = prom_getproperty(sbusnd, "ranges", (char *) XXX_sbus_ranges,
			       sizeof(XXX_sbus_ranges));
	if(err == -1) {
		prom_printf("clock_probe: Cannot get XXX sbus ranges\n");
		prom_halt();
	}
	XXX_sbus_nranges = (err / sizeof(struct linux_prom_ranges));

	while(1) {
		prom_getstring(node, "model", model, sizeof(model));
		if(strcmp(model, "mk48t02") &&
		   strcmp(model, "mk48t08") &&
		   strcmp(model, "mk48t59")) {
			node = prom_getsibling(node);
			if(node == 0) {
				prom_printf("clock_probe: Cannot find timer chip\n");
				prom_halt();
			}
			continue;
		}

		err = prom_getproperty(node, "reg", (char *)clk_reg,
				       sizeof(clk_reg));
		if(err == -1) {
			prom_printf("clock_probe: Cannot make Mostek\n");
			prom_halt();
		}

		/* XXX fix me badly */
		prom_adjust_regs(clk_reg, 1, XXX_sbus_ranges, XXX_sbus_nranges);

		if(model[5] == '0' && model[6] == '2') {
			mstk48t02_regs = (struct mostek48t02 *)
				sparc_alloc_io(clk_reg[0].phys_addr,
					       (void *) 0, sizeof(*mstk48t02_regs),
					       "clock", clk_reg[0].which_io, 0x0);
		} else if(model[5] == '0' && model[6] == '8') {
			mstk48t08_regs = (struct mostek48t08 *)
				sparc_alloc_io(clk_reg[0].phys_addr,
					       (void *) 0, sizeof(*mstk48t08_regs),
					       "clock", clk_reg[0].which_io, 0x0);
			mstk48t02_regs = &mstk48t08_regs->regs;
		} else {
			mstk48t59_regs = (struct mostek48t59 *)
				sparc_alloc_io(clk_reg[0].phys_addr,
					       (void *) 0, sizeof(*mstk48t59_regs),
					       "clock", clk_reg[0].which_io, 0x0);
			mstk48t02_regs = &mstk48t59_regs->regs;
		}
		break;
	}

	/* Report a low battery voltage condition. */
	if (has_low_battery())
		prom_printf("NVRAM: Low battery voltage!\n");

	/* Kick start the clock if it is completely stopped. */
	if (mstk48t02_regs->sec & MSTK_STOP)
		kick_start_clock();
}

#ifndef BCD_TO_BIN
#define BCD_TO_BIN(val) (((val)&15) + ((val)>>4)*10)
#endif

#ifndef BIN_TO_BCD
#define BIN_TO_BCD(val) ((((val)/10)<<4) + (val)%10)
#endif

__initfunc(void time_init(void))
{
	extern void init_timers(void (*func)(int, void *, struct pt_regs *));
	unsigned int year, mon, day, hour, min, sec;
	struct mostek48t02 *mregs;

	do_get_fast_time = do_gettimeofday;

	clock_probe();
	init_timers(timer_interrupt);

	mregs = mstk48t02_regs;
	if(!mregs) {
		prom_printf("Something wrong, clock regs not mapped yet.\n");
		prom_halt();
	}		

	mregs->creg |= MSTK_CREG_READ;
	sec = MSTK_REG_SEC(mregs);
	min = MSTK_REG_MIN(mregs);
	hour = MSTK_REG_HOUR(mregs);
	day = MSTK_REG_DOM(mregs);
	mon = MSTK_REG_MONTH(mregs);
	year = MSTK_CVT_YEAR( MSTK_REG_YEAR(mregs) );
	xtime.tv_sec = mktime(year, mon, day, hour, min, sec);
	xtime.tv_usec = 0;
	mregs->creg &= ~MSTK_CREG_READ;
}

static __inline__ unsigned long do_gettimeoffset(void)
{
	unsigned long offset = 0;
	unsigned int count;

	/* XXX -DaveM */
#if 0
	count = (*master_l10_counter >> 10) & 0x1fffff;
#else
	count = 0;
#endif

	if(test_bit(TIMER_BH, &bh_active))
		offset = 1000000;

	return offset + count;
}

void do_gettimeofday(struct timeval *tv)
{
	unsigned long flags;

	save_and_cli(flags);
	*tv = xtime;
	tv->tv_usec += do_gettimeoffset();
	if(tv->tv_usec >= 1000000) {
		tv->tv_usec -= 1000000;
		tv->tv_sec++;
	}
	restore_flags(flags);
}

void do_settimeofday(struct timeval *tv)
{
	cli();

	tv->tv_usec -= do_gettimeoffset();
	if(tv->tv_usec < 0) {
		tv->tv_usec += 1000000;
		tv->tv_sec--;
	}

	xtime = *tv;
	time_state = TIME_BAD;
	time_maxerror = 0x70000000;
	time_esterror = 0x70000000;
	sti();
}

static int set_rtc_mmss(unsigned long nowtime)
{
	int real_seconds, real_minutes, mostek_minutes;
	struct mostek48t02 *regs = mstk48t02_regs;

	/* Not having a register set can lead to trouble. */
	if (!regs) 
		return -1;

	/* Read the current RTC minutes. */
	regs->creg |= MSTK_CREG_READ;
	mostek_minutes = MSTK_REG_MIN(regs);
	regs->creg &= ~MSTK_CREG_READ;

	/*
	 * since we're only adjusting minutes and seconds,
	 * don't interfere with hour overflow. This avoids
	 * messing with unknown time zones but requires your
	 * RTC not to be off by more than 15 minutes
	 */
	real_seconds = nowtime % 60;
	real_minutes = nowtime / 60;
	if (((abs(real_minutes - mostek_minutes) + 15)/30) & 1)
		real_minutes += 30;	/* correct for half hour time zone */
	real_minutes %= 60;

	if (abs(real_minutes - mostek_minutes) < 30) {
		regs->creg |= MSTK_CREG_WRITE;
		MSTK_SET_REG_SEC(regs,real_seconds);
		MSTK_SET_REG_MIN(regs,real_minutes);
		regs->creg &= ~MSTK_CREG_WRITE;
	} else
		return -1;

	return 0;
}
