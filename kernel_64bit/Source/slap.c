#include "slap.h"

Slap_cache_Manager g_Slap_Manager;

Slap_cache_Manager* Get_SlapManager(void)
{
	return &g_Slap_Manager;
}

void Set_PCBSlab(void *PCBPool,int MaxCount)
{
	g_Slap_Manager.MaxCount[RES_TCB]=MaxCount;
	g_Slap_Manager.UseCount[RES_TCB]=0;
	g_Slap_Manager.Slab[RES_TCB]=PCBPool;
}

void Set_SemaphoreSlab(void *SemaphorePool,int MaxCount)
{
	g_Slap_Manager.MaxCount[RES_SEMAPHORE]=MaxCount;
	g_Slap_Manager.UseCount[RES_SEMAPHORE]=0;
	g_Slap_Manager.Slab[RES_SEMAPHORE]=SemaphorePool;
}

