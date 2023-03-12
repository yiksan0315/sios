#include "Command.h"
#include "Scheduler.h"
#include "string.h"
#include "DiskDriver.h"
#include "Memory.h"
#include "FileSystem.h"
#include "Shell.h"

extern int g_Cursor;
extern DiskInfo g_DiskInformation;

void help(int argc,char** argv)
{	
	int i;
	Command_Table* comtable=return_comtable();
	for(i=0; i<COMMAND_COUNT; i++)
	{
		if(i%6==0&&i)
		{
			putchar('\n');
		}
		printf("%s     ",comtable[i].Command); 
	}
}

void Clear_Command(int argc,char** argv)
{
	cleark();
	g_Cursor=0-WIDTH;
}

void date(int argc,char** argv)
{
	char DAYOFWEEK[7][10]={"Monday","Tuesday","Wednesday","Thursday","Friday","Saturday","Sunday"};
	ushort year;
	uchar month,day,dayofweek,hour,minute,second;
	
	year=2000+GetRTC(RTC_YEAR);
	month=GetRTC(RTC_MONTH);
	day=GetRTC(RTC_DAY);
	dayofweek=GetRTC(RTC_DAYOFWEEK);
	hour=GetRTC(RTC_HOUR);
	minute=GetRTC(RTC_MINUTE);
	second=GetRTC(RTC_SECOND);

	printf("%d/%d/%d %s %d:%d:%d\n",year,month,day,DAYOFWEEK[dayofweek+1],hour,minute,second);
}

void kill(int argc,char** argv)
{
	if(string_to_num(argv[1])<=0)
	{
		printf("Usage:kill {PID}");
	}
	else if(Terminate_Process(string_to_num(argv[1])))
	{
		printf("Terminate {Process:PID[%d]} >>> Successed.",string_to_num(argv[1]));
	}
	else
	{
		printf("Terminate {Process:PID[%d]} >>> Faliled.",string_to_num(argv[1]));
	}
}

void shutdown(int argc,char** argv)
{
	bool Flag;
	char Key;
	Input:
	printf("Do you want to shutdown pc?  [y/n]:");
	Key=GetKeyBuf(&Flag);
	while(!Flag) Key=GetKeyBuf(&Flag);
	switch(Key)
	{
		case 'y':
		case 'Y':
			break;
		case 'n':
		case 'N':
			putchar(Key);
			printf("\nCancled Shutdown pc.\n");
			return ;
		default:
			putchar(Key);
			putchar('\n');
			goto Input;
	}
 	__asm__ __volatile__(
 		"mov rax,0x2000;"
 		"mov rdx,0x604;"
		"outw dx,ax;"
 	);
 	printf("Error:Can't shutdown....\n");
}

void systeminfo(int argc,char **argv)
{
	int i;
	char Buffer[100];
	cpuid_(0,(ulld)(&Buffer[0]),(ulld)(&Buffer[8]),(ulld)(&Buffer[4]));
	Buffer[12]='\0';
	printf("        ===========CPU info=============\n");
	printf("Cpu is:[%s]\n",Buffer);
	printf("\n        ========Memory Information======\n");
	printf("Size of main memory:[%d]MB\n",GetMemorySize()/1024/1024);
	printf("Size of Dynamic memory:[%d]MB\n",GetKernelMemorySize()/1024/1024);
	printf("\n        ========Disk Information========\n");
	printf("Sector Count of Disk:[%d]\n",g_DiskInformation.TotalSectors);
	for(i=0; i<sizeof(g_DiskInformation.ModelNumber)/2; i++)
	{
		Buffer[i]=((char *)&g_DiskInformation.ModelNumber)[i];
	}
	Buffer[i]='\0'; trim(Buffer);
	printf("Model Name of Disk:[%s]\n",Buffer);
	for(i=0; i<sizeof(g_DiskInformation.SerialNumber)/2; i++)
	{
		Buffer[i]=((char *)&g_DiskInformation.SerialNumber)[i];
	}
	Buffer[i]='\0'; trim(Buffer);
	printf("Serial Number of Disk:[%s]\n",Buffer);
}

void ls(int argc,char **argv)
{
	uint i,j,Bit;
	Dir_Entry entry_dir[ENTRY_COUNT];
	inode inode_table[4];
	ReadSector(PATA_PORT_1,true,return_Super()->inode_table+(*Return_pwd_inode())/4,SECTOR/SECTOR,inode_table);
	printf("File Name | File Type\n");
	for(i=0; i<12; i++)
	{
		ReadSector(PATA_PORT_1,true,return_Super()->block_table+
		inode_table[(*Return_pwd_inode())%4].block[i],BLOCK_SIZE/SECTOR,entry_dir);
		Bit=0x2;
		for(j=1; j<ENTRY_COUNT; j++)
		{
			if(entry_dir[0].file_type&Bit)
			{
				printf("%s           %d\n",entry_dir[j].name,entry_dir[j].file_type);
			}
			Bit=Bit<<1;
		}
	}
}

void touch(int argc,char **argv)
{
	if(argc<=1)
	{
		printf("Usage:touch {Filename}\n");
		return ;
	}
	ERROR ErrorCode;
	int i;
	for(i=1; i<argc; i++)
	{
		ErrorCode=CreateFile(argv[i]);
		switch(ErrorCode)
		{
			case SAME_NAME:
				printf("File \"%s\" is already exist.\n",argv[i]);
				break;
			case SUCCESS:
				printf("Create file:\"%s\" Succefully!\n",argv[i]);
				break;
			case NAME_OVER:
				printf("\"%s\":Name of file must be less than 57byte!\n",argv[i]);
				break;
			case NOT_ENOUGH:
				printf("\"%s\":Now Directory can't get more files...\n",argv[i]);//삭제하고나서 다시 바로 생성할때 이게 걸림 
				break;       									 //inode 할당이 안되는걸로 추정 ,응 아니야 testcommand 잘만됨 
			default:
				printf("\"%s\":Error..... Can't find reason...\n",argv[i]);
				break;
		}//파일생성중 키보드 입력하면 라인이 이상해지는 버그 존재  
	}
}

void rm(int argc,char **argv)
{
	if(argc<=1)
	{
		printf("Usage:rm {Filename}\n");
		return ;
	}
	ERROR ErrorCode;
	int i;
	for(i=1; i<argc; i++)
	{
		ErrorCode=DeleteFile(argv[i]);
		switch(ErrorCode)
		{
			case NO_FILE:
				printf("File \"%s\" is not exist.\n",argv[i]);
				break;
			case SUCCESS:
				printf("Delete file:\"%s\" Succefully!\n",argv[i]);
				break;
			case NOT_FILE:
				printf("File:\"%s\" is a Directory...\n",argv[i]);
				break;
			default:
				printf("Error..... Can't find reason...\n");
				break;
		}
	}
}

void mkdir(int argc,char **argv) 
{
	if(argc<=1)
	{
		printf("Usage:mkdir {Directory name}\n");
		return ;
	}
	ERROR ErrorCode;
	int i;
	for(i=1; i<argc; i++)
	{
		ErrorCode=MakeDir(argv[i]);
		switch(ErrorCode)
		{
			case SAME_NAME:
				printf("Directory \"%s\" is already exist.\n",argv[i]);
				break;
			case SUCCESS:
				printf("Create directory:\"%s\" Succefully!\n",argv[i]);
				break;
			case NAME_OVER:
				printf("\"%s\":Name of directory must be less than 57byte!\n",argv[i]);
				break;
			case NOT_ENOUGH:
				printf("\"%s\":Now Directory can't get more files...\n",argv[i]); 
				break;       								
			default:
				printf("\"%s\":Error..... Can't find reason...\n",argv[i]);
				break;
		}
	}
}

void rmdir(int argc,char **argv)
{
	if(argc<=1)
	{
		printf("Usage:rmdir {Filename}\n");
		return ;
	}
	ERROR ErrorCode;
	int i;
	for(i=1; i<argc; i++)
	{
		ErrorCode=RemoveDir(argv[i]);
		switch(ErrorCode)
		{
			case NO_FILE:
				printf("Directory \"%s\" is not exist.\n",argv[i]);
				break;
			case SUCCESS:
				printf("Delete directory:\"%s\" Succefully!\n",argv[i]);
				break;
			case NOT_FILE:
				printf("Directory:\"%s\" is a File...\n",argv[i]);
				break;
			case NOT_ENOUGH:
				printf("Directory:\"%s\" is disremoveable.\n",argv[i]);
				break;
			case NAME_OVER:
				printf("Directory:\"%s\" has childs... not removeable.\n",argv[i]);
				break;
			default:
				printf("Error..... Can't find reason...\n");
				break;
		}
	}
}

void cd(int argc,char **argv)
{
	if(argc<=1)
	{
		printf("%s\n",return_pwd());
		return ;
	}
	else if(argc>2)
	{
		printf("Usage:cd {Directory name}\n");
		return ;
	}
	
	if(strcmp(argv[1],"/")||strcmp(argv[1],"root"))
	{
		(*Return_pwd_inode())=INODE_ROOTDIR;
		strcpy(return_pwd(),"root");
		return ;
	}
	
	ERROR ErrorCode;
	ErrorCode=ChangeDirectory(argv[1]);
	switch(ErrorCode)
	{
		case SUCCESS:
			printf("Now Path:%s\n",return_pwd());
			break;
		case NO_FILE:
			printf("Directory \"%s\" is not exist.\n",argv[1]);
			break;					
		default:
			printf("\"%s\":Error..... Can't find reason...\n",argv[1]);
			break;
	}
}

void ren(int argc,char **argv)
{
	if(argc<=2||argc>3)
	{
		printf("Usage:ren {File name(Old) File name(New Name)}\n");
		return ;
	}
	ERROR ErrorCode;
	ErrorCode=RenameFile(argv[1],argv[2]);
	switch(ErrorCode)
	{
		case SUCCESS:
			printf("\"%s\"->\"%s\"\n",argv[1],argv[2]);
			break;
		case NO_FILE:
			printf("\"%s\" is not exist.\n",argv[1]);
			break;		
		case NAME_OVER:
			printf("\"%s\":Name must be less than 57byte!\n",argv[1]);
			break;
		case SAME_NAME:	
			printf("\"%s\" is already exist.\n",argv[2]);
			break;		
		default:
			printf("\"%s\":Error..... Can't find reason...\n",argv[1]);
			break;
	}
}

void cat(int argc,char **argv)
{
	if(argc<2)
	{
		printf("Usage:cat {File name}\n");
		return ;
	}
	ERROR ErrorCode;
	ErrorCode=Concatenate(argv[1]);
	switch(ErrorCode)
	{
		case SUCCESS:
			printf("content of \"%s\".\n",argv[1]);
			break;
		case NO_FILE:
			printf("\"%s\" is not exist.\n",argv[1]);
			break;		
		case NOT_FILE:
			printf("\"%s\":is not file.\n",argv[1]);
			break;	
		default:
			printf("\"%s\":Error..... Can't find reason...\n",argv[1]);
			break;
	}
}

void write(int argc,char **argv)
{
	if(argc<2)
	{
		printf("Usage:wrtie {File name content}\n");
		return ;
	}
	ERROR ErrorCode;
	ErrorCode=WriteFile(argv[1],argv[2]);
	switch(ErrorCode)
	{
		case SUCCESS:
			printf("write on \"%s\".\n",argv[1]);
			break;
		case NO_FILE:
			printf("\"%s\" is not exist.\n",argv[1]);
			break;		
		case NOT_FILE:
			printf("\"%s\":is not file.\n",argv[1]);
			break;	
		case NOT_ENOUGH:
			printf("\"%s\":is max.\n",argv[1]);
			break;
		default:
			printf("\"%s\":Error..... Can't find reason...\n",argv[1]);
			break;
	}
}

