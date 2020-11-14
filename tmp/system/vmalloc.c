#include<xinu.h>

char* vmalloc(uint32 nbytes)
{
	uint32 dir_offset = 8;
	uint32 table_offset = 0;
	pd_t* PD_Base = proctab[currpid].prpdbr;
	pt_t* PT_Base;
	uint32 return_address;
	uint32 num_pages = ((uint32)roundpage(nbytes))/4096;	
	int mask;
	int i,j; uint32 count;
	mask = disable();
	write_cr3((unsigned long)PD_Base_Kernel);
	//i=0;
	//kprintf("Here %d",return_address);
	return_address = dir_offset<<22|table_offset<<12;
	//kprintf("Here %d \n",return_address);
	//kprintf(" Num Pages %d \n",num_pages); 
	// Finding the continuous space in the page table and pafe directory
	count=0;
	
	if(num_pages>=(1024*1024)-proctab[currpid].alloc_vir_pages-1)
		return (char*)SYSERR;
		
	for(;dir_offset<1024;dir_offset++)
	{
		if(PD_Base[dir_offset].pd_pres==0)
		{
			PD_Base[dir_offset].pd_pres = 1;
			PD_Base[dir_offset].pd_avail = 1;
			PD_Base[dir_offset].pd_write = 1;
			PT_Base = (pt_t*)getvirtualmem(4096,&pt_memlist);
			PD_Base[dir_offset].pd_base = (unsigned int)physical_frame(PT_Base);
			for(i=0;i<1024;i++)
			{
				PT_Base[i].pt_pres = 0;
				PT_Base[i].pt_avail = 0;
				PT_Base[i].pt_write = 1;
			}
		}
		else
		{
			PT_Base = (pt_t*)(PD_Base[dir_offset].pd_base<<12);
		}
		
		for(table_offset=0;table_offset<1024;table_offset++)
		{
			
			//kprintf("PT_Base : %x table_offset: %d pres %d count %d\n", (uint32)PT_Base, table_offset,PT_Base[table_offset].pt_pres,count);			
			if(PT_Base[table_offset].pt_avail!=0)
			{
				count = 0;
				if(table_offset==1023)
					return_address = ((dir_offset+1)<<22)|((0)<<12);
				else
					return_address = (dir_offset<<22)|((table_offset+1)<<12);
			}
			else
			{
				count++;
			}	
			if(num_pages == count)
				break;
		}
	
		if(num_pages==count)
			break;
	}

	//kprintf("Num Pages: %d Count: %d \n",num_pages, count);
	if(num_pages!=count)
	{
		kprintf("Currpid %d\n",currpid); 
		return (char*)SYSERR;
	}

	//kprintf("count %d\n",count);
	for(j=return_address;(j-return_address)<nbytes;j=j+4096)
	{
				
		table_offset=page_table_offset(j);
		dir_offset = page_base_offset(j);
		PT_Base = (pt_t*)(PD_Base[dir_offset].pd_base<<12);
		PT_Base[table_offset].pt_avail = 2;
		PT_Base[table_offset].pt_pres = 0;
		PT_Base[table_offset].pt_write = 1;
		proctab[currpid].alloc_vir_pages++;
		//kprintf("-----------------------------\n");
		//kprintf("Dir_offset %x\n",dir_offset<<22|table_offset<<12);
		//kprintf("Avail %d and Pres %d \n ",PT_Base[table_offset].pt_avail,PT_Base[table_offset].pt_pres);
		//while((j-return_address)/4096==10);
	}


	write_cr3((unsigned long)proctab[currpid].prpdbr);
	//kprintf("\n Malloc PDBR: %x",proctab[currpid].prpdbr);
	restore(mask);	
	//kprintf("\n dir offset : %x  Table_Offset %x", dir_offset, table_offset);
	//debug_bits();
	return (char *)return_address;
}
