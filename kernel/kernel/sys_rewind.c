#include <linux/syscalls.h>
#include <linux/kernel.h>
#include <linux/linkage.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/signal.h>
#include <linux/mm_rewinder.h>
#include <asm/msr.h>

#include <linux/cpu.h>
#include <linux/cpufreq.h>
#include <linux/smp.h>


static void rewind_threads(void) {
	struct task_struct *tsk;
	int err;
	err = rewind_de_thread(current);
	if (err) {
		printk(KERN_INFO "REWIND(RW): de thread error %d\n", err);
	}

	list_for_each_entry(tsk, &current->children, sibling) {
		printk(KERN_INFO "REWIND(RW): Kill pid %d\n", tsk->pid);
		send_sig (SIGKILL, tsk, 0);
	}

}


SYSCALL_DEFINE1(checkpoint, unsigned long __user, num) {
	unsigned long long tmp = rdtsc();
	printk(KERN_INFO "REWIND(CHK): Current task(%d,%d) is rewindable!\n", current->pid, current->tgid);

	current->rewind_cp = 0;
	current->rewind_cnt = 1;
	current->rewind_pte_cnt = 0;
	current->mm->rewindable = 1;

	copy_pgt(current->mm, 0);
	tmp = rdtsc() - tmp;

	printk(KERN_INFO "REWIND(CHK): Takes %llu ns, pte_cnt %lu\n", tmp, current->rewind_pte_cnt);
	current->rewind_vma_reuse = 0;
	current->rewind_vma_alloc = 0;
	
	return 0;
}

SYSCALL_DEFINE1(rewind, unsigned long __user, num) {
	unsigned long long tmp, tmpt;
	printk(KERN_INFO "REWIND(RW): REWIND start time %llu\n", ktime_get_real_ns());
	
	rewind_threads();
	
	current->rewind_pte = 0;
	current->rewind_clear = 0;
	current->rewind_unmap = 0;
	current->rewind_flush = 0;
	current->rewind_page_cnt = 0;
	current->rewind_pte_cnt = 0;
	current->rewind_total_vma = 0;
	current->rewind_unused_vma = 0;
	current->rewind_vma = 0;

	current->rewind_page_reuse = 0;
	current->rewind_reusable_size = 0;

	current->rewind_page_erase_cnt = 0;
	current->rewind_page_cow_cnt = 0;

	printk(KERN_INFO "REWIND(RW): Test rewind() with one parameter - %lu\n", num);
	printk(KERN_INFO "REWIND(RW): rewindable?: %u\n", current->rewindable);
	current->rewind_cnt++;

	tmpt = ktime_get_ns();
	tmp = rdtsc();
	copy_pgt(current->mm, 1); // return type int (have to if state)
	tmp = rdtsc()-tmp;
	tmpt = ktime_get_ns() - tmpt;

	printk(KERN_INFO "REWIND(RW): End time=%llu, Total_time=%llu, VMA_Unmap=%llu, PTE_copy=%llu, Page_clear=%llu, Flush=%llu, ktime=%llu\n", ktime_get_real_ns(), tmp, current->rewind_unmap, current->rewind_pte, current->rewind_clear, current->rewind_flush, tmpt);
	printk(KERN_INFO "REWIND(RW): Clear_page=%lu, Access_pte=%lu Erase_page=%lu Cow_page=%lu\n", current->rewind_page_cnt, current->rewind_pte_cnt, current->rewind_page_erase_cnt, current->rewind_page_cow_cnt);
	printk(KERN_INFO "REWIND(vma): Total=%lu, rewinds=%lu, unused_set=%lu\n", current->rewind_total_vma, current->rewind_vma, current->rewind_unused_vma);
	printk(KERN_INFO "REWIND(alloc_vma): vma_alloc=%lu, vma_reuse=%lu\n", current->rewind_vma_alloc, current->rewind_vma_reuse);
	printk(KERN_INFO "REWIND(reuse): %lu(current) / %lu(next)", current->rewind_reused_size, current->rewind_reusable_size);
	current->rewind_vma_reuse = 0;
        current->rewind_vma_alloc = 0;
	current->rewind_reused_page = 0;
	current->rewind_reused_size = 0;

	return 0;
}

SYSCALL_DEFINE0(rewindable) {
	printk(KERN_INFO "REWIND(able): Test rewind_parent become 1\n");
	current->rewind_parent = 1;
	printk(KERN_INFO "REWIND(able): rewind_parent = %u, rewindable = %u\n", current->rewind_parent, current->rewindable);
	
	return 0;
}

SYSCALL_DEFINE0(rewind_print) {
	printk(KERN_INFO "REWIND(TIME): Current time = %llu, Total pf time = %llu\n", ktime_get_real_ns(), current->rewind_time);
	printk(KERN_INFO "REWIND(alloc_vma): vma_alloc=%lu, vma_reuse=%lu\n", current->rewind_vma_alloc, current->rewind_vma_reuse);
	current->rewind_vma_reuse = 0;
        current->rewind_vma_alloc = 0;
	return 0;
}

SYSCALL_DEFINE0(rewind_child) {
	current->child_print = 1;
	return 0;
}
