// Page Fault Handler 
#include<xinu.h>

uint32 count1 = 0;
uint32 count1_value = 10;
void pagefault_handler()
{
	pd_t* PD_Base;
	uint32 dir_offset;
	uint32 table_offset;
	uint32 off_address;
	pt_t* PT_Base;
	char* ptr;	
	write_cr3((unsigned int)PD_Base_Kernel);
	PD_Base = (pd_t*)(proctab[currpid].prpdbr);
	off_address = (uint32)read_cr2();
	dir_offset = page_base_offset(off_address);
	table_offset = page_table_offset(off_address);
	PT_Base = (pt_t*)(PD_Base[dir_offset].pd_base<<12);
	//kprintf("\nPID %d , Accesing Memory : %x\n",currpid, off_address);
	//kprintf("Table Data Pres: %d Avail %d Directory Data Pres: %d Avail %d\n",PT_Base[table_offset].pt_pres,PT_Base[table_offset].pt_avail,PD_Base[dir_offset].pd_pres,PD_Base[dir_offset].pd_avail);
	if(PT_Base[table_offset].pt_avail==2&&PD_Base[dir_offset].pd_pres==1)
	{
		// Get page 
		ptr=getvirtualmem(4096,&ffs_memlist);
		//debug_bits();
		proctab[currpid].used_ffs_frames++;
		PT_Base[table_offset].pt_base = physical_frame(ptr);
		PT_Base[table_offset].pt_pres = 1;
		write_cr3((unsigned int)(proctab[currpid].prpdbr));
	}
	else
	{
		kprintf("P%d :: SEGMENTATION_FAULT\n",currpid);
		//write_cr3((unsigned long)PD_Base);
		kill(currpid);
	}
}

