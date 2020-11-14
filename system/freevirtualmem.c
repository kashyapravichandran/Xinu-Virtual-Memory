/* freemem.c - freemem */

#include <xinu.h>
uint32 count = 0;
uint32 count_value = 10; 
/*------------------------------------------------------------------------
 *  freevirtaulmem  -  Free a virtaul memory block, returning the block to the free list
 *------------------------------------------------------------------------
 */
syscall	freevirtualmem(
	  char		*blkaddr,	/* Pointer to memory block	*/
	  struct memblk* v_memlist		/* free list ptr	*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	memblk	*next, *prev, *block;
	uint32	top;
	
	mask = disable();
	//write_cr3((unsigned long)PD_Base_Kernel);
	block = (struct memblk *)blkaddr;
	//kprintf("I am here\n");
	//kprintf("FFS %x PTPD %x Memlist %x\n",ffs_memlist,pt_memlist,(*v_memlist));
	if((uint32)&ffs_memlist==(uint32)(v_memlist))
		count++;
	//kprintf("PREV %x \n",v_memlist->mnext);
	prev = v_memlist;			/* Walk along free list	*/
	next = v_memlist->mnext;
	while ((next != NULL) && (next < block)) {
		prev = next;
		next = next->mnext;
	}

	if (prev == v_memlist) {		/* Compute top of previous block*/
		top = (uint32) NULL;
	} else {
		top = (uint32) prev + prev->mlength;
	}

	/* Ensure new block does not overlap previous or next blocks	*/
	/*if((uint32)&ffs_memlist==(uint32)(v_memlist))	
	{
		kprintf("=============================\n");
		kprintf(" Address to be freed : %x \n",blkaddr);	
		kprintf("B Length: %d next: %x\n",ffs_memlist.mlength/4096,ffs_memlist.mnext);
		kprintf("B Length: %d next: %x\n",(ffs_memlist.mnext)->mlength/4096,(ffs_memlist.mnext)->mnext);
	}*/
	v_memlist->mlength += PAGE_SIZE;
	
	/* Either coalesce with previous block or add to free list */
	//kprintf("\n PREV %x Length %x",prev,prev->mlength);
	if (top == (uint32) block) { 	/* Coalesce with previous block	*/		
		prev->mlength += PAGE_SIZE;
		block = prev;
	} else {			/* Link into list as new node	*/
		block->mnext = next;
		block->mlength = PAGE_SIZE;
		prev->mnext = block;
	}

	/* Coalesce with next block if adjacent */

	if (((uint32) block + block->mlength) == (uint32) next) {
		block->mlength += next->mlength;
		block->mnext = next->mnext;
	}
	
	/*if((uint32)&ffs_memlist==(uint32)(v_memlist))
	{
		kprintf("A Length: %d next: %x\n",ffs_memlist.mlength/4096,ffs_memlist.mnext);
		kprintf("A Length: %d next: %x\n",(ffs_memlist.mnext)->mlength/4096,(ffs_memlist.mnext)->mnext);
	}*/
		
	//while((uint32)&ffs_memlist==(uint32)(v_memlist)&&count==count_value);
	//write_cr3((unsigned long)proctab[currpid].prpdbr);
	restore(mask);
	return OK;
}



