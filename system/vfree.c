#include<xinu.h>

// Things added for virtual memory

syscall vfree(char* ptr, uint32 nbytes)
{
	int mask;			/* Saved interrupt mask		*/
	struct	memblk	*next, *prev, *block;
	uint32	top;
	pd_t* PD_Base = proctab[currpid].prpdbr;
	pt_t* PT_Base;
	char *physical_ptr = NULL;	
	int i = 0; 
	uint32 dir_offset = 8;
	uint32 table_offset = 0;
	mask = disable();
	//kprintf("\n vfree PDBR: %x",proctab[currpid].prpdbr);
	write_cr3((unsigned int)PD_Base_Kernel);
	//kprintf("\n vfree PDBR: %x",PD_Base_Kernel);

	nbytes = (uint32) roundpage(nbytes);	/* Use memblk multiples	*/
	prev = &ffs_memlist;			/* Walk along free list	*/
	next = ffs_memlist.mnext;
	
	dir_offset = page_base_offset((uint32)ptr);
	table_offset = page_table_offset((uint32)ptr);
	PT_Base = (pt_t *)(PD_Base[dir_offset].pd_base<<12);
	//kprintf("\n PT Base: %x",(uint32)PT_Base);

	for(i=(uint32)ptr;(i-(uint32)ptr)<nbytes;i=i+4096)
	{
		//kprintf("\n Problem lies here ");
		//kprintf("\n Printing Offset here %d:",table_offset+i);
		//kprintf("\n Printing Present bit %d:",PT_Base[table_offset+i].pt_pres);
		dir_offset = page_base_offset((uint32)i);
		table_offset = page_table_offset((uint32)i);
		PT_Base = (pt_t *)(PD_Base[dir_offset].pd_base<<12);		
		if(PT_Base[table_offset].pt_avail == 0)
		{		
			//kprintf("\nReturning because not initialized\n");			
			write_cr3((unsigned int)proctab[currpid].prpdbr);
			restore(mask);
			return SYSERR;
		}
		
	}

	

	//kprintf("\n Here : %x",(uint32)physical_ptr);
	
	

	for(i=(uint32)ptr;(i-(uint32)ptr)<nbytes;i=i+4096)
	{
		dir_offset = page_base_offset((uint32)i);
		table_offset = page_table_offset((uint32)i);
		PT_Base = (pt_t *)(PD_Base[dir_offset].pd_base<<12);
		PT_Base[table_offset].pt_avail = 0;
		proctab[currpid].alloc_vir_pages --;

		//kprintf("\n OFFSET : %d",table_offset+i);
		//kprintf("\n Printing Present bit %d:",PT_Base[table_offset+i].pt_pres);			
		if(PT_Base[table_offset].pt_pres==1)
		{
			// Removing the block
			proctab[currpid].used_ffs_frames--;
			physical_ptr = (char*)((PT_Base[table_offset].pt_base)<<12);
			block = (struct memblk *)physical_ptr;
			if (((uint32) physical_ptr < (uint32) ffs_start)|| ((uint32) physical_ptr > (uint32) ffs_end)) 
			{
				write_cr3((unsigned int)proctab[currpid].prpdbr);
				//kprintf("Returning here\n");
				restore(mask);
				return SYSERR;
			}			
			prev = &ffs_memlist;			/* Walk along free list	*/
			next = ffs_memlist.mnext;
			while ((next != NULL) && (next < block)) {
				prev = next;
				next = next->mnext;
			}

			if (prev == &ffs_memlist) {		/* Compute top of previous block*/
				top = (uint32) NULL;
			} else {
				top = (uint32) prev + prev->mlength;
			}

			/* Ensure new block does not overlap previous or next blocks	*/

			if (((prev != &ffs_memlist) && (uint32) block < top)
			    || ((next != NULL)	&& (uint32) block+PAGE_SIZE>(uint32)next)) {
				write_cr3((unsigned int)proctab[currpid].prpdbr);
				//kprintf("Returning here\n");
				restore(mask);
				return SYSERR;
			}

			ffs_memlist.mlength += PAGE_SIZE;

			/* Either coalesce with previous block or add to free list */

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
		}
		PT_Base[table_offset].pt_pres=0;
	}
	
	write_cr3((unsigned int)proctab[currpid].prpdbr);
	restore(mask);
	return OK;
	

}
