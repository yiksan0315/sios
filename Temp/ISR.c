#include "ISR.h"
#include "PIC.h"
#include "Assembly_Func.h"
#include "IO.h"

int a=0;

void ISR_Exception_Div_by0(void)
{
//	SaveContext();
	printk("asdasdas",10,5,_MAGENTA);
//	LoadContext();
//	__asm__ __volatile__("iretq;");
}

void ISR_Exception_Debug(void)
{
	SaveContext();
	LoadContext();
	__asm__ __volatile__("iretq;");
}

void ISR_Exception_NMI(void)
{
	SaveContext();
	LoadContext();
	__asm__ __volatile__("iretq;");
}

void ISR_Exception_BreakPoint(void)
{
	SaveContext();
	LoadContext();
	__asm__ __volatile__("iretq;");
}

void ISR_Exception_INTO_Overflow(void)
{
	SaveContext();
	LoadContext();
	__asm__ __volatile__("iretq;");
}

void ISR_Exception_Bounds_Range_exceeded(void)
{
	SaveContext();
	LoadContext();
	__asm__ __volatile__("iretq;");
}

void ISR_Exception_Invalid_Op(void)
{
	SaveContext();
	LoadContext();
	__asm__ __volatile__("iretq;");
}

void ISR_Exception_Disavailable_Device(void)
{
	SaveContext();
	LoadContext();
	__asm__ __volatile__("iretq;");
}

void ISR_Exception_DoubleFault(void)
{
	SaveContext();
	LoadContext();
	__asm__ __volatile__("iretq;");
}

void ISR_Exception_Copro_Seg_Overrun(void)
{
	SaveContext();
	LoadContext();
	__asm__ __volatile__("iretq;");
}

void ISR_Exception_Invalid_Tss(void)
{
	SaveContext();
	LoadContext();
	__asm__ __volatile__("iretq;");
}

void ISR_Exception_NotPresent_Seg(void)
{
	SaveContext();
	LoadContext();
	__asm__ __volatile__("iretq;");
}

void ISR_Exception_StackFault(void)
{
	SaveContext();
	LoadContext();
	__asm__ __volatile__("iretq;");
}

void ISR_Exception_GprotectionFault(void)
{
	SaveContext();
	LoadContext();
	__asm__ __volatile__("iretq;");
}

void ISR_Exception_PageFault(void)
{
	SaveContext();
	LoadContext();
	__asm__ __volatile__("iretq;");
}

void ISR_Exception_Reserved(void)
{
	SaveContext();
	LoadContext();
	__asm__ __volatile__("iretq;");
}

void ISR_Exception_FPU(void)
{
	SaveContext();
	LoadContext();
	__asm__ __volatile__("iretq;");
}

void ISR_Exception_Check_Alignment(void)
{
	SaveContext();
	LoadContext();
	__asm__ __volatile__("iretq;");
}

void ISR_Exception_Check_Mach(void)
{
	SaveContext();
	LoadContext();
	__asm__ __volatile__("iretq;");
}

void ISR_Exception_SIMD(void)
{
	SaveContext();
	LoadContext();
	__asm__ __volatile__("iretq;");
}

void ISR_Reserved_20_31(void)
{
	SaveContext();
	LoadContext();
	__asm__ __volatile__("iretq;");
}

void ISR_Intterupt_Timer(void)	
{
	//SaveContext();
	Send_EOI_PIC(32-0x20);
	printk("qwert",12,a++,_CYAN);
	//LoadContext();
	//__asm__ __volatile__("iretq;");
}
		
volatile void ISR_Intterupt_KeyBoard(void)
{
	//SaveContext();
	__asm__ __volatile__(
	"push rbp;"
    "mov rbp,rsp;"
    "push rax;"
    "push rbx;"
    "push rcx;"
    "push rdx;"
    "push rdi;"
    "push rsi;"
    "push r8;"
    "push r9;"
    "push r10;"
    "push r11;"
    "push r12;"
    "push r13;"
    "push r14;"
    "push r15;"

    "mov ax,ds;"
    "push rax;"
    "mov ax,es;"
    "push rax;"
    "push fs;"
    "push gs;"

    "mov ax,0x10;"
    "mov ds,ax;"
    "mov es,ax;"
    "mov gs,ax;"
    "mov fs,ax;"
	
//	printk("qqqqq",7,5,_MAGENTA);
//	Send_EOI_PIC(33-0x20);
	//LoadContext();  
    "pop gs;"
    "pop fs;"
    "pop rax;"
    "mov es,ax;"
    "pop rax;"
    "mov ds,ax;"
    
    "push r15;"
    "push r14;"
    "push r13;"
    "push r12;"
    "push r11;"
    "push r10;"
    "push r9;"
    "push r8;"
    "push rsi;"
    "push rdi;"
    "push rdx;"
    "push rcx;"
    "push rbx;"
    "push rax;"
    "push rbp;"
    "iretq;"
	);
//	__asm__ __volatile__("iretq;");
}

void ISR_Intterupt_SlavePIC(void)		
{
	SaveContext();
	LoadContext();
	__asm__ __volatile__("iretq;");
}

void ISR_Intterupt_Serial_2(void)
{
	SaveContext();
	LoadContext();
	__asm__ __volatile__("iretq;");
}

void ISR_Intterupt_Serial_1(void)			
{
	SaveContext();
	LoadContext();
	__asm__ __volatile__("iretq;");
}

void ISR_Intterupt_Parallel_2(void)
{
	SaveContext();
	LoadContext();
	__asm__ __volatile__("iretq;");
}

void ISR_Intterupt_Floppy(void)
{
	SaveContext();
	LoadContext();
	__asm__ __volatile__("iretq;");
}		

void ISR_Intterupt_Parallel_1(void)
{
	SaveContext();
	LoadContext();
	__asm__ __volatile__("iretq;");
}

void ISR_Intterupt_RTC(void)			
{
	SaveContext();
	LoadContext();
	__asm__ __volatile__("iretq;");
}

void ISR_Intterupt_Reserved(void)
{
	SaveContext();
	LoadContext();
	__asm__ __volatile__("iretq;");
}

void ISR_Intterupt_NonUsed_1(void)
{
	SaveContext();
	LoadContext();
	__asm__ __volatile__("iretq;");
}

void ISR_Intterupt_NonUser_2(void)			
{
	SaveContext();
	LoadContext();
	__asm__ __volatile__("iretq;");
}

void ISR_Intterupt_Mouse(void)				
{
	SaveContext();
	LoadContext();
	__asm__ __volatile__("iretq;");
}

void ISR_Intterupt_Coprocessor(void)
{
	SaveContext();
	LoadContext();
	__asm__ __volatile__("iretq;");
}

void ISR_Intterupt_Hdd_1(void)			
{
	SaveContext();
	LoadContext();
	__asm__ __volatile__("iretq;");
}

void ISR_Intterupt_Hdd_2(void)				
{
	SaveContext();
	LoadContext();
	__asm__ __volatile__("iretq;");
}

void ISR_Intterupt_Other(void)			
{
	SaveContext();
	LoadContext();
	__asm__ __volatile__("iretq;");
}

