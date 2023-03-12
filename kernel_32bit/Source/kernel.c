#include "IO.h"
#include "Page.h"

bool Check_Memory(void);
bool Init_IA32e(void);

void _CPUID(uint arg_EAX,uint* ret_EAX,uint* EBX,uint* ECX,uint* EDX);
void Switch_IA_32e_Mode();
void kCopyKernel64ImageTo2Mbyte( void );

void main()
{ 
	char CpuMessage[22]={"Cpu is:["};
	uint arg_ERgst[4];
	printk("Check Memory size...",7,_WHITE);
	if(!Check_Memory())
	{
		printk("Not Enough Memory... Memory is at least 128MB",8,_RED);
		return ;
	}
	printk("Memory Size is Enough!",8,_BLUE);
	
	printk("Initializing IA-32e Kernel Area...",9,_WHITE);

	if(!Init_IA32e()) 
	{
		printk("Error:Can't Initializing IA-32e Kernel Area...",10,_RED);
		return ;
	}
	printk("Initializing IA-32e Kernel Area Successfully!",10,_BLUE);
	_CPUID(0,&arg_ERgst[0],&arg_ERgst[1],&arg_ERgst[2],&arg_ERgst[3]);
	
	*((uint*)CpuMessage+2)=arg_ERgst[1]; *((uint*)CpuMessage+3)=arg_ERgst[3]; *((uint*)CpuMessage+4)=arg_ERgst[2];
	CpuMessage[20]=']'; CpuMessage[21]='\0';
	printk(CpuMessage,12,CYAN);
	
	_CPUID(0x80000001,&arg_ERgst[0],&arg_ERgst[1],&arg_ERgst[2],&arg_ERgst[3]);
	if(arg_ERgst[3]&0x20000000) //0010 0000 0000 0000 0000 0000 0000 0000
	{
		printk("This Cpu Supports IA-32e Mode!",13,_BLUE);
	}
	else
	{
		printk("This Cpu doesn't Support IA-32e Mode! ... Can't Switch kernel to IA-32e Mode",13,_RED);
		return ;
	}
	printk("Switch to IA-32e Mode...",15,_WHITE);
	MakePageTable();
	kCopyKernel64ImageTo2Mbyte();
	Switch_IA_32e_Mode();
    return ;
}

bool Check_Memory(void)
{
	uint* NowAddress;
	for(NowAddress=(uint *)0x100000; NowAddress<(uint *)0x8000000; NowAddress+=(0x100000/4))
	{
		*NowAddress=0x12345678;
		if(*NowAddress!=0x12345678)
		{
			return false;
		}
	}
	return true;
}

bool Init_IA32e(void)
{
	uint* NowAddress;
	for(NowAddress=(uint *)0x100000; NowAddress<(uint *)0x600000; NowAddress++)
	{
		*NowAddress=0x00;
		if(*NowAddress!=0x00)
			return false;
	}
	return true;
}

void kCopyKernel64ImageTo2Mbyte( void )
{
    unsigned short wKernel32SectorCount, wTotalKernelSectorCount;
    uint* pdwSourceAddress,* pdwDestinationAddress;

    int i;
    wTotalKernelSectorCount = *( (unsigned short* ) 0x7D4A );
    wKernel32SectorCount = *( ( unsigned short* ) 0x7D48 );
    pdwSourceAddress = ( uint* ) ( 0x10000 + ( wKernel32SectorCount * 512 ) );

    pdwDestinationAddress = ( uint* ) 0x200000;

    // IA-32e 모드 커널 섹터 크기만큼 복사

    for( i = 0 ; i < 512 * ( wTotalKernelSectorCount - wKernel32SectorCount ) / 4; i++ )
    {
        *pdwDestinationAddress = *pdwSourceAddress;
        pdwDestinationAddress++;
        pdwSourceAddress++;
    }
}

