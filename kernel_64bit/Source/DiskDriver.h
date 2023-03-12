#ifndef _DISKDRIVER_H_
#define _DISKDRIVER_H_

#include "Data Types.h"
#include "Scheduler.h"
#include "semaphore.h"

#define PATA_PORT_1 0x1F0
#define PATA_PORT_2 0x170

#define OFFSET_PORT_DATA 		0x0
#define OFFSET_PORT_ERROR 		0x1
#define OFFSET_PORT_SECTORCNT	0x2
#define OFFSET_PORT_LBA_LOW		0x3
#define OFFSET_PORT_LBA_MID		0x4
#define OFFSET_PORT_LBA_HIGH	0x5
#define OFFSET_PORT_DRIVE_HEAD	0x6
#define OFFSET_PORT_STATUS 		0x07	
#define OFFSET_PORT_COMMAND 	0x07
#define OFFSET_PORT_DIGITAL 	0x206

#define DISK_COMMAND_READ		0x20
#define DISK_COMMAND_WRITE		0x30
#define DISK_COMMAND_IDNETIFY	0xEC

#define STATUS_ERROR 			0x01
#define STATUS_INDEX 			0x02
#define STATUS_CORR  			0x04
#define STATUS_DRQ   			0x08
#define STATUS_SRV   			0x10
#define STATUS_DF    			0x20
#define STATUS_RDY   			0x40
#define STATUS_BSY   			0x80

#define SECTOR 512

typedef struct sturct_DiskInfo
{
    ushort Configuation;

    ushort NumberOfCylinder;
    ushort Reserved1;

    ushort NumberOfHead;
    ushort UnformattedBytesPerTrack;
    ushort UnformattedBytesPerSector;

    ushort NumberOfSectorPerCylinder;
    ushort InterSectorGap;
    ushort BytesInPhaseLock;
    ushort NumberOfVendorUniqueStatusWord;

    ushort SerialNumber[10];
    ushort ControllerType;
    ushort BufferSize; 
    ushort NumberOfECCBytes;
    ushort FirmwareRevision[4];
    
    ushort ModelNumber[20];
    ushort Reserved2[13];

    uint TotalSectors;   
	ushort Reserved3[198];
}DiskInfo;


bool InitDisk(void);
bool SetDisk(ushort ParalellPort,bool IsMaster);

uint ReadSector(ushort ParalellPort,bool IsMaster,ulld LBA_Num,uint Count,void *Buffer);
uint WriteSector(ushort ParalellPort,bool IsMaster,ulld LBA_Num,uint Count,void *Data);

void Swap(ushort *Data,int Size);
void GetSector(void *Buffer,ushort Port);
void SetSector(void* Data,ushort Port);

bool* Flag_Interuppted(void);

DiskInfo* return_diskinfo(void);

#endif
