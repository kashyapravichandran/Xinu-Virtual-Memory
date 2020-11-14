/* getmem.c - getmem */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  getmem  -  Allocate heap storage, returning lowest word address
 *------------------------------------------------------------------------
 */
char  	*getmem(
	  uint32	nbytes		/* Size of memory requested	*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	memblk	*prev, *curr, *leftover;

	mask = disable();
	if (nbytes == 0) {
		restore(mask);
		return (char *)SYSERR;
	}

	nbytes = (uint32) roundmb(nbytes);	/* Use memblk multiples	*/

	prev = &memlist;
	curr = memlist.mnext;
	while (curr != NULL) {			/* Search free list	*/

		if (curr->mlength == nbytes) {	/* Block is exact match	*/
			prev->mnext = curr->mnext;
			memlist.mlength -= nbytes;
			restore(mask);
			return (char *)(curr);

		} else if (curr->mlength > nbytes) { /* Split big block	*/
			leftover = (struct memblk *)((uint32) curr +
					nbytes);
			prev->mnext = leftover; 
			leftover->mnext = curr->mnext;
			leftover->mlength = curr->mlength - nbytes;
			memlist.mlength -= nbytes;
			restore(mask);
			return (char *)(curr);
		} else {			/* Move to next block	*/
			prev = curr;
			curr = curr->mnext;
		}
	}
	restore(mask);
	return (char *)SYSERR;
}

// Things added for virtual memory

char* getvirtualmem( uint32 nbytes, struct  memblk  *v_memlist)
{
	int mask;
	struct memblk *prev, *curr, *leftover;

	mask = disable();

	if( nbytes == 0)
	{
		restore(mask);
		return (char *)SYSERR;
	}
	
	nbytes = (uint32) roundpage(nbytes);	/* Use memblk multiples	*/
	//kprintf(" Nbytes : %d\n", nbytes);
	prev = v_memlist;
	curr = v_memlist->mnext;
	while (curr != NULL) {			/* Search free list	*/

		if (curr->mlength == nbytes) {	/* Block is exact match	*/
			prev->mnext = curr->mnext;
			v_memlist->mlength -= nbytes;
			//kprintf("getmem %d Address: %x\n",currpid,(uint32)curr);
			restore(mask);
			return (char *)(curr);

		} else if (curr->mlength > nbytes) { /* Split big block	*/
			leftover = (struct memblk *)((uint32) curr + nbytes);
			//kprintf("leftover Address: %x\n", (uint32)leftover);
			prev->mnext = leftover;
			//kprintf("prev->mnext %x\n", (uint32)prev->mnext);
			leftover->mnext = curr->mnext;
			leftover->mlength = curr->mlength - nbytes;
			v_memlist->mlength -= nbytes;
			//kprintf("Curr Length %d and Leftover Length  %d Address: %x\n",curr->mlength,leftover->mlength,(uint32)curr);
			restore(mask);
			return (char *)(curr);
		} else {			/* Move to next block	*/
			prev = curr;
			curr = curr->mnext;
		}
	}
	restore(mask);
	return (char *)SYSERR;	

}
