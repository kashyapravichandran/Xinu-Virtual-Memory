/* kill.c - kill */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  kill  -  Kill a process and remove it from the system
 *------------------------------------------------------------------------
 */
syscall	kill(
	  pid32		pid		/* ID of process to kill	*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/
	int32	i;			/* Index into descriptors	*/
	pd_t* PD_Base = (pd_t *)proctab[pid].prpdbr;
	pt_t* PT_Base; 
	uint32 dir_offset = 8;
	uint32 table_offset = 0;

	mask = disable();
	
	if (isbadpid(pid) || (pid == NULLPROC)
	    || ((prptr = &proctab[pid])->prstate) == PR_FREE) {
		//write_cr3((unsigned long)proctab[currpid].prpdbr);
		restore(mask);
		return SYSERR;
	}

	if (--prcount <= 1) {		/* Last user process completes	*/
		xdone();
	}

	send(prptr->prparent, pid);
	for (i=0; i<3; i++) {
		close(prptr->prdesc[i]);
	}
	freestk(prptr->prstkbase, prptr->prstklen);
	// Freeing Virtual Memory
	// GO through all the directories and go through all the page table and send the memory to be deleted  using the function freevirtualmem(char* ptr, memlist)
	
	if(proctab[pid].user_process == TRUE)
	{
	   //kprintf("Kill %d Base %x: \n",pid,PD_Base); 
	   //kprintf("DIR OFFSET :%d \n",dir_offset);
	   //kprintf("PT_BASE :%x \n",PD_Base[0].pd_base);
	   //debug_bits();	
	   write_cr3((unsigned long)PD_Base_Kernel); // Kernel mode
	   //kprintf("PT_BASE :%x \n",PD_Base[0].pd_base);
	   for(dir_offset=0;dir_offset<1024;dir_offset++)
	   {
		
		//kprintf("Kills %d Table Base %x\n",pid,PT_Base);
		if(PD_Base[dir_offset].pd_avail==1 && PD_Base[dir_offset].pd_pres==1)
		{
			for(table_offset=0;table_offset<1024;table_offset++)
			{
							
				PT_Base = (pt_t*)(PD_Base[dir_offset].pd_base<<12);
				///kprintf("--------------------------\n");
				//kprintf("Dir_offset %x\n",dir_offset<<22|table_offset<<12);
				//kprintf("Avail %d and Pres %d \n ",PT_Base[table_offset].pt_avail,PT_Base[table_offset].pt_pres);				
				if(PT_Base[table_offset].pt_avail==2 && PT_Base[table_offset].pt_pres==1)// freeing memory
				{
					
					//kprintf("Freeing up the FFS Area: %x \n",PT_Base[table_offset].pt_base<<12);
					freevirtualmem((char*)(PT_Base[table_offset].pt_base<<12),(&ffs_memlist));
				}
				PT_Base[table_offset].pt_avail=0;
				PT_Base[table_offset].pt_pres=0;PT_Base[table_offset].pt_write=0;
			}
			freevirtualmem((char*)(PD_Base[dir_offset].pd_base<<12),(&pt_memlist));
		}
		PD_Base[dir_offset].pd_avail=0;
		PD_Base[dir_offset].pd_pres=0;PD_Base[dir_offset].pd_write=0;
	   }
	   //kprintf("Kills %d Dir Base %x\n",pid,PD_Base);
	   freevirtualmem((char*)PD_Base,(&pt_memlist));
	//write_cr3((unsigned long)proctab[currpid].prpdbr);	
	}	
	switch (prptr->prstate) {
	case PR_CURR:
		prptr->prstate = PR_FREE;	/* Suicide */
		resched();

	case PR_SLEEP:
	case PR_RECTIM:
		unsleep(pid);
		prptr->prstate = PR_FREE;
		break;

	case PR_WAIT:
		semtab[prptr->prsem].scount++;
		/* Fall through */

	case PR_READY:
		getitem(pid);		/* Remove from queue */
		/* Fall through */

	default:
		prptr->prstate = PR_FREE;
	}
	//if(currpid!=pid)
		
	restore(mask);
	return OK;
}
