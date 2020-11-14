#include <xinu.h>

/* Memory bounds */

void	*minheap;		/* Start of heap			*/
void	*maxheap;		/* Highest valid heap address		*/
void	*ffs_start;		/* Start of heap address		*/
void	*ffs_end;		/* End of heap address			*/
void 	*pdpt_start;		/* pdpt start address			*/
void 	*pdpt_end;		/* pdpt end address			*/

/*------------------------------------------------------------------------
 * meminit - initialize memory bounds and the free memory list
 *------------------------------------------------------------------------
 */
void	meminit(void) {

       struct	memblk	*memptr;	/* Ptr to memory block		*/

       /* Initialize the free memory list */

       /* Note: we pre-allocate  the "hole" between 640K and 1024K */   
	//maxheap already initialized in i386.c
//       maxheap = (void *)0x600000;	/* Assume 64 Mbytes for now */
       minheap = &end;

       memptr = memlist.mnext = (struct memblk *)roundmb(minheap);
       if ((char *)(maxheap+1) > HOLESTART) {
       	/* create two blocks that straddle the hole */
       	memptr->mnext = (struct memblk *)HOLEEND;
       	memptr->mlength = (int) truncmb((unsigned) HOLESTART -
            		 (unsigned)&end - 4);
       	memptr = (struct memblk *) HOLEEND;
       	memptr->mnext = (struct memblk *) NULL;
       	memptr->mlength = (int) truncmb( (uint32)maxheap - 
       			(uint32)HOLEEND - NULLSTK);
       } else {
       	/* initialize free memory list to one block */
       	memlist.mnext = memptr = (struct memblk *) roundmb(&end);
       	memptr->mnext = (struct memblk *) NULL;
       	memptr->mlength = (uint32) truncmb((uint32)maxheap -
       			(uint32)&end - NULLSTK);
       }

       return;
}

// Things added for virtual memory

void virtualmem_init(){

	struct memblk *memptr;	/* Ptr to memory block */

	pdpt_start = (char *)(XINU_PAGES*PAGE_SIZE);
	pdpt_end = (char *)(MAX_PT_SIZE*PAGE_SIZE + pdpt_start -1);
	ffs_start = (char *)(pdpt_end + 1);
	ffs_end = (char *)(MAX_FFS_SIZE*PAGE_SIZE + ffs_start - 1);
	
	ffs_memlist.mnext = memptr = (struct memblk *)roundpage(ffs_start);
	ffs_memlist.mlength = (uint32) ((uint32)ffs_end - (uint32)ffs_start + 1);
        memptr->mnext = (struct memblk *) NULL;
        memptr->mlength = (uint32) ((uint32)ffs_end - (uint32)ffs_start + 1);
	//kprintf("ffs_start: %d ffs_end: %d\n",ffs_start,ffs_end); 
	
	//kprintf("virtualmem_init: ffs_memptr: %x mnext: %x mlength: %d\n",memptr,memptr->mnext,memptr->mlength);
	
	pt_memlist.mnext = memptr = (struct memblk *)roundpage(pdpt_start);
	pt_memlist.mlength = (uint32) ((uint32)pdpt_end - (uint32)pdpt_start + 1);
	memptr->mnext = (struct memblk *) NULL;
	memptr->mlength = (uint32) ((uint32)pdpt_end - (uint32)pdpt_start + 1);	
	//kprintf("pdpt_start: %d pdpt_end: %d\n",pdpt_start,pdpt_end);
	//kprintf("virtualmem_init: pt_memptr: %x mnext: %x mlength: %d\n",memptr,memptr->mnext,memptr->mlength);
	
}
