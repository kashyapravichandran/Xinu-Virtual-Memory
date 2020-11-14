#include<xinu.h>

// Things that are added for allocating page directory and page tables.

syscall map_xinu_area(pd_t* PDBR,uint32 num_pages,pid32 pid)
{
	// Things that we need to do here is to allocate a page table every 1024 pages, and 
	uint32 Base_Address = 0x00; 
	uint32 Address;
	uint32 prev_dir_index = 1;
	uint32 i=0;
	pt_t* PT_Base;
	uint32 dir_index, table_index;
	uint32 PFN;

	for( i=0; i<num_pages; i++)
	{
		Address = (Base_Address + (i*PAGE_SIZE));
		dir_index = page_base_offset(Address);
		table_index = page_table_offset(Address);
		PFN = physical_frame(Address);
		if(prev_dir_index != dir_index)
		{
			// Allocate the pages for page table
			// Allocate the entry in the page directory
			PT_Base = (pt_t*)getvirtualmem(4096,&pt_memlist);
			PDBR[dir_index].pd_base = (unsigned int)physical_frame(PT_Base);
			PDBR[dir_index].pd_pres = 1;				// Present
			PDBR[dir_index].pd_avail = 1;				// Valid
			PDBR[dir_index].pd_write = 1;				// Writable

		}
		
		proctab[pid].alloc_vir_pages++;
		PT_Base[table_index].pt_base = PFN;
		PT_Base[table_index].pt_pres = 1; 				// Present 
		PT_Base[table_index].pt_avail = 1;				// Valid 
		PT_Base[table_index].pt_write = 1;				// Writable

		prev_dir_index = dir_index;
/*
		kprintf(" --------------- i = %d ----------------------- \n",i); 
		kprintf(" Directory Address : %x Table Address : %x\n",(uint32)PDBR, (uint32)PT_Base);
		kprintf(" Address : %x PFN %x \n", Address, PFN);		
		kprintf(" Dir Offset %d, Table Offset :%d \n",dir_index, table_index); 
		kprintf(" PD Offset : %d PT Offset: %d\n", PDBR[dir_index].pd_base, PT_Base[table_index].pt_base);
*/
	}
	//kprintf("text: %x Address: %x\n",text,Address);
	return OK;

}
