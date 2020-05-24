#ifndef _MEMORY_H_
#define _MEMORY_H_

#include "semaphore.h"

#define KMEM_ADDR   0x2000000
#define ALLOC_LIMIT 0x200000
#define LEAST_LIMIT 0x400

#define BUDDY_ADDRESS 0x800000
#define BUDDY_MAXCOUNT 0x1000 /*==4096==4KB*/
#define ROOT_MAXCOUNT 0x400   /*==1024*/

#define BUDDYFLAG_ALLOCED 0x1   //할당됨   
#define BUDDYFLAG_END 0x2 //제일 아래 노드임    

void Init_KernelMemory(void);

void* k_malloc(ulld MemSize);
bool k_free(void *Alloced_Memory);

void* Caculate_Mem(uint _rootIndex,uint _treeIndex);
void Free_Buddy(ulld* Free_Address,ulld Buddy_Size);

ulld Align_MemSize(ulld MemSize,uint* Level);
bool Is_ParentAlloced(uint RootIndex,uint Index);
bool Is_ChildAlloced(uint RootIndex,uint Index); 

void CaculateMemorySize(void);
ulld GetMemorySize(void);
ulld GetKernelMemorySize(void);

#endif
