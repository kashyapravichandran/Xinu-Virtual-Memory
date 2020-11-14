#include<xinu.h>

uint32 free_ffs_pages(){
	

/*
	kprintf("In free_ffs_pages\n");
	curr = ffs_memlist.mnext;
	
	while (curr != NULL) {			
		kprintf("curr : %d free_pages:\n", (uint32)curr);
		free_pages = free_pages + curr->mlength;
		curr = curr->mnext;
	}
	free_pages = free_pages >> 12;
*/
	return ffs_memlist.mlength>>12;		
}

uint32 allocated_virtual_pages(pid32 pid){

	return proctab[pid].alloc_vir_pages;

}

uint32 used_ffs_frames(pid32 pid){
	
	return proctab[pid].used_ffs_frames;
}

void debug_bits()
{

	int mask;
	pt_t* PT_Base;
	pd_t* PD_Base = (pd_t*)proctab[currpid].prpdbr;
	uint32 dir_offset, table_offset;
	mask = disable();
	write_cr3(PD_Base_Kernel);
	kprintf("We are in debug function \n");
	for(dir_offset=0;dir_offset<1024;dir_offset++)
	{
		if(PD_Base[dir_offset].pd_avail==1)
		{
			PT_Base = (pt_t*) (PD_Base[dir_offset].pd_base<<12);
			for(table_offset=0;table_offset<1024;table_offset++)
			{
				if(PT_Base[table_offset].pt_avail==0)
				{
					kprintf("Address: %x \n",PT_Base[table_offset].pt_base<<12);
				}
			}
		}
	}
	write_cr3(proctab[currpid].prpdbr);
	restore(mask);
	
}
