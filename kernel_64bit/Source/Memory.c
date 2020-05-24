#include "Memory.h"
#include "IO.h"

static ulld g_MemorySize,g_KMemorySize;
ulld g_RootCount;
uchar *g_BuddyTree[ROOT_MAXCOUNT];

void Init_KernelMemory(void)
{
	int i,j; 
	ulld BuddyAddress=KMEM_ADDR;
	CaculateMemorySize();
	ulld KernelMemory=g_MemorySize/8;
	if(KernelMemory>(ulld)2*1024*1024*1024)
	{
		KernelMemory=(ulld)2*1024*1024*1024;
	}
	g_RootCount=KernelMemory/ALLOC_LIMIT; //아 대강 루트 개수 구하는거 나중에
	g_KMemorySize=g_RootCount*ALLOC_LIMIT;

	for(j=0; j<ROOT_MAXCOUNT; j++)
	{
		g_BuddyTree[j]=(uchar*)BuddyAddress;
		for(i=0; i<BUDDY_MAXCOUNT; i++)
		{
			g_BuddyTree[j][i]=0x00;
			if(2048<=i&&i<=4095) //이거는 수정  
			{
				g_BuddyTree[j][i]|=BUDDYFLAG_END;
			}
		}
		BuddyAddress+=ALLOC_LIMIT;
	}
}
//g_BuddyTree의 최대 개수는 4096개, Tree 하나 당 2MB Mapping.-> 2MB*g_RootCount==kernelMemory Size 
  
void* k_malloc(ulld MemSize)
{
	if(MemSize>ALLOC_LIMIT)
	{
		return NULL;
	}
	int i,j; 
	uint Level;
	MemSize=Align_MemSize(MemSize,&Level);
	for(i=0; i<g_RootCount; i++)
	{
		for(j=1<<Level; j<(1<<(Level+1)); j++)
		{
			if(!Is_ParentAlloced(i,j)&&!Is_ChildAlloced(i,j)&&!(g_BuddyTree[i][j]&BUDDYFLAG_ALLOCED))
			{
				g_BuddyTree[i][j]|=BUDDYFLAG_ALLOCED;
				return Caculate_Mem(i,j);
			}
		}
	}
	return NULL;
}

bool k_free(void *Alloced_Memory)
{
	if(Alloced_Memory==NULL)
	{
		return false;
	}
	uint Root_Index,Offset,Index,Level=BUDDY_MAXCOUNT;
	ulld* FreeBuddy;
	ulld AllocedMem,Divide_BuddySize=LEAST_LIMIT;
	
	FreeBuddy=Alloced_Memory;
	AllocedMem=(ulld)Alloced_Memory-KMEM_ADDR;
	Root_Index=AllocedMem/ALLOC_LIMIT;
	AllocedMem=AllocedMem%ALLOC_LIMIT;
	while(Level!=1)
	{
		Offset=AllocedMem/Divide_BuddySize;
		Level=Level>>1;
		Index=Level+Offset;
		if(g_BuddyTree[Root_Index][Index]&BUDDYFLAG_ALLOCED)
		{
			g_BuddyTree[Root_Index][Index]&=0xFE;
			Free_Buddy(FreeBuddy,Divide_BuddySize);
			return true;
		}
		if(!(Offset%2))
		{
			Divide_BuddySize=Divide_BuddySize<<1;
		}
		else
		{
			break;
		}
	
	}
	return false;
}
	
void Free_Buddy(ulld* Free_Address,ulld Buddy_Size)
{
	int i;
	for(i=0; i<Buddy_Size; i++)
		((uchar*)Free_Address)[i]=0x00;
}

void* Caculate_Mem(uint _rootIndex,uint _treeIndex)
{
	uint _indexTemp=_treeIndex;
	ulld Buddy_Offset,Offset_Tree,_2n=1;

	while(_indexTemp)
	{
		_indexTemp=_indexTemp>>1;
		_2n=_2n<<1;
	}
	_2n=_2n>>1;
	Offset_Tree=_rootIndex*ALLOC_LIMIT;
	Buddy_Offset=(ALLOC_LIMIT/_2n)*(_treeIndex-_2n);
	
	return (void *)(KMEM_ADDR+Offset_Tree+Buddy_Offset);
}

ulld Align_MemSize(ulld MemSize,uint* Level)
{
	int Level_n=0;
	ulld AlignedSize=ALLOC_LIMIT;
	while(true)
	{
		if(AlignedSize<=MemSize)
		{
			*Level=Level_n;
			return AlignedSize;
		}
		AlignedSize=AlignedSize>>1;
		Level_n++;
	}
} 

bool Is_ParentAlloced(uint RootIndex,uint Index)
{
	if(g_BuddyTree[RootIndex][Index]&BUDDYFLAG_ALLOCED)
	{
		return true;
	}
	if(Index==1)
	{
		return false;
	}
	return Is_ParentAlloced(RootIndex,Index/2);
}

bool Is_ChildAlloced(uint RootIndex,uint Index)
{
	if(g_BuddyTree[RootIndex][Index]&BUDDYFLAG_ALLOCED)
	{
		return true;
	}
	if(g_BuddyTree[RootIndex][Index]&BUDDYFLAG_END)
	{
		return false;
	}
	return Is_ParentAlloced(RootIndex,Index*2) || Is_ParentAlloced(RootIndex,Index*2+1);
} 

void CaculateMemorySize(void)
{
	ulld Temp;
	ulld* NowAddress;
	for(NowAddress=(ulld *)0x4000000; true; NowAddress+=(0x400000/4))
	{
		Temp=*NowAddress;
		*NowAddress=0x12345678;
		if(*NowAddress!=0x12345678)
		{
			break;
		}
		*NowAddress=Temp;
	}
	g_MemorySize=(ulld)NowAddress;
}

ulld GetMemorySize(void)
{
	return g_MemorySize;
}

ulld GetKernelMemorySize(void)
{
	return g_KMemorySize;
}

//최대 메모리 요청 상한은 2MB 

