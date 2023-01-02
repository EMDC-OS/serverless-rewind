#include <linux/kernel_stat.h>
#include <linux/mm.h>
#include <linux/sched/mm.h>
#include <linux/sched/coredump.h>
#include <linux/sched/numa_balancing.h>
#include <linux/sched/task.h>
#include <linux/hugetlb.h>
#include <linux/mman.h>
#include <linux/swap.h>
#include <linux/highmem.h>
#include <linux/pagemap.h>
#include <linux/memremap.h>
#include <linux/ksm.h>
#include <linux/rmap.h>
#include <linux/export.h>
#include <linux/delayacct.h>
#include <linux/init.h>
#include <linux/pfn_t.h>
#include <linux/writeback.h>
#include <linux/memcontrol.h>
#include <linux/mmu_notifier.h>
#include <linux/kallsyms.h>
#include <linux/swapops.h>
#include <linux/elf.h>
#include <linux/gfp.h>
#include <linux/migrate.h>
#include <linux/string.h>
#include <linux/dma-debug.h>
#include <linux/debugfs.h>
#include <linux/userfaultfd_k.h>
#include <linux/dax.h>
#include <linux/oom.h>

#include <trace/events/kmem.h>

#include <asm/io.h>
#include <asm/mmu_context.h>
#include <asm/pgalloc.h>
#include <linux/uaccess.h>
#include <asm/tlb.h>
#include <asm/tlbflush.h>
#include <asm/pgtable.h>

#include "internal.h"

/* For REWIND */
#include <linux/mm_rewind.h>

/* ******Go to header file***************** */
#define FAULT_INIT 0x0
#define FAULT_NEW_RO 0x1
#define FAULT_NEW_WRS 0x2
#define FAULT_EXT_CoW 0x3
#define FAULT_EXT_SHR 0x4
#define FAULT_ERROR 0x5

#define SOMETHING_ERROR 0x1

/* Rewind objects */
struct mm_rewind_pt {
	// Something...
	struct mm_struct *cloned_mm;
	pid_t pid;
}
/* *************************************** */

/* do_fault -> read fault */
/* do_annonymous_fault -> no write flag */
int mm_rewind_new_read(struct mm_rewind_pt *cloned_pt, struct vm_fault *vmf) {
	/* new pte alloc & mapping given address */
	unsigned int address = vmf->address;
	struct vm_area_struct *vma = vmf->vma
	struct mm_struct *mm = cloned_pt->cloned_mm;
	struct page *page = vmf->page;
	pte_t entry;
	
	/* ???? */
	// Upper level page table allocation
	pgd = pgd_offset(mm, address);
	p4d = p4d_alloc(mm, pgd, address);
	pud = pud_alloc(mm, p4d, address);
	pmd = pmd_alloc(mm, pud, address);
	
	// Create pte and map the page
	/* page = alloc_zeroed_user_highpage_movable(vma, address); // already allocated */
	entry = mk_pte(page, vma->vm_page_prot);
	pte = pte_offset_map_lock(mm, pmd, address, &vmf->ptl);
	/* ???? */

	return 0;
}

/* do_fault -> cow_fault, shared_fault */
/* do_annoymous_fault -> write flag */
int mm_rewind_new_writable(void) {
	/* new page alloc */
	/* new pte alloc */
	// Allocate page and pte
	
	// Starting case statement (Will be modified)
	// New allocation case (NEW_RO, NEW_WRS)
	struct mem_cgroup *memcg;
	/* ??? */
	if (pte_alloc(mm, pmd))
		goto err;
	/*******/

	// Text, Data fault (do_fault) path -> no more new page
	pte = pte_alloc_one(mm);
	entry = vmf->page;	// fixed (CoW page is set on cow_fault)
	// Read fault
	// CoW fault (vmf->cow_page)
	// Share fault
	// Common part (__do_fault is related to allocate page -> no needed)
	ret = 0;
	if (!(vma->vm_flags & VM_SHARED))
		ret = check_stable_address_space(vmf->vma->vm_mm);
	if (!ret)
		// create pte and map to page
		ret = alloc_set_pte(vmf, vmf->memcg, entry); // Will be modified for rewind case
	if (pte)
		pte_unmap_unlock(pte, ptl);
		
	
	// Anon fault path
	// Read path
	pte = pte_offset_map(pmd, address);
	//orig_pte = *pte;
	// pte_wrprotect(pte); // clear RW bit (no write)
	if (!pte_none(pte))
		goto unlock;
	if (check_stable_address_space(mm))
		goto unlock
	goto setpte;

	
	// Write path
	if (anon_vma_prepare(vma))
		goto err;
	page = alloc_zeroed_user_highpage_movable(vma, address);
	if (!page)
		goto err;
	if (mem_cgroup_try_charge_delay(page, mm, GFP_KERNEL, &memcg, false))
		goto err;
	
	__SetPageUpdate(page);
	
	entry = mk_pte(page, vma->vm_page_prot);
	entry = pte_wrprotect(entry); // remove write flag (wp_fault will occur at execution phase)
	pte = pte_offset_map_lock(mm, pmd, address, &ptl);
	if (!pte_none(*pte))
		goto release;
	if (check_stable_address_space(mm))
		goto release;
	
	inc_mm_counter_fast(mm, MM_ANONPAGES);
	page_add_new_anon_rmap(page, vma, address, false);
	mem_cgroup_commit_charge(page, memcg, false, false);
	lru_cache_add_active_or_unevictable(page, vma);
setpte:
	set_pte_at(mm, address, pte, entry);
	update_mmu_cache(vma, address, pte);
unlock:
	pte_unmap_unlock(pte, ptl);
	return 0; // ????
release:
	mem_cgroup_cancel_charge(page, memcg, false);
	put_page(page);
	goto unlock;
err:
	return SOMETHING_ERROR;
	
	
/*


	// Each case
	if (vma_is_anonymous(vma)) {
		// something_for_anonymous();
	} else {
		// something_for_not_anonymous();
	}

	ptl = pte_lockptr(mm, pmd);
	spin_lock(ptl);
	entry = orig_pte;
	entry = pte_mkdirty(entry);
	if (ptep_set_access_flags(vma, address, pte, entry, 0)) {
		// 0 -> dirty flag (Is write fault flag set?)
		update_mmu_cache(vma, address, pte);
	} else {
		// If write fault flag set then,
		// flush_tlb_fix_spurious_fault(vma, address);
	}
	pte_unmap_unlock(pte, ptl);

	page = alloc_zeroed_user_highpage_movable(vma, address);
	entry = mk_pte(page, vma->vm_page_prot);
	pte = pte_offset_map_lock(mm, pmd, address, &vmf->ptl);

	// ???? 
	return 0;
err:
	return SOMETHING_ERROR;
*/
}

/* do_wp_page ->
 * (!normal page && read-only page flag) ||
 * [normal page && {(anon page && map_count > 1) || (!anon page && read-only page flag)}] */
int mm_rewind_exist_cow(void) {
	/* Original page not remove (forced, will be done at fault path, on-going) */
	/* Check rewind_pt have read-only page? */
	return 0;
}

/* do_wp_page ->
 * (!normal page && write|share page flag) ||
 * [normal page && {(!anon page && write|shared page flag) || (anon page && map_count == 1)}] */
/* means flag changed */
int mm_rewind_exist_shared(void) {
	/* Original data keep and new page for current process pt (will be done at fault path, on-going) */
	/* Check rewind_pt have read-only page? */
	return 0;
}

int mm_rewind_manager(struct vm_fault *vmf, unsigned int fault_type) {
	// Preprocessing on this function and type specific work done by extra function
	// Function process ID: vmf->vma->vm_mm->owner->pid (value)
	// Page address: vmf->address (value)
	// PGD of process: vmf->vma->vm_mm->pgd (pointer)
	if (fault_type >= FAULT_ERROR)
		goto err;

	/* Initialization for cloned page table */
	if (fault_type == FAULT_INIT) {
		/* create new struct mm_rewind_pt */
		struct mm_rewind_pt *new_pt;
		// copy vmf->vma->vm_mm to new_pt->cloned_mm
		new_pt->pid = vmf->vma->vm_mm->owner->pid;
		return 0;
	}

	
	/* Common pre-works... */

	/* Fault handling */
	if (fault_type == FAULT_NEW_RO)
		if (!mm_rewind_new_read(vmf)) {
			goto err;
		}

	else if (fault_type == FAULT_NEW_WRS)
		if (!mm_rewind_new_writable()) {
			goto err;
		}

	else if (fault_type == FAULT_EXT_CoW)
		if (!mm_rewind_exist_cow()) {
			goto err;
		}

	else if (fault_type == FAULT_EXT_SHR)
		if (!mm_rewind_exist_shared()) {
			goto err;
		}

	/* Common after works... */
	return 0;

err:
	return SOMETHING_ERROR
}

/* Set new pte for cloned_pt at do_fault(): no pte and file/text/data.
 * Same action for read_fault, cow_fault, shared_fault.
 * Simply assign new pte and map to fault page (vmf->page).
 * To Do: More consideration about CoW fault (8/23 13:15)
 */
int do_simple_clone(struct vm_fault *vmf, struct mm_struct *cloned_mm) {
	struct mem_cgroup *memcg;
	struct page *page;
	vm_fault_t ret = 0;
	pte_t *pte;
	spinlock_t *ptl;

	pte = pte_alloc_one(cloned_mm);
        page = vmf->page;      // CoW mapped page is vmf->cow_page and original page is vmf->page

        // Common part (__do_fault is related to allocate page -> no needed)
        if (!(vma->vm_flags & VM_SHARED))
                ret = check_stable_address_space(vmf->vma->vm_mm); // Right..?
        if (!ret)
                // create pte and map to page
                ret = alloc_set_pte(vmf, vmf->memcg, page); // Will be modified for rewind case
        if (pte)
                pte_unmap_unlock(pte, ptl);

	return 0;
}

/* Set new pte for cloned_pt at do_anonymous_fault(): no pte and bss/stack/heap
 * Allocate new page if writable flag set - have to set flags and etc.
 * Map allocated page to new pte
 * To Do: 
 */
int do_anonymous_clone(struct vm_fault *vmf, struct mm_struct cloned_mm, pmd_t *pmd) {
	struct vm_area_struct *vma = vmf->vma; // Right..?
	struct mem_cgroup *memcg;
        struct page *page;
	unsigned long address = vmf->address;
        vm_fault_t ret = 0;
        pte_t *pte;
	pte_t entry;
	spinlock_t *ptl;

	if (!(vmf->flags & FAULT_FLAG_WRITE)) {
	        // Read path
		pte = pte_offset_map(pmd, address);
        	//orig_pte = *pte;
	        // pte_wrprotect(pte); // clear RW bit (no write)
        	if (!pte_none(pte))
 	               goto unlock;
        	if (check_stable_address_space(cloned_mm))
                	goto unlock;
	        goto setpte;
	} else {
        	// Write path
       		if (anon_vma_prepare(vma))
                	goto err;
	        page = alloc_zeroed_user_highpage_movable(vma, address);
        	if (!page)
                	goto err;
	        if (mem_cgroup_try_charge_delay(page, cloned_mm, GFP_KERNEL, &memcg, false))
        	        goto err;

	        __SetPageUpdate(page);

        	entry = mk_pte(page, vma->vm_page_prot);
	        entry = pte_wrprotect(entry); // remove write flag (wp_fault will occur at execution phase)
        	pte = pte_offset_map_lock(cloned_mm, pmd, address, &ptl);
	        if (!pte_none(*pte))
        	        goto release;
	        if (check_stable_address_space(cloned_mm))
        	        goto release;

	        inc_mm_counter_fast(cloned_mm, MM_ANONPAGES);
        	page_add_new_anon_rmap(page, vma, address, false);
	        mem_cgroup_commit_charge(page, memcg, false, false);
        	lru_cache_add_active_or_unevictable(page, vma);
	}

setpte:
        set_pte_at(cloned_mm, address, pte, entry);
        update_mmu_cache(vma, address, pte);
unlock:
        pte_unmap_unlock(pte, ptl);
        return 0; // ????
release:
        mem_cgroup_cancel_charge(page, memcg, false);
        put_page(page);
        goto unlock;
err:
        return SOMETHING_ERROR;
}


int do_mm_rewind(struct vm_fault *vmf) {
	unsigned int address = vmf->address; // 
        struct vm_area_struct *vma = vmf->vma; // Is this right...?
        struct mm_struct *mm = cloned_pt->cloned_mm; // cloned_pt will be change pid-index list
        struct page *page = vmf->page;
        spinlock_t *ptl;
        vm_fault_t ret;
        pgt_t *pgd;
        p4d_t *p4d;
        pud_t *pud;
        pmd_t *pmd;
        pte_t *pte;
        pte_t orig_pte, entry;
	int res;

        /* ???? */
        // Upper level page table allocation
        pgd = pgd_offset(mm, address);
        p4d = p4d_alloc(mm, pgd, address);
        if (!p4d)
                goto err;
        pud = pud_alloc(mm, p4d, address);
        if (!pud)
                goto err;
        /* Some huge page(?) related code on mm/memory.c:3921 */
        pmd = pmd_alloc(mm, pud, address);
        if (!pmd)
                goto err;

	if (!vmf->pte) {
		if (vma_is_anonymous(vmf->vma))
			res = do_anonymous_clone();
		else
			res = do_simple_clone();		
	}

}
