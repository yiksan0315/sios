#include "DiskDriver.h"
#include "Assembly_Func.h"
#include "IO.h"

DiskInfo g_DiskInformation;
Semaphore* Disk_Mutex;

bool IsInteruppted;

bool InitDisk(void)
{
	Disk_Mutex=Alloc_Semaphore(1);
	WritePort_Out(PATA_PORT_1+OFFSET_PORT_DIGITAL,0x00);
	WritePort_Out(PATA_PORT_2+OFFSET_PORT_DIGITAL,0x00);
	return SetDisk(PATA_PORT_1,true);
}

bool SetDisk(ushort ParalellPort,bool IsMaster)
{
	int Count=0xFFFF;
	uchar Status,LBA=0xE0;
	if(!IsMaster)
	{
		LBA|=0x10;
	}
	Status=ReadPort_In(ParalellPort+OFFSET_PORT_STATUS);
	while((Status&STATUS_BSY))
	{
		if(Status&STATUS_ERROR)
		{
			return false;
		}
		Status=ReadPort_In(ParalellPort+OFFSET_PORT_STATUS);
	}
	WritePort_Out(ParalellPort+OFFSET_PORT_DRIVE_HEAD,LBA);
	
	Status=ReadPort_In(ParalellPort+OFFSET_PORT_STATUS);
	while(!(Status&STATUS_RDY))
	{
		if(Status&STATUS_ERROR)
		{
			return false;
		}
		Status=ReadPort_In(ParalellPort+OFFSET_PORT_STATUS);
	}
	IsInteruppted=false;
	WritePort_Out(ParalellPort+OFFSET_PORT_COMMAND,DISK_COMMAND_IDNETIFY);
	while(!IsInteruppted)
	{
		Status=ReadPort_In(ParalellPort+OFFSET_PORT_STATUS);
		if(Status&STATUS_ERROR)
		{
			return false;
		}
	}
	IsInteruppted=false;
	GetSector(&g_DiskInformation,ParalellPort+OFFSET_PORT_DATA);
	Swap(g_DiskInformation.ModelNumber,sizeof(g_DiskInformation.ModelNumber)/2);
	Swap(g_DiskInformation.SerialNumber,sizeof(g_DiskInformation.SerialNumber)/2);	
	return true;
}

uint ReadSector(ushort ParalellPort,bool IsMaster,ulld LBA_Num,uint Count,void *Buffer)
{
	if(Count<=0||Count>256||(LBA_Num+Count)>=g_DiskInformation.TotalSectors)
	{
		return 0;
	}
	int i;
	uchar Status;
	ulld Offset=0;	uchar Drive=0xE0;
	if(!IsMaster)
	{
		Drive|=0x10;
	}
	while(!acquire(Disk_Mutex));
	Status=ReadPort_In(ParalellPort+OFFSET_PORT_STATUS);
	while((Status&STATUS_BSY))
	{
		if(Status&STATUS_ERROR)
		{
			release(Disk_Mutex);
			return 0;
		}
		Status=ReadPort_In(ParalellPort+OFFSET_PORT_STATUS);
	}  
	WritePort_Out(ParalellPort+OFFSET_PORT_SECTORCNT,Count);
	WritePort_Out(ParalellPort+OFFSET_PORT_LBA_LOW,(LBA_Num)&0xFF);
	WritePort_Out(ParalellPort+OFFSET_PORT_LBA_MID,(LBA_Num>>8)&0xFF);
	WritePort_Out(ParalellPort+OFFSET_PORT_LBA_HIGH,(LBA_Num>>16)&0xFF);
	
	WritePort_Out(ParalellPort+OFFSET_PORT_DRIVE_HEAD,Drive|((LBA_Num>>24)&0x0F));
	Status=ReadPort_In(ParalellPort+OFFSET_PORT_STATUS);
	while(!(Status&STATUS_RDY))
	{
		if(Status&STATUS_ERROR)
		{
			release(Disk_Mutex);
			return 0;
		}
		Status=ReadPort_In(ParalellPort+OFFSET_PORT_STATUS);
	}

	IsInteruppted=false;
	WritePort_Out(ParalellPort+OFFSET_PORT_COMMAND,DISK_COMMAND_READ);
	for(i=0; i<Count; i++)
	{
		Status=ReadPort_In(ParalellPort+OFFSET_PORT_STATUS);
		if(!(Status&STATUS_DRQ))
		{
			while(!IsInteruppted);
			IsInteruppted=false;
		} 
		GetSector(Buffer,ParalellPort);
		Buffer=Buffer+SECTOR;
	}
	release(Disk_Mutex);
	return i; 
}

uint WriteSector(ushort ParalellPort,bool IsMaster,ulld LBA_Num,uint Count,void *Data)
{
	if(Count<=0||Count>256||(LBA_Num+Count)>=g_DiskInformation.TotalSectors)
	{
		return 0;
	}
	int i;
	uchar Status;
	ulld Offset=0;	uchar Drive=0xE0;
	if(!IsMaster)
	{
		Drive|=0x10;
	}
	while(!acquire(Disk_Mutex));
	Status=ReadPort_In(ParalellPort+OFFSET_PORT_STATUS);
	while((Status&STATUS_BSY))
	{
		if(Status&STATUS_ERROR)
		{
			release(Disk_Mutex);
			return 0;
		}
		Status=ReadPort_In(ParalellPort+OFFSET_PORT_STATUS);
	}  
	WritePort_Out(ParalellPort+OFFSET_PORT_SECTORCNT,Count);
	WritePort_Out(ParalellPort+OFFSET_PORT_LBA_LOW,(LBA_Num)&0xFF);
	WritePort_Out(ParalellPort+OFFSET_PORT_LBA_MID,(LBA_Num>>8)&0xFF);
	WritePort_Out(ParalellPort+OFFSET_PORT_LBA_HIGH,(LBA_Num>>16)&0xFF);
	
	WritePort_Out(ParalellPort+OFFSET_PORT_DRIVE_HEAD,Drive|((LBA_Num>>24)&0x0F));	
	Status=ReadPort_In(ParalellPort+OFFSET_PORT_STATUS);
	while(!(Status&STATUS_RDY))
	{
		if(Status&STATUS_ERROR)
		{
			release(Disk_Mutex);
			return 0;
		}
		Status=ReadPort_In(ParalellPort+OFFSET_PORT_STATUS);
	}

	IsInteruppted=false;
	WritePort_Out(ParalellPort+OFFSET_PORT_COMMAND,DISK_COMMAND_WRITE);
	for(i=0; i<Count; i++)
	{
		Status=ReadPort_In(ParalellPort+OFFSET_PORT_STATUS);
		if(!(Status&STATUS_DRQ))
		{
			while(!IsInteruppted);
			IsInteruppted=false;
		} 
		SetSector(Data,ParalellPort);
		Data=Data+SECTOR;
	}
	release(Disk_Mutex);
	return i;
}

void Swap(ushort *Data,int Size)
{
	int i;
	for(i=0; i<Size; i++)	
		Data[i]=(Data[i]>>8)|(Data[i]<<8);
}

void GetSector(void *Buffer,ushort Port)
{
	__asm__ __volatile__("mov rdx,rsi;");
	__asm__ __volatile__("mov rcx, 256");
	__asm__ __volatile__("rep insw");
}

void SetSector(void* Data,ushort Port)
{
	__asm__ __volatile__("mov rdx,rsi;");
	__asm__ __volatile__("mov rsi,rdi;");
	__asm__ __volatile__("mov rcx, 256");
	__asm__ __volatile__("rep outsw");
}

bool* Flag_Interuppted(void)
{
	return &IsInteruppted;
}

DiskInfo* return_diskinfo(void)
{
	return &g_DiskInformation;
}
