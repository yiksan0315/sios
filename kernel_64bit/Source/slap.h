#ifndef _SLAP_H_
#define _SLAP_H_

#include "Data Types.h"

#define RESOUCE_COUNT 2

typedef enum eResources
{
	RES_TCB,
	RES_SEMAPHORE,
}RESOURCES;

typedef struct struct_TaskPool
{
	void* Slab[RESOUCE_COUNT]; 
	int MaxCount[RESOUCE_COUNT];
	int UseCount[RESOUCE_COUNT];
}Slap_cache_Manager;

Slap_cache_Manager* Get_SlapManager(void);
void Set_PCBSlab(void *PCBPool,int MaxCount);
void Set_SemaphoreSlab(void *SemaphorePool,int MaxCount);

#endif
