/* Automatically generated. Do not edit. */
#ifndef __ASM_OFFSETS_H__
#define __ASM_OFFSETS_H__

#include <linux/config.h>

#ifndef CONFIG_SMP

#define AOFF_task_state	0x00000000
#define ASIZ_task_state	0x00000004
#define AOFF_task_flags	0x00000004
#define ASIZ_task_flags	0x00000004
#define AOFF_task_sigpending	0x00000008
#define ASIZ_task_sigpending	0x00000004
#define AOFF_task_addr_limit	0x0000000c
#define ASIZ_task_addr_limit	0x00000004
#define AOFF_task_exec_domain	0x00000010
#define ASIZ_task_exec_domain	0x00000004
#define AOFF_task_need_resched	0x00000014
#define ASIZ_task_need_resched	0x00000004
#define AOFF_task_counter	0x00000018
#define ASIZ_task_counter	0x00000004
#define AOFF_task_priority	0x0000001c
#define ASIZ_task_priority	0x00000004
#define AOFF_task_avg_slice	0x00000020
#define ASIZ_task_avg_slice	0x00000004
#define AOFF_task_has_cpu	0x00000024
#define ASIZ_task_has_cpu	0x00000004
#define AOFF_task_processor	0x00000028
#define ASIZ_task_processor	0x00000004
#define AOFF_task_last_processor	0x0000002c
#define ASIZ_task_last_processor	0x00000004
#define AOFF_task_lock_depth	0x00000030
#define ASIZ_task_lock_depth	0x00000004
#define AOFF_task_next_task	0x00000034
#define ASIZ_task_next_task	0x00000004
#define AOFF_task_prev_task	0x00000038
#define ASIZ_task_prev_task	0x00000004
#define AOFF_task_next_run	0x0000003c
#define ASIZ_task_next_run	0x00000004
#define AOFF_task_prev_run	0x00000040
#define ASIZ_task_prev_run	0x00000004
#define AOFF_task_binfmt	0x00000044
#define ASIZ_task_binfmt	0x00000004
#define AOFF_task_exit_code	0x00000048
#define ASIZ_task_exit_code	0x00000004
#define AOFF_task_exit_signal	0x0000004c
#define ASIZ_task_exit_signal	0x00000004
#define AOFF_task_pdeath_signal	0x00000050
#define ASIZ_task_pdeath_signal	0x00000004
#define AOFF_task_personality	0x00000054
#define ASIZ_task_personality	0x00000004
#define AOFF_task_pid	0x0000005c
#define ASIZ_task_pid	0x00000004
#define AOFF_task_pgrp	0x00000060
#define ASIZ_task_pgrp	0x00000004
#define AOFF_task_tty_old_pgrp	0x00000064
#define ASIZ_task_tty_old_pgrp	0x00000004
#define AOFF_task_session	0x00000068
#define ASIZ_task_session	0x00000004
#define AOFF_task_leader	0x0000006c
#define ASIZ_task_leader	0x00000004
#define AOFF_task_p_opptr	0x00000070
#define ASIZ_task_p_opptr	0x00000004
#define AOFF_task_p_pptr	0x00000074
#define ASIZ_task_p_pptr	0x00000004
#define AOFF_task_p_cptr	0x00000078
#define ASIZ_task_p_cptr	0x00000004
#define AOFF_task_p_ysptr	0x0000007c
#define ASIZ_task_p_ysptr	0x00000004
#define AOFF_task_p_osptr	0x00000080
#define ASIZ_task_p_osptr	0x00000004
#define AOFF_task_pidhash_next	0x00000084
#define ASIZ_task_pidhash_next	0x00000004
#define AOFF_task_pidhash_pprev	0x00000088
#define ASIZ_task_pidhash_pprev	0x00000004
#define AOFF_task_tarray_ptr	0x0000008c
#define ASIZ_task_tarray_ptr	0x00000004
#define AOFF_task_wait_chldexit	0x00000090
#define ASIZ_task_wait_chldexit	0x00000004
#define AOFF_task_vfork_sem	0x00000094
#define ASIZ_task_vfork_sem	0x00000004
#define AOFF_task_policy	0x00000098
#define ASIZ_task_policy	0x00000004
#define AOFF_task_rt_priority	0x0000009c
#define ASIZ_task_rt_priority	0x00000004
#define AOFF_task_it_real_value	0x000000a0
#define ASIZ_task_it_real_value	0x00000004
#define AOFF_task_it_prof_value	0x000000a4
#define ASIZ_task_it_prof_value	0x00000004
#define AOFF_task_it_virt_value	0x000000a8
#define ASIZ_task_it_virt_value	0x00000004
#define AOFF_task_it_real_incr	0x000000ac
#define ASIZ_task_it_real_incr	0x00000004
#define AOFF_task_it_prof_incr	0x000000b0
#define ASIZ_task_it_prof_incr	0x00000004
#define AOFF_task_it_virt_incr	0x000000b4
#define ASIZ_task_it_virt_incr	0x00000004
#define AOFF_task_real_timer	0x000000b8
#define ASIZ_task_real_timer	0x00000014
#define AOFF_task_times	0x000000cc
#define ASIZ_task_times	0x00000010
#define AOFF_task_start_time	0x000000dc
#define ASIZ_task_start_time	0x00000004
#define AOFF_task_per_cpu_utime	0x000000e0
#define ASIZ_task_per_cpu_utime	0x00000004
#define AOFF_task_min_flt	0x000000e8
#define ASIZ_task_min_flt	0x00000004
#define AOFF_task_maj_flt	0x000000ec
#define ASIZ_task_maj_flt	0x00000004
#define AOFF_task_nswap	0x000000f0
#define ASIZ_task_nswap	0x00000004
#define AOFF_task_cmin_flt	0x000000f4
#define ASIZ_task_cmin_flt	0x00000004
#define AOFF_task_cmaj_flt	0x000000f8
#define ASIZ_task_cmaj_flt	0x00000004
#define AOFF_task_cnswap	0x000000fc
#define ASIZ_task_cnswap	0x00000004
#define AOFF_task_uid	0x00000102
#define ASIZ_task_uid	0x00000002
#define AOFF_task_euid	0x00000104
#define ASIZ_task_euid	0x00000002
#define AOFF_task_suid	0x00000106
#define ASIZ_task_suid	0x00000002
#define AOFF_task_fsuid	0x00000108
#define ASIZ_task_fsuid	0x00000002
#define AOFF_task_gid	0x0000010a
#define ASIZ_task_gid	0x00000002
#define AOFF_task_egid	0x0000010c
#define ASIZ_task_egid	0x00000002
#define AOFF_task_sgid	0x0000010e
#define ASIZ_task_sgid	0x00000002
#define AOFF_task_fsgid	0x00000110
#define ASIZ_task_fsgid	0x00000002
#define AOFF_task_ngroups	0x00000114
#define ASIZ_task_ngroups	0x00000004
#define AOFF_task_groups	0x00000118
#define ASIZ_task_groups	0x00000040
#define AOFF_task_cap_effective	0x00000158
#define ASIZ_task_cap_effective	0x00000004
#define AOFF_task_cap_inheritable	0x0000015c
#define ASIZ_task_cap_inheritable	0x00000004
#define AOFF_task_cap_permitted	0x00000160
#define ASIZ_task_cap_permitted	0x00000004
#define AOFF_task_user	0x00000164
#define ASIZ_task_user	0x00000004
#define AOFF_task_rlim	0x00000168
#define ASIZ_task_rlim	0x00000050
#define AOFF_task_used_math	0x000001b8
#define ASIZ_task_used_math	0x00000002
#define AOFF_task_comm	0x000001ba
#define ASIZ_task_comm	0x00000010
#define AOFF_task_link_count	0x000001cc
#define ASIZ_task_link_count	0x00000004
#define AOFF_task_tty	0x000001d0
#define ASIZ_task_tty	0x00000004
#define AOFF_task_semundo	0x000001d4
#define ASIZ_task_semundo	0x00000004
#define AOFF_task_semsleeping	0x000001d8
#define ASIZ_task_semsleeping	0x00000004
#define AOFF_task_tss	0x000001e0
#define ASIZ_task_tss	0x00000388
#define AOFF_task_fs	0x00000568
#define ASIZ_task_fs	0x00000004
#define AOFF_task_files	0x0000056c
#define ASIZ_task_files	0x00000004
#define AOFF_task_mm	0x00000570
#define ASIZ_task_mm	0x00000004
#define AOFF_task_sigmask_lock	0x00000574
#define ASIZ_task_sigmask_lock	0x00000001
#define AOFF_task_sig	0x00000578
#define ASIZ_task_sig	0x00000004
#define AOFF_task_signal	0x0000057c
#define ASIZ_task_signal	0x00000008
#define AOFF_task_blocked	0x00000584
#define ASIZ_task_blocked	0x00000008
#define AOFF_task_sigqueue	0x0000058c
#define ASIZ_task_sigqueue	0x00000004
#define AOFF_task_sigqueue_tail	0x00000590
#define ASIZ_task_sigqueue_tail	0x00000004
#define AOFF_task_sas_ss_sp	0x00000594
#define ASIZ_task_sas_ss_sp	0x00000004
#define AOFF_task_sas_ss_size	0x00000598
#define ASIZ_task_sas_ss_size	0x00000004
#define AOFF_task_parent_exec_id	0x0000059c
#define ASIZ_task_parent_exec_id	0x00000004
#define AOFF_task_self_exec_id	0x000005a0
#define ASIZ_task_self_exec_id	0x00000004
#define AOFF_task_oom_kill_try	0x000005a4
#define ASIZ_task_oom_kill_try	0x00000004
#define AOFF_mm_mmap	0x00000000
#define ASIZ_mm_mmap	0x00000004
#define AOFF_mm_mmap_avl	0x00000004
#define ASIZ_mm_mmap_avl	0x00000004
#define AOFF_mm_mmap_cache	0x00000008
#define ASIZ_mm_mmap_cache	0x00000004
#define AOFF_mm_pgd	0x0000000c
#define ASIZ_mm_pgd	0x00000004
#define AOFF_mm_count	0x00000010
#define ASIZ_mm_count	0x00000004
#define AOFF_mm_map_count	0x00000014
#define ASIZ_mm_map_count	0x00000004
#define AOFF_mm_mmap_sem	0x00000018
#define ASIZ_mm_mmap_sem	0x0000000c
#define AOFF_mm_context	0x00000024
#define ASIZ_mm_context	0x00000004
#define AOFF_mm_start_code	0x00000028
#define ASIZ_mm_start_code	0x00000004
#define AOFF_mm_end_code	0x0000002c
#define ASIZ_mm_end_code	0x00000004
#define AOFF_mm_start_data	0x00000030
#define ASIZ_mm_start_data	0x00000004
#define AOFF_mm_end_data	0x00000034
#define ASIZ_mm_end_data	0x00000004
#define AOFF_mm_start_brk	0x00000038
#define ASIZ_mm_start_brk	0x00000004
#define AOFF_mm_brk	0x0000003c
#define ASIZ_mm_brk	0x00000004
#define AOFF_mm_start_stack	0x00000040
#define ASIZ_mm_start_stack	0x00000004
#define AOFF_mm_arg_start	0x00000044
#define ASIZ_mm_arg_start	0x00000004
#define AOFF_mm_arg_end	0x00000048
#define ASIZ_mm_arg_end	0x00000004
#define AOFF_mm_env_start	0x0000004c
#define ASIZ_mm_env_start	0x00000004
#define AOFF_mm_env_end	0x00000050
#define ASIZ_mm_env_end	0x00000004
#define AOFF_mm_rss	0x00000054
#define ASIZ_mm_rss	0x00000004
#define AOFF_mm_total_vm	0x00000058
#define ASIZ_mm_total_vm	0x00000004
#define AOFF_mm_locked_vm	0x0000005c
#define ASIZ_mm_locked_vm	0x00000004
#define AOFF_mm_def_flags	0x00000060
#define ASIZ_mm_def_flags	0x00000004
#define AOFF_mm_cpu_vm_mask	0x00000064
#define ASIZ_mm_cpu_vm_mask	0x00000004
#define AOFF_mm_swap_cnt	0x00000068
#define ASIZ_mm_swap_cnt	0x00000004
#define AOFF_mm_swap_address	0x0000006c
#define ASIZ_mm_swap_address	0x00000004
#define AOFF_mm_segments	0x00000070
#define ASIZ_mm_segments	0x00000004
#define AOFF_thread_uwinmask	0x00000000
#define ASIZ_thread_uwinmask	0x00000004
#define AOFF_thread_kregs	0x00000004
#define ASIZ_thread_kregs	0x00000004
#define AOFF_thread_sig_address	0x00000008
#define ASIZ_thread_sig_address	0x00000004
#define AOFF_thread_sig_desc	0x0000000c
#define ASIZ_thread_sig_desc	0x00000004
#define AOFF_thread_ksp	0x00000010
#define ASIZ_thread_ksp	0x00000004
#define AOFF_thread_kpc	0x00000014
#define ASIZ_thread_kpc	0x00000004
#define AOFF_thread_kpsr	0x00000018
#define ASIZ_thread_kpsr	0x00000004
#define AOFF_thread_kwim	0x0000001c
#define ASIZ_thread_kwim	0x00000004
#define AOFF_thread_fork_kpsr	0x00000020
#define ASIZ_thread_fork_kpsr	0x00000004
#define AOFF_thread_fork_kwim	0x00000024
#define ASIZ_thread_fork_kwim	0x00000004
#define AOFF_thread_reg_window	0x00000028
#define ASIZ_thread_reg_window	0x00000200
#define AOFF_thread_rwbuf_stkptrs	0x00000228
#define ASIZ_thread_rwbuf_stkptrs	0x00000020
#define AOFF_thread_w_saved	0x00000248
#define ASIZ_thread_w_saved	0x00000004
#define AOFF_thread_float_regs	0x00000250
#define ASIZ_thread_float_regs	0x00000080
#define AOFF_thread_fsr	0x000002d0
#define ASIZ_thread_fsr	0x00000004
#define AOFF_thread_fpqdepth	0x000002d4
#define ASIZ_thread_fpqdepth	0x00000004
#define AOFF_thread_fpqueue	0x000002d8
#define ASIZ_thread_fpqueue	0x00000080
#define AOFF_thread_flags	0x00000358
#define ASIZ_thread_flags	0x00000004
#define AOFF_thread_current_ds	0x0000035c
#define ASIZ_thread_current_ds	0x00000004
#define AOFF_thread_core_exec	0x00000360
#define ASIZ_thread_core_exec	0x00000020
#define AOFF_thread_new_signal	0x00000380
#define ASIZ_thread_new_signal	0x00000004

#else /* CONFIG_SMP */

#define AOFF_task_state	0x00000000
#define ASIZ_task_state	0x00000004
#define AOFF_task_flags	0x00000004
#define ASIZ_task_flags	0x00000004
#define AOFF_task_sigpending	0x00000008
#define ASIZ_task_sigpending	0x00000004
#define AOFF_task_addr_limit	0x0000000c
#define ASIZ_task_addr_limit	0x00000004
#define AOFF_task_exec_domain	0x00000010
#define ASIZ_task_exec_domain	0x00000004
#define AOFF_task_need_resched	0x00000014
#define ASIZ_task_need_resched	0x00000004
#define AOFF_task_counter	0x00000018
#define ASIZ_task_counter	0x00000004
#define AOFF_task_priority	0x0000001c
#define ASIZ_task_priority	0x00000004
#define AOFF_task_avg_slice	0x00000020
#define ASIZ_task_avg_slice	0x00000004
#define AOFF_task_has_cpu	0x00000024
#define ASIZ_task_has_cpu	0x00000004
#define AOFF_task_processor	0x00000028
#define ASIZ_task_processor	0x00000004
#define AOFF_task_last_processor	0x0000002c
#define ASIZ_task_last_processor	0x00000004
#define AOFF_task_lock_depth	0x00000030
#define ASIZ_task_lock_depth	0x00000004
#define AOFF_task_next_task	0x00000034
#define ASIZ_task_next_task	0x00000004
#define AOFF_task_prev_task	0x00000038
#define ASIZ_task_prev_task	0x00000004
#define AOFF_task_next_run	0x0000003c
#define ASIZ_task_next_run	0x00000004
#define AOFF_task_prev_run	0x00000040
#define ASIZ_task_prev_run	0x00000004
#define AOFF_task_binfmt	0x00000044
#define ASIZ_task_binfmt	0x00000004
#define AOFF_task_exit_code	0x00000048
#define ASIZ_task_exit_code	0x00000004
#define AOFF_task_exit_signal	0x0000004c
#define ASIZ_task_exit_signal	0x00000004
#define AOFF_task_pdeath_signal	0x00000050
#define ASIZ_task_pdeath_signal	0x00000004
#define AOFF_task_personality	0x00000054
#define ASIZ_task_personality	0x00000004
#define AOFF_task_pid	0x0000005c
#define ASIZ_task_pid	0x00000004
#define AOFF_task_pgrp	0x00000060
#define ASIZ_task_pgrp	0x00000004
#define AOFF_task_tty_old_pgrp	0x00000064
#define ASIZ_task_tty_old_pgrp	0x00000004
#define AOFF_task_session	0x00000068
#define ASIZ_task_session	0x00000004
#define AOFF_task_leader	0x0000006c
#define ASIZ_task_leader	0x00000004
#define AOFF_task_p_opptr	0x00000070
#define ASIZ_task_p_opptr	0x00000004
#define AOFF_task_p_pptr	0x00000074
#define ASIZ_task_p_pptr	0x00000004
#define AOFF_task_p_cptr	0x00000078
#define ASIZ_task_p_cptr	0x00000004
#define AOFF_task_p_ysptr	0x0000007c
#define ASIZ_task_p_ysptr	0x00000004
#define AOFF_task_p_osptr	0x00000080
#define ASIZ_task_p_osptr	0x00000004
#define AOFF_task_pidhash_next	0x00000084
#define ASIZ_task_pidhash_next	0x00000004
#define AOFF_task_pidhash_pprev	0x00000088
#define ASIZ_task_pidhash_pprev	0x00000004
#define AOFF_task_tarray_ptr	0x0000008c
#define ASIZ_task_tarray_ptr	0x00000004
#define AOFF_task_wait_chldexit	0x00000090
#define ASIZ_task_wait_chldexit	0x00000004
#define AOFF_task_vfork_sem	0x00000094
#define ASIZ_task_vfork_sem	0x00000004
#define AOFF_task_policy	0x00000098
#define ASIZ_task_policy	0x00000004
#define AOFF_task_rt_priority	0x0000009c
#define ASIZ_task_rt_priority	0x00000004
#define AOFF_task_it_real_value	0x000000a0
#define ASIZ_task_it_real_value	0x00000004
#define AOFF_task_it_prof_value	0x000000a4
#define ASIZ_task_it_prof_value	0x00000004
#define AOFF_task_it_virt_value	0x000000a8
#define ASIZ_task_it_virt_value	0x00000004
#define AOFF_task_it_real_incr	0x000000ac
#define ASIZ_task_it_real_incr	0x00000004
#define AOFF_task_it_prof_incr	0x000000b0
#define ASIZ_task_it_prof_incr	0x00000004
#define AOFF_task_it_virt_incr	0x000000b4
#define ASIZ_task_it_virt_incr	0x00000004
#define AOFF_task_real_timer	0x000000b8
#define ASIZ_task_real_timer	0x00000014
#define AOFF_task_times	0x000000cc
#define ASIZ_task_times	0x00000010
#define AOFF_task_start_time	0x000000dc
#define ASIZ_task_start_time	0x00000004
#define AOFF_task_per_cpu_utime	0x000000e0
#define ASIZ_task_per_cpu_utime	0x00000080
#define AOFF_task_min_flt	0x000001e0
#define ASIZ_task_min_flt	0x00000004
#define AOFF_task_maj_flt	0x000001e4
#define ASIZ_task_maj_flt	0x00000004
#define AOFF_task_nswap	0x000001e8
#define ASIZ_task_nswap	0x00000004
#define AOFF_task_cmin_flt	0x000001ec
#define ASIZ_task_cmin_flt	0x00000004
#define AOFF_task_cmaj_flt	0x000001f0
#define ASIZ_task_cmaj_flt	0x00000004
#define AOFF_task_cnswap	0x000001f4
#define ASIZ_task_cnswap	0x00000004
#define AOFF_task_uid	0x000001fa
#define ASIZ_task_uid	0x00000002
#define AOFF_task_euid	0x000001fc
#define ASIZ_task_euid	0x00000002
#define AOFF_task_suid	0x000001fe
#define ASIZ_task_suid	0x00000002
#define AOFF_task_fsuid	0x00000200
#define ASIZ_task_fsuid	0x00000002
#define AOFF_task_gid	0x00000202
#define ASIZ_task_gid	0x00000002
#define AOFF_task_egid	0x00000204
#define ASIZ_task_egid	0x00000002
#define AOFF_task_sgid	0x00000206
#define ASIZ_task_sgid	0x00000002
#define AOFF_task_fsgid	0x00000208
#define ASIZ_task_fsgid	0x00000002
#define AOFF_task_ngroups	0x0000020c
#define ASIZ_task_ngroups	0x00000004
#define AOFF_task_groups	0x00000210
#define ASIZ_task_groups	0x00000040
#define AOFF_task_cap_effective	0x00000250
#define ASIZ_task_cap_effective	0x00000004
#define AOFF_task_cap_inheritable	0x00000254
#define ASIZ_task_cap_inheritable	0x00000004
#define AOFF_task_cap_permitted	0x00000258
#define ASIZ_task_cap_permitted	0x00000004
#define AOFF_task_user	0x0000025c
#define ASIZ_task_user	0x00000004
#define AOFF_task_rlim	0x00000260
#define ASIZ_task_rlim	0x00000050
#define AOFF_task_used_math	0x000002b0
#define ASIZ_task_used_math	0x00000002
#define AOFF_task_comm	0x000002b2
#define ASIZ_task_comm	0x00000010
#define AOFF_task_link_count	0x000002c4
#define ASIZ_task_link_count	0x00000004
#define AOFF_task_tty	0x000002c8
#define ASIZ_task_tty	0x00000004
#define AOFF_task_semundo	0x000002cc
#define ASIZ_task_semundo	0x00000004
#define AOFF_task_semsleeping	0x000002d0
#define ASIZ_task_semsleeping	0x00000004
#define AOFF_task_tss	0x000002d8
#define ASIZ_task_tss	0x00000388
#define AOFF_task_fs	0x00000660
#define ASIZ_task_fs	0x00000004
#define AOFF_task_files	0x00000664
#define ASIZ_task_files	0x00000004
#define AOFF_task_mm	0x00000668
#define ASIZ_task_mm	0x00000004
#define AOFF_task_sigmask_lock	0x0000066c
#define ASIZ_task_sigmask_lock	0x00000001
#define AOFF_task_sig	0x00000670
#define ASIZ_task_sig	0x00000004
#define AOFF_task_signal	0x00000674
#define ASIZ_task_signal	0x00000008
#define AOFF_task_blocked	0x0000067c
#define ASIZ_task_blocked	0x00000008
#define AOFF_task_sigqueue	0x00000684
#define ASIZ_task_sigqueue	0x00000004
#define AOFF_task_sigqueue_tail	0x00000688
#define ASIZ_task_sigqueue_tail	0x00000004
#define AOFF_task_sas_ss_sp	0x0000068c
#define ASIZ_task_sas_ss_sp	0x00000004
#define AOFF_task_sas_ss_size	0x00000690
#define ASIZ_task_sas_ss_size	0x00000004
#define AOFF_task_parent_exec_id	0x00000694
#define ASIZ_task_parent_exec_id	0x00000004
#define AOFF_task_self_exec_id	0x00000698
#define ASIZ_task_self_exec_id	0x00000004
#define AOFF_task_oom_kill_try	0x0000069c
#define ASIZ_task_oom_kill_try	0x00000004
#define AOFF_mm_mmap	0x00000000
#define ASIZ_mm_mmap	0x00000004
#define AOFF_mm_mmap_avl	0x00000004
#define ASIZ_mm_mmap_avl	0x00000004
#define AOFF_mm_mmap_cache	0x00000008
#define ASIZ_mm_mmap_cache	0x00000004
#define AOFF_mm_pgd	0x0000000c
#define ASIZ_mm_pgd	0x00000004
#define AOFF_mm_count	0x00000010
#define ASIZ_mm_count	0x00000004
#define AOFF_mm_map_count	0x00000014
#define ASIZ_mm_map_count	0x00000004
#define AOFF_mm_mmap_sem	0x00000018
#define ASIZ_mm_mmap_sem	0x0000000c
#define AOFF_mm_context	0x00000024
#define ASIZ_mm_context	0x00000004
#define AOFF_mm_start_code	0x00000028
#define ASIZ_mm_start_code	0x00000004
#define AOFF_mm_end_code	0x0000002c
#define ASIZ_mm_end_code	0x00000004
#define AOFF_mm_start_data	0x00000030
#define ASIZ_mm_start_data	0x00000004
#define AOFF_mm_end_data	0x00000034
#define ASIZ_mm_end_data	0x00000004
#define AOFF_mm_start_brk	0x00000038
#define ASIZ_mm_start_brk	0x00000004
#define AOFF_mm_brk	0x0000003c
#define ASIZ_mm_brk	0x00000004
#define AOFF_mm_start_stack	0x00000040
#define ASIZ_mm_start_stack	0x00000004
#define AOFF_mm_arg_start	0x00000044
#define ASIZ_mm_arg_start	0x00000004
#define AOFF_mm_arg_end	0x00000048
#define ASIZ_mm_arg_end	0x00000004
#define AOFF_mm_env_start	0x0000004c
#define ASIZ_mm_env_start	0x00000004
#define AOFF_mm_env_end	0x00000050
#define ASIZ_mm_env_end	0x00000004
#define AOFF_mm_rss	0x00000054
#define ASIZ_mm_rss	0x00000004
#define AOFF_mm_total_vm	0x00000058
#define ASIZ_mm_total_vm	0x00000004
#define AOFF_mm_locked_vm	0x0000005c
#define ASIZ_mm_locked_vm	0x00000004
#define AOFF_mm_def_flags	0x00000060
#define ASIZ_mm_def_flags	0x00000004
#define AOFF_mm_cpu_vm_mask	0x00000064
#define ASIZ_mm_cpu_vm_mask	0x00000004
#define AOFF_mm_swap_cnt	0x00000068
#define ASIZ_mm_swap_cnt	0x00000004
#define AOFF_mm_swap_address	0x0000006c
#define ASIZ_mm_swap_address	0x00000004
#define AOFF_mm_segments	0x00000070
#define ASIZ_mm_segments	0x00000004
#define AOFF_thread_uwinmask	0x00000000
#define ASIZ_thread_uwinmask	0x00000004
#define AOFF_thread_kregs	0x00000004
#define ASIZ_thread_kregs	0x00000004
#define AOFF_thread_sig_address	0x00000008
#define ASIZ_thread_sig_address	0x00000004
#define AOFF_thread_sig_desc	0x0000000c
#define ASIZ_thread_sig_desc	0x00000004
#define AOFF_thread_ksp	0x00000010
#define ASIZ_thread_ksp	0x00000004
#define AOFF_thread_kpc	0x00000014
#define ASIZ_thread_kpc	0x00000004
#define AOFF_thread_kpsr	0x00000018
#define ASIZ_thread_kpsr	0x00000004
#define AOFF_thread_kwim	0x0000001c
#define ASIZ_thread_kwim	0x00000004
#define AOFF_thread_fork_kpsr	0x00000020
#define ASIZ_thread_fork_kpsr	0x00000004
#define AOFF_thread_fork_kwim	0x00000024
#define ASIZ_thread_fork_kwim	0x00000004
#define AOFF_thread_reg_window	0x00000028
#define ASIZ_thread_reg_window	0x00000200
#define AOFF_thread_rwbuf_stkptrs	0x00000228
#define ASIZ_thread_rwbuf_stkptrs	0x00000020
#define AOFF_thread_w_saved	0x00000248
#define ASIZ_thread_w_saved	0x00000004
#define AOFF_thread_float_regs	0x00000250
#define ASIZ_thread_float_regs	0x00000080
#define AOFF_thread_fsr	0x000002d0
#define ASIZ_thread_fsr	0x00000004
#define AOFF_thread_fpqdepth	0x000002d4
#define ASIZ_thread_fpqdepth	0x00000004
#define AOFF_thread_fpqueue	0x000002d8
#define ASIZ_thread_fpqueue	0x00000080
#define AOFF_thread_flags	0x00000358
#define ASIZ_thread_flags	0x00000004
#define AOFF_thread_current_ds	0x0000035c
#define ASIZ_thread_current_ds	0x00000004
#define AOFF_thread_core_exec	0x00000360
#define ASIZ_thread_core_exec	0x00000020
#define AOFF_thread_new_signal	0x00000380
#define ASIZ_thread_new_signal	0x00000004

#endif /* CONFIG_SMP */

#endif /* __ASM_OFFSETS_H__ */
