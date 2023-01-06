#include <linux/module.h>     // included for all kernel modules
#include <linux/init.h>       // included for __init and __exit macros
#include <linux/kthread.h>    // included for threading related functions
#include <linux/sched.h>      // included to create tesk_struct
#include <linux/delay.h>      // included for the sleep/delay function in the thread

#include <linux/slab.h>	      // included for kmalloc
#include <linux/mm.h>
#include <linux/mm_rewinder.h>

struct task_struct *rewind_kth;
unsigned long rewind_pf_cnt;
static LIST_HEAD(pf_rewind_list);
static DEFINE_SPINLOCK(pf_rewind_lock);

struct pf_rewind_info
{
	//struct vm_fault *vmf;
	pid_t pid;
	unsigned int flags;
	unsigned long address;
	pte_t *pte;
	struct list_head list;
};

void add_on_rewind_list(struct vm_fault *vmf){
	// pf_rewind structure
	printk(KERN_INFO "REWIND(add): pid=%d, pte=0x%lx\n", vmf->vma->vm_mm->owner->pid, vmf->pte->pte);
/*
	struct pf_rewind_info *create = kmalloc(sizeof(*create), GFP_KERNEL);

	if (!vmf) {
		printk(KERN_INFO "REWIND(add): Something Wrong...\n");
	} else {
		create->pid = vmf->vma->vm_mm->owner->pid;
		create->flags = vmf->flags;
		create->address = vmf->address;
		create->pte = vmf->pte;
	}

	spin_lock(&pf_rewind_lock);
	list_add_tail(&create->list, &pf_rewind_list);
	spin_unlock(&pf_rewind_lock);

	wake_up_process(rewind_kth);
*/
}

int rewind_manager(void *tmp) {
/*
	unsigned int i = 0;

	while (!kthread_should_stop()) {
		set_current_state(TASK_INTERRUPTIBLE);
		if (list_empty(&pf_rewind_list))
			schedule();
		__set_current_state(TASK_RUNNING);
		
		//printk(KERN_INFO "REWIND(kth): state-%ld\n", rewind_kth->state);
		printk(KERN_INFO "REWIND(kth): wakeup %d time\n", i);
		//printk(KERN_INFO "REWIND(kth): pf cnt = %lu\n", rewind_pf_cnt);
		i++;

		spin_lock(&pf_rewind_lock);
		while (!list_empty(&pf_rewind_list)) {
			// extract pf info
			struct pf_rewind_info *create;
			
			create = list_entry(pf_rewind_list.next,
						struct pf_rewind_info, list);
			list_del_init(&create->list);
			spin_unlock(&pf_rewind_lock);

			// do mm rewind
			if (!create) {
				printk("REWIND(kth): Something Wrong...\n");	
			} else {
				printk(KERN_INFO "REWIND(kth): get %d's page fault\n", create->pid);
				//printk(KERN_INFO "REWIND(kth): flags = 0x%x\n", create->flags);
				printk(KERN_INFO "REWIND(kth): address = 0x%lx\n", create->address);
				//if (create->pte)
				//	printk(KERN_INFO "REWIND(kth): pte(value) = 0x%lx\n", create->pte->pte);
				//else
				//	printk(KERN_INFO "REWIND(kth): no pte\n");
				printk(KERN_INFO "REWIND(kth): ---------------------------------\n");
				kfree(create);
			}

			spin_lock(&pf_rewind_lock);
		}
		spin_unlock(&pf_rewind_lock);

		//msleep(1000);
		//kthread_park(rewind_kth);
		//kthread_parkme();
		//__set_current_state(TASK_PARKED);
		//cond_resched();
	}
	printk(KERN_INFO "Rewinder stop!\n");
*/
	return 0;
}

int __init rewinder_init(void) {
	//struct task_struct *kth;
	rewind_kth = kthread_create(rewind_manager, NULL, "rewinder_kth");
	rewind_pf_cnt = 0;

	if (rewind_kth != NULL) {
		wake_up_process(rewind_kth);
		printk(KERN_INFO "Rewinder kernel thread is running\n");
	} else {
		printk(KERN_INFO "Rewinder ERROR!\n");
		return -1;
	}

	return 0;
}

static void __exit rewinder_exit(void) {
	kthread_stop(rewind_kth);
	return;
}

module_init(rewinder_init)
module_exit(rewinder_exit)


