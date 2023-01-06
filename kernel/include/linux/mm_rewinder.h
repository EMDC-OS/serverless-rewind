#include <linux/sched.h>

#define ORIG2CP (1)
#define CP2ORIG (2)

#define REWIND_READ		(1)
#define REWIND_COW		(2)
#define REWIND_SHARED		(3)
#define REWIND_ANON_WRITE	(4)
#define REWIND_ANON_NO_WRITE	(5)
#define REWIND_WP_COW		(6)
#define REWIND_WP_REUSE		(7)
#define REWIND_WP_SHARED	(9)

extern struct task_struct *rewind_kth;
extern unsigned long rewind_pf_cnt;

extern void copy_pgt(struct mm_struct *mm, unsigned int rewind_flag);
extern int dup_rewind_pgd(struct mm_struct *mm, unsigned int flag);
extern void add_on_rewind_list(struct vm_fault *vmf);
extern int rewinder_init(void);
extern int rewind_de_thread(struct task_struct *tsk);

extern pgd_t *rewind_pgd_alloc(struct mm_struct *mm);

extern void pf_add_rewind_list(struct vm_fault *vmf, unsigned int rewindable, unsigned int flag);

extern void rewind_pgd_walk(struct mmu_gather *tlb, struct vm_area_struct *vma, unsigned long addr, unsigned long end, unsigned long rewind_flag);
