#include "IO.h"
#include "Scheduler.h"
#include "Set_HardWare.h"
#include "slap.h"
#include "DiskDriver.h"

extern Scheduler g_scheduler;

void Exception_Routine(uint Routine_line)
{
	char Buf[3]={0,0,'\0'};
	Buf[1]=Routine_line%10+'0';
	Buf[0]=Routine_line/10+'0';
	printk("Exception Occur:",0,0,_RED);
	printk(Buf,0,16,_BLUE);
	while(true){__asm__ __volatile__("hlt");}
}

void Intterupt_Routine(unsigned int ISR_line)
{
	printk("Intterupt Occur!!",1,10,_BLUE);
	Send_EOI_PIC(ISR_line);
}

void ISR_Timer(unsigned int ISR_line)
{
	Send_EOI_PIC(ISR_line);
	DecreaseProcessorTime();
	if(IsTimeOut())
		Preempted_ByTimeOut();
}

void ISR_Keyboard(unsigned int ISR_line)
{
	unsigned long long int RFLAGS_;
	unsigned char Key_Buf;
	if(IsBuf_full(Out_Buf))
	{	
		if(Scan_Convert_ASCII(ReadPort_In(0x60),&Key_Buf))
		{
			__asm__ __volatile__("cli");
			RFLAGS_=Read_Rflags();
			PutKeyBuf(Key_Buf);
			if(RFLAGS_&0x200)
				__asm__ __volatile("sti;");
		}		
	}
	Send_EOI_PIC(ISR_line);
}

void ISR_FPU_ContextSwitching(unsigned char ISR_line)
{
	clts();
	if(g_scheduler.CURRENT_Process->PID==g_scheduler.LastUsed_PID)
	{
		return ;
	}
	else if(g_scheduler.LastUsed_PID!=-1)
	{
		SaveFPU(((PCB*)Get_SlapManager()->Slab[RES_TCB])[g_scheduler.LastUsed_PID].FPUContext);
	}
	
	if(!(g_scheduler.CURRENT_Process->Use_FPU))
	{
		__asm__ __volatile__("finit");
		g_scheduler.CURRENT_Process->Use_FPU=true;
	}
	else
	{
		LoadFPU(g_scheduler.CURRENT_Process->FPUContext);
	}
	g_scheduler.LastUsed_PID=g_scheduler.CURRENT_Process->PID;
}

void ISR_HardDisk(unsigned int ISR_line)
{
	*Flag_Interuppted()=true;
	Send_EOI_PIC(ISR_line);
}
