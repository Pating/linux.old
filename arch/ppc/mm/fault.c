/*
 *  ARCH/ppc/mm/fault.c
 *
 *  Copyright (C) 1991, 1992, 1993, 1994  Linus Torvalds
 *  Ported to PPC by Gary Thomas
 */

#include <linux/config.h>
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/head.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/types.h>
#include <linux/ptrace.h>
#include <linux/mman.h>
#include <linux/mm.h>

#include <asm/page.h>
#include <asm/pgtable.h>

extern void die_if_kernel(char *, struct pt_regs *, long);
extern void do_page_fault(struct pt_regs *, unsigned long, unsigned long);

#if 0
#define SHOW_FAULTS
#endif

void
DataAccessException(struct pt_regs *regs)
{
  pgd_t *dir;
  pmd_t *pmd;
  pte_t *pte;
  int tries, mode = 0;
  if (user_mode(regs)) mode |= 0x04;
  if (regs->dsisr & 0x02000000) mode |= 0x02;  /* Load/store */
  if (regs->dsisr & 0x08000000) mode |= 0x01;  /* Protection violation */
#if 0
  /*#ifdef SHOW_FAULTS*/
  printk("Data Access Fault - Loc: %x, DSISR: %x, PC: %x", regs->dar, regs->dsisr, regs->nip);
  printk(" PR: %d\n", regs->msr &&(1<<14));
#endif
  if (mode & 0x01)
  {
#if 0
    printk("Write Protect Fault - Loc: %x, DSISR: %x, PC: %x\n", regs->dar, regs->dsisr, regs->nip);
#endif
    do_page_fault(regs, regs->dar, mode);
    return;
  }
  for (tries = 0;  tries < 1;  tries++)
  {
    dir = pgd_offset(current->mm, regs->dar & PAGE_MASK);
    if (dir)
    {
      pmd = pmd_offset(dir, regs->dar & PAGE_MASK);
      if (pmd && pmd_present(*pmd))
      {
	pte = pte_offset(pmd, regs->dar & PAGE_MASK);
	if (pte && pte_present(*pte))
	{
#if 0
	  printk("Page mapped - PTE: %x[%x]\n", pte, *(long *)pte);
#endif
	  MMU_hash_page(&current->tss, regs->dar & PAGE_MASK, pte);
	  return;
	}
      }
    } else
    {
      printk("No PGD\n");
    }
    do_page_fault(regs, regs->dar, mode);
  }
}

void
InstructionAccessException(struct pt_regs *regs)
{
  pgd_t *dir;
  pmd_t *pmd;
  pte_t *pte;
  int tries, mode = 0;
#if 0
  panic("Instruction Access Fault - Loc: %x, DSISR: %x, PC: %x\n", regs->dar, regs->dsisr, regs->nip);
#endif	
  if (user_mode(regs)) mode |= 0x04;
  if (regs->dsisr & 0x02000000) mode |= 0x02;  /* Load/store */
  if (regs->dsisr & 0x08000000) mode |= 0x01;  /* Protection violation */
  if (mode & 0x01)
  {
    do_page_fault(regs, regs->dar, mode);
    return;
  }
  for (tries = 0;  tries < 1;  tries++)
  {
    dir = pgd_offset(current->mm, regs->dar & PAGE_MASK);
    if (dir)
    {
      pmd = pmd_offset(dir, regs->dar & PAGE_MASK);
      if (pmd && pmd_present(*pmd))
      {
	pte = pte_offset(pmd, regs->dar & PAGE_MASK);
	if (pte && pte_present(*pte))
	{
#if 0
	  printk("Page mapped - PTE: %x[%x]\n", pte, *(long *)pte);
#endif
	  MMU_hash_page(&current->tss, regs->dar & PAGE_MASK, pte);
	  return;
	}
      }
    } else
    {
#if 1
  panic("No PGD Instruction Access Fault - Loc: %x, DSISR: %x, PC: %x\n", regs->dar, regs->dsisr, regs->nip);
#endif	
      printk("No PGD\n");
    }
    do_page_fault(regs, regs->dar, mode);
  }
}

/*
 * This routine handles page faults.  It determines the address,
 * and the problem, and then passes it off to one of the appropriate
 * routines.
 *
 * The error_code parameter just the same as in the i386 version:
 *
 *	bit 0 == 0 means no page found, 1 means protection fault
 *	bit 1 == 0 means read, 1 means write
 *	bit 2 == 0 means kernel, 1 means user-mode
 */
void do_page_fault(struct pt_regs *regs, unsigned long address, unsigned long error_code)
{
  struct vm_area_struct * vma;
  unsigned long page;
/*  printk("\ndo_page_fault()\n");*/
#if 1
  for (vma = current->mm->mmap ; ; vma = vma->vm_next)
  {
    if (!vma)
    {
#if 0 /* mfisk */
        {   struct vm_area_struct * mmp;

          for(mmp=current->mm->mmap; mmp; mmp=mmp->vm_next) {
            printk("notinmap: current: %x; mm: %x; mmap: %x; vma: %x to %x flags: %x\n",
                   current, current->mm, mmp, mmp->vm_start, mmp->vm_end, mmp->vm_flags);
          }
        }
#endif
       panic("do_page_fault()  !vma \n");
      panic("not in map: ip = %x; current=%x; mm=%x; mmap=%x; address = %x error_code = %x\n",
	    regs->nip, current, current->mm, current->mm->mmap, address, error_code);
      goto bad_area;
    }
    if (vma->vm_end > address)
      break;
  }
#else
  vma = find_vma(current, address);
  if (!vma)
    goto bad_area;
#endif
  if (vma->vm_start <= address){
    goto good_area;
  }
  if (!(vma->vm_flags & VM_GROWSDOWN))
  {
    panic("stack\n");
    goto bad_area;
  }
  if (vma->vm_end - address > current->rlim[RLIMIT_STACK].rlim_cur)
  {    panic("stack 2\n");
    goto bad_area;
     }
  vma->vm_offset -= vma->vm_start - (address & PAGE_MASK);
  vma->vm_start = (address & PAGE_MASK);
  
  /*
   * Ok, we have a good vm_area for this memory access, so
   * we can handle it..
   */
good_area:
  /*
   * was it a write?
   */
  if (error_code & 2) {
    if (!(vma->vm_flags & VM_WRITE))
    {
      panic("\ndo_page_fault()  write\n");
      panic("\ndo_page_fault() write! current: %x, address:%x, vm_flags: %x, mm: %x; vma(%x) %x to %x\n",
	    current,address,vma->vm_flags,current->mm,vma,vma->vm_start,vma->vm_end);
      goto bad_area;
    }
  } else {
    /* read with protection fault? */
    if (error_code & 1)
    {
      panic("do_page_fault()  error code thing\n");	    
      goto bad_area;
    }
    if (!(vma->vm_flags & (VM_READ | VM_EXEC)))
    {
#if 0
      _printk("vma = %x\n", vma);
      _printk("vma->vm_flags = %x\n", vma->vm_flags);      
      _printk("VM_READ = %x VM_EXEC = %x\n",VM_READ,VM_EXEC);
#endif
#if 0
      printk("vma = %x VM_READ = %x VM_EXEC = %x\n",
	     vma, VM_READ,VM_EXEC);
      printk("vma->vm_start = %x vma->vm_end = %d\n",
	     vma->vm_start, vma->vm_end);
      printk("error_code = %x\n", error_code);      
      printk("regs = %x\n", regs);
      printk("vma->vm_flags = %x\n", vma->vm_flags);
#endif
/*      printk("do_page_fault()  multi thing\n"); */
      goto bad_area; 
    }
  }
  handle_mm_fault(vma, address, error_code & 2); 
  return;
  
  /*
   * Something tried to access memory that isn't in our memory map..
   * Fix it, but check if it's kernel or user first..
   */
bad_area:
  if (user_mode(regs)) {
    panic("Task: %x, PC: %x, bad area! - Addr: %x\n", current, regs->nip, address);
    send_sig(SIGSEGV, current, 1);
    return;
  }
#if 0
  panic("\nKERNEL! Task: %x, PC: %x, bad area! - Addr: %x, PGDIR: %x\n",
	 current, regs->nip, address, current->tss.pg_tables);
#else
  panic("\nKERNELmm! current: %x, address:%x, vm_flags: %x, mm: %x; vma(%x) %x to %x\n",
	current,address,vma->vm_flags,current->mm,vma,vma->vm_start,vma->vm_end);
#endif
  
  while (1) ;
}

va_to_phys(unsigned long address)
{
  pgd_t *dir;
  pmd_t *pmd;
  pte_t *pte;
  dir = pgd_offset(current->mm, address & PAGE_MASK);
  if (dir)
  {
    pmd = pmd_offset(dir, address & PAGE_MASK);
    if (pmd && pmd_present(*pmd))
    {
      pte = pte_offset(pmd, address & PAGE_MASK);
      if (pte && pte_present(*pte))
      {
	return(pte_page(*pte) | (address & ~(PAGE_MASK-1)));
      }
    } else
    {
      return (0);
    }
  } else
  {
    return (0);
  }
}













