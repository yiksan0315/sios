#ifndef _SEMAPHORE_H_
#define _SEMAPHORE_H_

#include "Scheduler.h"

#define SEMAPHORE_ADDRESS (TASKPOOL_ADDRESS+sizeof(PCB)*TASKPOOL_MAXCOUNT)
#define SEMAPHOER_MAXCOUNT 256

typedef struct Semaphore
{
	volatile int MaxCount;
	volatile int Count;
	volatile bool IsAlloced;
}Semaphore;

void Init_Semaphore(void);

Semaphore* Alloc_Semaphore(int Count);
void Free_Semaphore(Semaphore* _Free_Semaphore);

bool acquire(Semaphore* Lock);
void release(Semaphore* Lock);

void RemoveFrom_Wait_Queue(PCB* _PROCESS);
#endif
