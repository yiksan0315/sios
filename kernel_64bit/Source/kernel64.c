#include "interrupt.h"
#include "IO.h"
#include "Set_HardWare.h"
#include "Scheduler.h"
#include "Memory.h"
#include "DiskDriver.h"
#include "FileSystem.h"

void main(void)
{
	printk("Switch to IA-32e Mode Succellfully!",16,0,_BLUE);
	printk("Initializing... Please Wait...",18,0,_WHITE);
	SetTables_GDT_IDT_TSS();
	Init_Scheduler();
	Init_KernelMemory();
	Set_PIT(PIT_ONESECOND/1000,true);
	if(EnableKeyBoard())
	{
		On_Off_LED(true,false,false);
	}
	else
	{
		printk("Error:Couldn't Enable KeyBoard...",19,0,_RED);
	}
	Init_PIC();
	Set_Mask_IRQ(0);
	__asm__ __volatile__("sti");
	if(!InitDisk())
	{
		printk("Error:Error is occued while Initializing Disk!",19,0,_RED);
		return ;
	}
	if(!Format())//포맷하고 나면 느려짐  ->뒤족에 있는 메모리를 할당받게 됨  
	{
		printk("Can't Format Harddisk... Harddisk is at least 128MB",19,0,_RED);
		return ;
	}
	printk("Complete Initializing Successfully!",19,0,_WHITE);
	sched();
	return ;
}


