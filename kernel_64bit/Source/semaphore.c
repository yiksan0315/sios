#include "semaphore.h"
#include "slap.h"
#include "Assembly_Func.h"

extern Scheduler g_scheduler;
Semaphore* Semaphore_Pool;
Ready_QUEUE Wait_Queue;

void Init_Semaphore(void)
{
	int i;
	Semaphore_Pool=(Semaphore*)SEMAPHORE_ADDRESS;
	Set_SemaphoreSlab((Semaphore*)Semaphore_Pool,SEMAPHOER_MAXCOUNT);
	for(i=0; i<SEMAPHOER_MAXCOUNT; i++)
	{
		Semaphore_Pool[i].Count=0;
		Semaphore_Pool[i].MaxCount=0;
		Semaphore_Pool[i].IsAlloced=false;
	}
}

Semaphore* Alloc_Semaphore(int Count)
{
	int i;
	Semaphore* SEMAPHORE;
	if(Count<=0||Get_SlapManager()->UseCount==Get_SlapManager()->MaxCount)
	{
		return NULL;
	}
	for(i=0; i<SEMAPHOER_MAXCOUNT; i++)
	{
		if(!Semaphore_Pool[i].IsAlloced)
		{
			SEMAPHORE=&Semaphore_Pool[i];
			SEMAPHORE->MaxCount=Count;
			SEMAPHORE->Count=Count;
			SEMAPHORE->IsAlloced=true;
			Get_SlapManager()->UseCount[RES_SEMAPHORE]--;
			
			return SEMAPHORE;
		}
	}
	return NULL;
}

void Free_Semaphore(Semaphore* _Free_Semaphore)
{
	_Free_Semaphore->Count=0;
	_Free_Semaphore->IsAlloced=false;
	_Free_Semaphore->MaxCount=0;
	Get_SlapManager()->UseCount[RES_SEMAPHORE]++;
}

bool acquire(Semaphore* Lock)
{
	ulld RFLAGS_=Read_Rflags();
	__asm__ __volatile("cli;");
	Lock->Count--;
	if(Lock->Count<0)
	{
		Add_Task_in_Tail(g_scheduler.CURRENT_Process,&Wait_Queue); 
		g_scheduler.CURRENT_Process->PFlag=STATE_WAITING;
		if(RFLAGS_&0x200)
			__asm__ __volatile__("sti;");
		GiveOut_Processor();
		return false;
	}
	if(RFLAGS_&0x200)
		__asm__ __volatile__("sti;");
	return true;
}

void release(Semaphore* Lock)
{
	if(Lock->Count>=Lock->MaxCount)
	{
		return ;
	}
	PCB* PROCESS;
	ulld RFLAGS_=Read_Rflags();
	__asm__ __volatile("cli;");
	Lock->Count++;
	if(Lock->Count<=0)
	{
		PROCESS=Remove_Task_At(0,&Wait_Queue);
		if(PROCESS=NULL)
		{
			if(RFLAGS_&0x200)
			__asm__ __volatile__("sti;");
			return ;
		}
		Add_Task_in_Tail(PROCESS,&((Return_READY_QUEUE())[PROCESS->Priority]));
		PROCESS->PFlag=STATE_READY;
	}
	if(RFLAGS_&0x200)
		__asm__ __volatile__("sti;");
}

void RemoveFrom_Wait_Queue(PCB* _PROCESS)
{
	int i;
	ulld RFLAGS_=Read_Rflags();
	__asm__ __volatile__("cli;");
	PCB* Temp=Wait_Queue.p_Head;
	for(i=0; i<Wait_Queue.Task_Count; i++)
	{
		if(Temp->PID==_PROCESS->PID)
		{
			Remove_Task_At(i,&Wait_Queue);
			break;
		}
		Temp=Temp->p_Next;
	}
	if(RFLAGS_&0x200)
		__asm __volatile__("sti;");
}

