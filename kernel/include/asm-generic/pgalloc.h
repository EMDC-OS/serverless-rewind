/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __ASM_GENERIC_PGALLOC_H
#define __ASM_GENERIC_PGALLOC_H

#ifdef CONFIG_MMU

#define GFP_PGTABLE_KERNEL	(GFP_KERNEL | __GFP_ZERO)
#define GFP_PGTABLE_USER	(GFP_PGTABLE_KERNEL | __GFP_ACCOUNT)

/**
 * __pte_alloc_one_kernel - allocate a page for PTE-level kernel page table
 * @mm: the mm_struct of the current context
 *
 * This function is intended for architectures that need
 * anything beyond simple page allocation.
 *
 * Return: pointer to the allocated memory or %NULL on error
 */
static inline pte_t *__pte_alloc_one_kernel(struct mm_struct *mm)
{
	//return (pte_t *)__get_free_page(GFP_PGTABLE_KERNEL);
	struct task_struct *own = mm->owner;
	
	if (own && (own->rewindable == 1)) {
		pte_t *ret;
		struct page *pg;
		ret = (pte_t *)__get_free_pages(GFP_PGTABLE_KERNEL, 1);
		//printk(KERN_INFO "REWIND(pte_alloc): kernel allocate 8KB pgt\n");
		if (ret) {
			pg = pte_page(*ret);
			set_bit(PG_rewind, &(pg->flags));
			//printk(KERN_INFO "REWIND(pte_alloc): (flag=0x%lx)\n", pg->flags);
		}
		return ret;
	}
	else
		return (pte_t *)__get_free_page(GFP_PGTABLE_KERNEL);
}

#ifndef __HAVE_ARCH_PTE_ALLOC_ONE_KERNEL
/**
 * pte_alloc_one_kernel - allocate a page for PTE-level kernel page table
 * @mm: the mm_struct of the current context
 *
 * Return: pointer to the allocated memory or %NULL on error
 */
static inline pte_t *pte_alloc_one_kernel(struct mm_struct *mm)
{
	return __pte_alloc_one_kernel(mm);
}
#endif

/**
 * pte_free_kernel - free PTE-level kernel page table page
 * @mm: the mm_struct of the current context
 * @pte: pointer to the memory containing the page table
 */
static inline void pte_free_kernel(struct mm_struct *mm, pte_t *pte)
{
	struct task_struct *own = mm->owner;
	//free_page((unsigned long)pte);
	if (own && (own->rewindable == 1)) {
                free_pages((unsigned long)pte, 1);
        }
        else {
                free_page((unsigned long)pte);
        }
}

/**
 * __pte_alloc_one - allocate a page for PTE-level user page table
 * @mm: the mm_struct of the current context
 * @gfp: GFP flags to use for the allocation
 *
 * Allocates a page and runs the pgtable_pte_page_ctor().
 *
 * This function is intended for architectures that need
 * anything beyond simple page allocation or must have custom GFP flags.
 *
 * Return: `struct page` initialized as page table or %NULL on error
 */
static inline pgtable_t __pte_alloc_one(struct mm_struct *mm, gfp_t gfp)
{
	struct page *pte;

//	pte = alloc_page(gfp);
        /* REWIND */
	struct task_struct *own = mm->owner;
        if (own && (own->rewindable == 1)) {
		//printk(KERN_INFO "REWIND(pte_alloc): user allocate 8KB pgt\n");
                pte = alloc_pages(gfp, 1);
		if (pte) 
                        set_bit(PG_rewind, &(pte->flags));
		//printk(KERN_INFO "REWIND(pte_alloc): (flag=0x%lx)\n", pte->flags);
        }
	else
                pte = alloc_page(gfp);	

	if (!pte)
		return NULL;
	if (!pgtable_pte_page_ctor(pte)) {
		__free_page(pte);
		return NULL;
	}

	return pte;
}

#ifndef __HAVE_ARCH_PTE_ALLOC_ONE
/**
 * pte_alloc_one - allocate a page for PTE-level user page table
 * @mm: the mm_struct of the current context
 *
 * Allocates a page and runs the pgtable_pte_page_ctor().
 *
 * Return: `struct page` initialized as page table or %NULL on error
 */
static inline pgtable_t pte_alloc_one(struct mm_struct *mm)
{
	return __pte_alloc_one(mm, GFP_PGTABLE_USER);
}
#endif

/*
 * Should really implement gc for free page table pages. This could be
 * done with a reference count in struct page.
 */

/**
 * pte_free - free PTE-level user page table page
 * @mm: the mm_struct of the current context
 * @pte_page: the `struct page` representing the page table
 */
static inline void pte_free(struct mm_struct *mm, struct page *pte_page)
{
	 struct task_struct *own = mm->owner;
	
	pgtable_pte_page_dtor(pte_page);
	//__free_page(pte_page);
	if (own && (own->rewindable == 1)) {
                __free_pages(pte_page, 1);
        }
        else {
                __free_page(pte_page);
        }

}

#endif /* CONFIG_MMU */

#endif /* __ASM_GENERIC_PGALLOC_H */
