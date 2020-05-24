#include "interrupt.h"
#include "Assembly_Func.h"
#include "IO.h"
#include "Shell.h"
#include "slap.h"
#include "Memory.h"
#include "DiskDriver.h"

/**********problem***********/
//자식들을 넣는 큐에서 계속 p_Next 가 0번지를 가리킴
//그랬다 안그랬다 그럼  
/**************************/ 

Scheduler g_scheduler;
Ready_QUEUE ReadyQueue[PRIORITY_TYPES];
Ready_QUEUE TerminateQueue;

PCB* g_TaskPool;

void sched(void) 
{
	sched_CreateStartProcess();
	int i; ulld RFLAGS_; PCB* TerminateProcess,*Temp;
	while(true)
	{
		if(TerminateQueue.Task_Count>0)
		{
			RFLAGS_=Read_Rflags();
			__asm__ __volatile__("cli;");
			while(true)
			{
				TerminateProcess=Remove_Task_At(0,&TerminateQueue);
				if(TerminateProcess==NULL)
					break;
				if(!TerminateProcess->Is_Thread) 
				{
					if(g_TaskPool[TerminateProcess->ParentPID].PCB_Is_Alloced)
					{
						Temp=g_TaskPool[TerminateProcess->ParentPID].Childs.p_Head;
						for(i=0; i<g_TaskPool[TerminateProcess->ParentPID].Childs.Task_Count; i++)
						{
							if(Temp->PID==TerminateProcess->PID)
							{
								Remove_Child_At(i,&g_TaskPool[TerminateProcess->ParentPID]);
								break;
							}
							Temp=Temp->child_Next;
						}
					}
					Temp=TerminateProcess->Childs.p_Head;
					for(i=0; i<TerminateProcess->Childs.Task_Count; i++)
					{
						Terminate_Process(Temp->PID); // 자식 프로세스들 모두 terminateQueue 에 넣음  
						Temp=Temp->child_Next;
					}
				}
				else if(TerminateProcess->Is_Thread)
				{
					Temp=g_TaskPool[TerminateProcess->ParentPID].Childs.p_Head;
					for(i=0; i<g_TaskPool[TerminateProcess->ParentPID].Childs.Task_Count; i++)
					{
						if(Temp->PID==TerminateProcess->PID)
						{  
							Remove_Child_At(i,&(g_TaskPool[TerminateProcess->ParentPID]));
							break;
						}
						Temp=Temp->child_Next;
					}
				}
				Free_PCB(TerminateProcess);
				Get_SlapManager()->UseCount[RES_TCB]--;
			}
			if(RFLAGS_&0x200)
				__asm__ __volatile__("sti;");
		}
		GiveOut_Processor();
	}
}

void sched_CreateStartProcess(void)
{
	Create_Process(PRIORITY_VERYHIGH,(ulld)Shell_Start,(void *)0x2000000,0x100000,false);
}

PCB* Create_Process(ulld pPriority,ulld pEntryAddr,void *pMemoryAddress,ulld MemorySize,bool isThread)
{ 
	if(Get_SlapManager()->MaxCount[RES_TCB]==Get_SlapManager()->UseCount[RES_TCB])
	{
		return NULL;
	}
	int i; ulld RFLAGS_;
	PCB* p_Process,*p_Parent;
	
	RFLAGS_=Read_Rflags();
	__asm__ __volatile__("cli;");
	for(i=0; i<TASKPOOL_MAXCOUNT; i++)
	{
		if(!g_TaskPool[i].PCB_Is_Alloced)
		{
			p_Process=&g_TaskPool[i];
			break;
		}
	}
	p_Parent=g_scheduler.CURRENT_Process;
	if(p_Parent->Is_Thread&&isThread)
	{
		if(RFLAGS_&0x200)
		__asm__ __volatile("sti;");
		return NULL;
	}
	Get_SlapManager()->UseCount[RES_TCB]++;
	Set_Attr_Process(p_Process,STATE_NEW,i,pEntryAddr,(void *)STACK_ADDRESS+i*STACK_SIZE,STACK_SIZE);
	p_Process->Priority=pPriority;
	Ready_QUEUE* Queue=&ReadyQueue[pPriority];
	Add_Task_in_Tail(p_Process,Queue);
	Add_Child_in_Tail(p_Process,p_Parent); //오류 나는 이유: Ready_Queue에서 이미 자신의 포인터에 갖다 넣어 놓고  
												   //p_Next랑 p_Prev를 이미 다른걸 가리키게 해놨는데 그걸 childs 큐에서 
												   //p_Next랑 p_Prev, 즉 같은 포인터가 참조하는 주소를 바꿔서 이럼. 
												   //+ context switching 하는 과정에서 준비 완료 큐에서 넣고 빼고 하면서 
												   //해당 TCB의 p_Next와 p_Prev를 수정함  
	p_Process->PFlag=STATE_READY;

	if(isThread)
	{
		p_Process->MemoryAddress=p_Parent->MemoryAddress;
		p_Process->MemorySize=p_Parent->MemorySize;
	}
	else
	{
		p_Process->MemoryAddress=pMemoryAddress;
		p_Process->MemorySize=MemorySize;
	}
	p_Process->Is_Thread=isThread;
	if(RFLAGS_&0x200)
		__asm__ __volatile("sti;");
	return p_Process;
}

bool Terminate_Process(ulld pPID)
{
	int i;
	ulld RFLAGS_; 
	PCB* PROCESS;
	if(pPID>=TASKPOOL_MAXCOUNT||pPID<0)
	{
		return false;
	}
	if(g_scheduler.CURRENT_Process->PID==pPID)
	{
		g_scheduler.CURRENT_Process->PFlag=STATE_TERMINATED;
		GiveOut_Resource();
		GiveOut_Processor();
	}
	else
	{
		PROCESS=&g_TaskPool[pPID];
		if(!PROCESS->PCB_Is_Alloced)
		{
			return false;
		}
		RFLAGS_=Read_Rflags();
		__asm__ __volatile__("cli;");
		GiveOut_Resource();
		if(PROCESS->PFlag==STATE_WAITING)
		{
			RemoveFrom_Wait_Queue(PROCESS);
		}	
		else
		{
			RemoveFrom_ReadyQueue(PROCESS); //여기서 문제가 발생함  
		}
		PROCESS->PFlag=STATE_TERMINATED;
		Add_Task_in_Tail(PROCESS,&TerminateQueue);
		if(RFLAGS_&0x200)
			__asm__ __volatile__("sti;");
	}
	return true;
}

void exit(void)
{
	Terminate_Process(g_scheduler.CURRENT_Process->PID);
}

void GiveOut_Resource(void)
{
//	__asm__ __volatile__("fint;");
//	__asm__ __volatile__("finit;");
/*	LoadFPU(g_TaskPool[2].FPUContext);
		printk("FPU!",10,0,_BLUE);*/
//	int b;
//	double a;
//	a*5;
//	a=5;
	//SaveFPU(g_TaskPool[2].FPUContext);
}

bool ChangePriority(ulld pPID,int pPriority)
{
	ulld RFLAGS_;
	PCB* PROCESS=&g_TaskPool[pPID];
	if(!PROCESS->PCB_Is_Alloced)
	{
		return false;
	}
	if(PROCESS->PFlag==STATE_READY)
	{
	RFLAGS_=Read_Rflags();
	__asm__ __volatile__("cli;");
		RemoveFrom_ReadyQueue(PROCESS);
		PROCESS->Priority=pPriority;
		Add_Task_in_Tail(PROCESS,&ReadyQueue[PROCESS->Priority]);
	if(RFLAGS_&0x200)
	__asm__ __volatile__("sti;");
		return true;
	}
	PROCESS->Priority=pPriority;
	return true;
}

void Set_Attr_Process(PCB* p_PCB,ulld pFlag,ulld pPID,ulld pEntryAddr,void *pStackAddr,ulld pStackSize)
{
	int i;
	for(i=0; i<REGISTER_COUNT; i++)
	{
		p_PCB->REGISTER[i]=0;
	}
	
	p_PCB->REGISTER[RSP]=(ulld)pStackAddr+pStackSize-8;
	p_PCB->REGISTER[RBP]=(ulld)pStackAddr+pStackSize-8;
	
	*(ulld *)((ulld)pStackAddr+pStackSize-8)=(ulld)exit;
	
	p_PCB->REGISTER[CS]=IA_32e_CodeDescriptor;
	p_PCB->REGISTER[DS]=IA_32e_DataDescriptor;
	p_PCB->REGISTER[ES]=IA_32e_DataDescriptor;
	p_PCB->REGISTER[FS]=IA_32e_DataDescriptor;
	p_PCB->REGISTER[GS]=IA_32e_DataDescriptor;
	p_PCB->REGISTER[SS]=IA_32e_DataDescriptor;
	
	p_PCB->REGISTER[RIP]=pEntryAddr;
	p_PCB->REGISTER[RFLAGS]|=0x0200;
	
	p_PCB->PID=pPID;
	p_PCB->ParentPID=g_scheduler.CURRENT_Process->PID;
	p_PCB->StackAddress=pStackAddr;
	p_PCB->StackSize=pStackSize;
	p_PCB->PFlag=pFlag;
	
	p_PCB->PCB_Is_Alloced=true;
	
	p_PCB->Priority=PRIORITY_MIDDLE;
}

void Free_PCB(PCB* Task_PCB)
{
	int i;
	Task_PCB->PCB_Is_Alloced=false;
	Task_PCB->PFlag=-1;
	Task_PCB->PID=-1;
	Task_PCB->ParentPID=-1;
	Task_PCB->p_Next=NULL;
	Task_PCB->p_Prev=NULL;
	Task_PCB->Use_FPU=false;
	
	Task_PCB->StackAddress=NULL;
	Task_PCB->StackSize=0;
	Task_PCB->MemoryAddress=NULL;
	Task_PCB->MemorySize=0;

	Task_PCB->Priority=0;
	
	Task_PCB->Childs.p_Head=NULL;
	Task_PCB->Childs.p_Tail=NULL;
	Task_PCB->Childs.Task_Count=0;
	
	Task_PCB->child_Next=NULL;
	Task_PCB->child_Prev=NULL;
	for(i=0; i<REGISTER_COUNT; i++)
	{
		Task_PCB->REGISTER[i]=0;
	}
}

void GiveOut_Processor(void)
{
	bool RFLAGS_;
	PCB* RunningTask=g_scheduler.CURRENT_Process;
	PCB* ScheduledTask;
	if(!Is_TaskInQueue())
	{
		return ;
	}
	RFLAGS_=Read_Rflags();
	__asm__ __volatile__("cli;");
	ScheduledTask=Schedule_Process();
	
	ScheduledTask->PFlag=STATE_RUNNING;
	g_scheduler.Process_Queue=&ReadyQueue[ScheduledTask->Priority];	
	g_scheduler.CURRENT_Process=ScheduledTask;
	if(ScheduledTask->PID!=g_scheduler.LastUsed_PID)
		stts();
	else
		clts();
	if(RunningTask->PFlag==STATE_TERMINATED)
	{
		Add_Task_in_Tail(RunningTask,&TerminateQueue);
		SwitchContext(NULL,ScheduledTask->REGISTER);
	}
	else if(RunningTask->PFlag==STATE_WAITING)
	{
		SwitchContext(RunningTask->REGISTER,ScheduledTask->REGISTER);
	}
	else
	{
		Add_Task_in_Tail(RunningTask,&ReadyQueue[RunningTask->Priority]);
		RunningTask->PFlag=STATE_READY;
		SwitchContext(RunningTask->REGISTER,ScheduledTask->REGISTER);
	}
	g_scheduler.Time_=PROCESSOR_TIME-RunningTask->Priority;
	if(RFLAGS_&0x200)
		__asm__ __volatile__("sti;");
}

bool Is_TaskInQueue(void)
{
	PCB* Temp;
	int i;
	for(i=0; i<PRIORITY_TYPES; i++)
	{
		Temp=Get_Task_At(0,&ReadyQueue[i]);
		if(Temp!=NULL)
			return true;
	}
	return false;
}

bool Preempted_ByTimeOut(void)
{
	int i;
	PCB* RunningTask=g_scheduler.CURRENT_Process;
	PCB* ScheduledTask=Schedule_Process();
	if(ScheduledTask==NULL)
	{
		return false;
	}
	char *Ist_Context=(char *)IST_START+IST_SIZE-sizeof(ulld)*REGISTER_COUNT;
	if(RunningTask->PFlag==STATE_TERMINATED)
	{
		Add_Task_in_Tail(RunningTask,&TerminateQueue);
	}
	else if(RunningTask->PFlag==STATE_WAITING)
	{
		for(i=0; i<sizeof(ulld)*REGISTER_COUNT; i++)
		{
			((char *)RunningTask->REGISTER)[i]=Ist_Context[i];
		}
	}
	else
	{
		Add_Task_in_Tail(RunningTask,g_scheduler.Process_Queue);
		RunningTask->PFlag=STATE_READY;
		for(i=0; i<sizeof(ulld)*REGISTER_COUNT; i++)
		{
			((char *)RunningTask->REGISTER)[i]=Ist_Context[i];
		}
	}
	ScheduledTask->PFlag=STATE_RUNNING;
	g_scheduler.CURRENT_Process=ScheduledTask;
	g_scheduler.Process_Queue=&ReadyQueue[ScheduledTask->Priority];
	for(i=0; i<sizeof(ulld)*REGISTER_COUNT; i++)
	{
		Ist_Context[i]=((char *)ScheduledTask->REGISTER)[i];
	}
	g_scheduler.Time_=PROCESSOR_TIME-g_scheduler.CURRENT_Process->Priority;
	return true;
}  

PCB* Schedule_Process(void)
{
	PCB* Temp;
	int i,Priority=g_scheduler.Schedule_Priority;
	for(i=0; i<PRIORITY_TYPES; i++)
	{
		Temp=Remove_Task_At(0,&ReadyQueue[Priority]);
		if(Temp!=NULL)
		{
			g_scheduler.Schedule_Priority=(g_scheduler.Schedule_Priority+1)%PRIORITY_TYPES;
			return Temp;
		}
		if(--Priority<0)
			Priority=PRIORITY_VERYLOW;
	}
	g_scheduler.Schedule_Priority=(g_scheduler.Schedule_Priority+1)%PRIORITY_TYPES;
	return NULL;
}

void RemoveFrom_ReadyQueue(PCB* _PROCESS)
{
	int i;
	ulld RFLAGS_=Read_Rflags();
	__asm__ __volatile__("cli;");
	PCB* Temp=ReadyQueue[_PROCESS->Priority].p_Head;
	for(i=0; i<ReadyQueue[_PROCESS->Priority].Task_Count; i++)
	{
		if(Temp->PID==_PROCESS->PID)
		{
			Remove_Task_At(i,&ReadyQueue[_PROCESS->Priority]);
			break;
		}
		Temp=Temp->p_Next;
	}
	if(RFLAGS_&0x200)
		__asm __volatile__("sti;");
}

void DecreaseProcessorTime(void)
{
	if(g_scheduler.Time_>0)
	{
		g_scheduler.Time_--;
	}
}

bool IsTimeOut(void)
{
	if(g_scheduler.Time_<=0)
	{
		return true;
	}
	return false;
}

void Init_Scheduler(void)
{
	Init_TCBPool();
	Init_Ready_QUEUE();
	Init_Semaphore();
	
	g_scheduler.CURRENT_Process=&g_TaskPool[PID_SCHED];
	g_scheduler.CURRENT_Process->PID=PID_SCHED; //Sched's PID=0
	g_scheduler.Process_Queue=&ReadyQueue[PRIORITY_VERYHIGH];
	g_scheduler.Schedule_Priority=PRIORITY_VERYHIGH;
	g_scheduler.LastUsed_PID=-1;
	
	g_TaskPool[PID_SCHED].PCB_Is_Alloced=true;
	g_TaskPool[PID_SCHED].Priority=PRIORITY_VERYHIGH;
	g_TaskPool[PID_SCHED].PFlag=STATE_NEW;
	g_TaskPool[PID_SCHED].MemoryAddress=(void*)0x100000;
	g_TaskPool[PID_SCHED].MemorySize=0x500000;
	g_TaskPool[PID_SCHED].StackAddress=(void*)0x600000;
	g_TaskPool[PID_SCHED].StackSize=0x100000;
	Get_SlapManager()->UseCount[RES_TCB]++;
}

void Init_TCBPool(void)
{
	int i;
	g_TaskPool=(PCB *)TASKPOOL_ADDRESS;
	Set_PCBSlab((PCB *)g_TaskPool,TASKPOOL_MAXCOUNT);
	for(i=0; i<TASKPOOL_MAXCOUNT; i++)
	{
		g_TaskPool[i].PCB_Is_Alloced=false;
		g_TaskPool[i].PID=0;
		g_TaskPool[i].p_Prev=NULL;
		g_TaskPool[i].p_Next=NULL;
		g_TaskPool[i].ParentPID=0;
		g_TaskPool[i].Is_Thread=false;
		g_TaskPool[i].Use_FPU=false;
		
		g_TaskPool[i].Childs.p_Head=NULL;
		g_TaskPool[i].Childs.p_Tail=NULL;
		g_TaskPool[i].Childs.Task_Count=0;
		
		g_TaskPool[i].child_Next=NULL;
		g_TaskPool[i].child_Prev=NULL;
	}
}

void Init_Ready_QUEUE(void)
{
	int i;
	for(i=0; i<PRIORITY_TYPES; i++)
	{
		ReadyQueue[i].p_Head=NULL;
		ReadyQueue[i].p_Tail=NULL;
		ReadyQueue[i].Task_Count=0;
	}
}

bool Add_Task_in_Head(PCB* Task_PCB,Ready_QUEUE* queue)
{
	if(Get_SlapManager()->MaxCount[RES_TCB]==Get_SlapManager()->UseCount[RES_TCB])
	{
		return false;
	}
	if(!(queue->Task_Count++))
	{
		queue->p_Head=(void *)Task_PCB;
		queue->p_Tail=(void *)Task_PCB;
		return true;
	}
	Task_PCB->p_Prev=NULL;
	Task_PCB->p_Next=(PCB *)queue->p_Head;
	((PCB *)queue->p_Head)->p_Prev=Task_PCB;
	queue->p_Head=(void *)Task_PCB;
	return true;
}

bool Add_Task_in_Tail(PCB* Task_PCB,Ready_QUEUE* queue)
{
	if(Get_SlapManager()->MaxCount[RES_TCB]==Get_SlapManager()->UseCount[RES_TCB])
	{
		return false;
	}
	if(!(queue->Task_Count++))
	{
		queue->p_Head=(void *)Task_PCB;
		queue->p_Tail=(void *)Task_PCB;
		return true;
	}
	Task_PCB->p_Next=NULL;
	Task_PCB->p_Prev=(PCB *)queue->p_Tail;
	((PCB *)queue->p_Tail)->p_Next=Task_PCB;
	queue->p_Tail=(void *)Task_PCB;
	return true;
}

bool Insert_Task_At(PCB* Task_PCB,int Index,Ready_QUEUE* queue)
{
	PCB* Task_;
	if(Index<0||Index>=queue->Task_Count)
	{
		return false;
	}
	if(!Index)
	{
		Add_Task_in_Head(Task_PCB,queue);
		queue->Task_Count--;
	}
	else if(Index==queue->Task_Count-1)
	{
		Add_Task_in_Tail(Task_PCB,queue);
		queue->Task_Count--;
	}
	else
	{
		Task_=Get_Task_At(Index,queue);
		Task_PCB->p_Next=Task_;
		Task_PCB->p_Prev=Task_->p_Prev;
		Task_->p_Prev=Task_PCB;
		Task_PCB->p_Prev->p_Next=Task_PCB;
		Task_->p_Next=NULL;
	} 
	queue->Task_Count++;
	return true;
}

PCB* Remove_Task_At(int Index,Ready_QUEUE* queue)
{
	PCB* Task_;
	if(Index<0||Index>=queue->Task_Count)
	{
		return NULL;
	}
	if(queue->Task_Count==1)
	{
		Task_=queue->p_Head;
		queue->p_Head=NULL;
		queue->p_Tail=NULL;
	}
	else if(Index==0)
	{
		Task_=queue->p_Head;
		Task_->p_Next->p_Prev=NULL;
		queue->p_Head=Task_->p_Next;
		Task_->p_Next=NULL;
	}
	else if(Index==queue->Task_Count-1)
	{
		Task_=queue->p_Tail;
		Task_->p_Prev->p_Next=NULL;
		queue->p_Tail=Task_->p_Prev;
		Task_->p_Prev=NULL;
	}
	else
	{
		Task_=Get_Task_At(Index,queue);
		Task_->p_Next->p_Prev=Task_->p_Prev;
		Task_->p_Prev->p_Next=Task_->p_Next;
		Task_->p_Next=NULL;
	} 
	queue->Task_Count--;
	return Task_;
}

PCB* Get_Task_At(int Index,Ready_QUEUE* queue)
{
	PCB* NowTemp;
	if(Index<0||Index>=queue->Task_Count)
	{
		return NULL;
	}
	if(queue->Task_Count/2<Index)
	{
		NowTemp=queue->p_Head;
		for(Index; Index>0; Index--)
		{
			NowTemp=NowTemp->p_Next;
		}
	}
	else
	{
		NowTemp=queue->p_Tail;
		for(Index; Index<queue->Task_Count-1; Index++) //인덱스 4에 개수 7개  
		{
			NowTemp=NowTemp->p_Prev;
		}
	}
	return NowTemp;
}

bool Add_Child_in_Head(PCB* Child,PCB* Parent)
{
	if(Get_SlapManager()->MaxCount[RES_TCB]==Get_SlapManager()->UseCount[RES_TCB])
	{
		return false;
	}
	if(!(Parent->Childs.Task_Count++))
	{
		Parent->Childs.p_Head=(void *)Child;
		Parent->Childs.p_Tail=(void *)Child;
		return true;
	}
	Child->child_Prev=NULL;
	Child->child_Next=(PCB *)Parent->Childs.p_Head;
	((PCB *)Parent->Childs.p_Head)->child_Prev=Child;
	Parent->Childs.p_Head=(void *)Child;
	return true;
}

bool Add_Child_in_Tail(PCB* Child,PCB* Parent)
{
	if(Get_SlapManager()->MaxCount[RES_TCB]==Get_SlapManager()->UseCount[RES_TCB])
	{
		return false;
	}
	if(!(Parent->Childs.Task_Count++))
	{
		Parent->Childs.p_Head=(void *)Child;
		Parent->Childs.p_Tail=(void *)Child;
		return true;
	}
	Child->p_Next=NULL;
	Child->child_Prev=(PCB *)Parent->Childs.p_Tail;
	((PCB *)Parent->Childs.p_Tail)->child_Next=Child;
	Parent->Childs.p_Tail=(void *)Child;
	return true;
}

PCB* Remove_Child_At(int Index,PCB* Parent)
{
	PCB* Task_;
	if(Index<0||Index>=Parent->Childs.Task_Count)
	{
		return NULL;
	}
	if(Parent->Childs.Task_Count==1)
	{
		Task_=Parent->Childs.p_Head;
		Parent->Childs.p_Head=NULL;
		Parent->Childs.p_Tail=NULL;
	}
	else if(Index==0)
	{
		Task_=Parent->Childs.p_Head;
		Task_->child_Next->child_Prev=NULL;
		Parent->Childs.p_Head=Task_->child_Next;
		Task_->child_Next=NULL;
	}
	else if(Index==Parent->Childs.Task_Count-1)
	{
		Task_=Parent->Childs.p_Tail;
		Task_->child_Prev->child_Next=NULL;
		Parent->Childs.p_Tail=Task_->child_Prev;
		Task_->child_Prev=NULL;
	}
	else
	{
		Task_=Get_Child_At(Index,Parent);
		Task_->child_Next->child_Prev=Task_->child_Prev;
		Task_->child_Prev->child_Next=Task_->child_Next;
		Task_->child_Next=NULL;
	} 
	Parent->Childs.Task_Count--;
	return Task_;
}

PCB* Get_Child_At(int Index,PCB* Parent)
{
	PCB* NowTemp;
	if(Index<0||Index>=Parent->Childs.Task_Count)
	{
		return NULL;
	}
	if(Parent->Childs.Task_Count/2<Index)
	{
		NowTemp=Parent->Childs.p_Head;
		for(Index; Index>0; Index--)
		{
			NowTemp=NowTemp->child_Next;
		}
	}
	else
	{
		NowTemp=Parent->Childs.p_Tail;
		for(Index; Index<Parent->Childs.Task_Count-1; Index++) //인덱스 4에 개수 7개  
		{
			NowTemp=NowTemp->child_Prev;
		}
	}
	return NowTemp;
}

Ready_QUEUE* Return_READY_QUEUE(void)
{
	return ReadyQueue;
}

void TestTask_Create(int argc,char* argv[])
{
	int i;
	for(i=0; i<1; i++)
	{
		if(Create_Process(PRIORITY_MIDDLE,(ulld)TestTask1,0,0,true)==NULL)
		{
			break;
		}
	}
	
	for(i=0; i<1; i++)
	{
		if(Create_Process(PRIORITY_MIDDLE,(ulld)TestTask2,0,0,true)==NULL)
		{
			break;
		}
	}
}

void TestTask1(void)
{
//	double a=(double)10.0f;
	static int Count1=0;
	PCB* Me=g_scheduler.CURRENT_Process;
	while(1)
	{
		printk("Task_1",Me->PID,0,0x0A);
		GiveOut_Processor();
	}
	
}

void TestTask2(void)
{
	static int Count2=0;
	PCB* Me=g_scheduler.CURRENT_Process;
	while(1)
	{
		printk("Task_2",Me->PID,0,0x03);
		GiveOut_Processor();
	}
}
