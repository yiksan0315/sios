#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include "Set_HardWare.h"
#include "Data Types.h"

#define PID_SCHED 0

#define STATE_NEW 0
#define STATE_READY 1
#define STATE_RUNNING 2
#define STATE_WAITING 3
#define STATE_TERMINATED 4

#define PRIORITY_TYPES 5

#define PRIORITY_VERYHIGH 0
#define PRIORITY_HIGH 1
#define PRIORITY_MIDDLE 2 
#define PRIORITY_LOW 3
#define PRIORITY_VERYLOW 4

#define TASKPOOL_MAXCOUNT 2048
#define TASKPOOL_ADDRESS 0xC00000

#define STACK_ADDRESS 0x1400000
#define STACK_SIZE 8192

#define GS         0
#define FS         1
#define ES         2
#define DS         3
#define R15        4
#define R14        5
#define R13        6
#define R12        7
#define R11        8
#define R10        9
#define R9        10
#define R8        11 
#define RSI       12
#define RDI       13
#define RDX       14
#define RCX       15 
#define RBX       16
#define RAX		  17
#define RBP		  18
#define RIP		  19
#define CS		  20
#define RFLAGS	  21
#define RSP		  22
#define SS		  23

#define REGISTER_COUNT 24
#define REGISTER_SIZE 8

#define PROCESSOR_TIME 5 

#pragma pack(push,1)

typedef struct struct_Ready_QUEUE
{
	void* p_Head;
	void* p_Tail;
	int Task_Count;
}Ready_QUEUE;


typedef struct struct_IO
{
	void *Buffer;
	ulld Sector_Count;
	ulld ParalellPort;
	bool Command_Read;
	uint Drive;
	ulld LBA_Num;
}IO_Info;

typedef struct _PCB
{
	ulld PFlag;
	ulld Priority;

	ulld PID;
	ulld ParentPID;
	
	ulld FPUContext[64];
	ulld REGISTER[REGISTER_COUNT];
	
	Ready_QUEUE Childs;
	
	void *MemoryAddress;
	ulld MemorySize;
	
	void *StackAddress;
	ulld StackSize;
	
	struct _PCB* p_Next;
	struct _PCB* p_Prev;
	
	struct _PCB* child_Next;
	struct _PCB* child_Prev;
	
	bool PCB_Is_Alloced;
	bool Is_Thread;
	bool Use_FPU;
	
	IO_Info PCB_IO_info;
	char Padding[4];
}PCB;

typedef struct struct_Scheduler
{
	PCB* CURRENT_Process;
	int Time_;
	Ready_QUEUE *Process_Queue;
	unsigned char Schedule_Priority;
	ulld LastUsed_PID;
}Scheduler;


#pragma pack(pop)

void sched(void);
void sched_CreateStartProcess(void);

void Init_Scheduler(void);
void Init_TCBPool(void);
void Init_Ready_QUEUE(void);

bool Add_Task_in_Head(PCB* Task_PCB,Ready_QUEUE* queue);
bool Add_Task_in_Tail(PCB* Task_PCB,Ready_QUEUE* queue);
bool Insert_Task_At(PCB* Task_PCB,int Index,Ready_QUEUE* queue);
PCB* Remove_Task_At(int Index,Ready_QUEUE* queue);
PCB* Get_Task_At(int Index,Ready_QUEUE* queue);

bool Add_Child_in_Head(PCB* Child,PCB* Parent);
bool Add_Child_in_Tail(PCB* Child,PCB* Parent);
PCB* Remove_Child_At(int Index,PCB* Parent);
PCB* Get_Child_At(int Index,PCB* Parent);

PCB* Create_Process(ulld pPriority,ulld pEntryAddr,void *pMemoryAddress,ulld MemorySize,bool isThread);
bool Terminate_Process(ulld pPID);
void GiveOut_Resource(void);
void exit(void);
bool ChangePriority(ulld pPID,int pPriority);
void Set_Attr_Process(PCB* p_PCB,ulld pFlag,ulld pPID,ulld pEntryAddr,void *pStackAddr,ulld pStackSize);
void Free_PCB(PCB* Task_PCB);

void GiveOut_Processor(void);
bool Preempted_ByTimeOut(void);

PCB* Schedule_Process(void);
bool Is_TaskInQueue(void);

void RemoveFrom_ReadyQueue(PCB* _PROCESS);

void DecreaseProcessorTime(void);
bool IsTimeOut(void);

void TestTask_Create(int argc,char* argv[]);
void TestTask1(void);
void TestTask2(void);

void SwitchContext(ulld *Context_Victim,ulld* Context_Scheduled);
void SaveFPU(ulld *Context_FPU);
void LoadFPU(ulld *Context_FPU);

Ready_QUEUE* Return_READY_QUEUE(void);


#endif
