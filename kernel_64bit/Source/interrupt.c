#include "interrupt.h"
#include "ISR.h"
#include "IO.h"

static void* g_ISR_Vector[COUNT_ROUTINE]=
{
	ISR_Exception_Div_by0, ISR_Exception_Debug,	ISR_Exception_NMI, ISR_Exception_BreakPoint, ISR_Exception_INTO_Overflow, ISR_Exception_Bounds_Range_exceeded,
	ISR_Exception_Invalid_Op, ISR_Exception_Disavailable_Device, ISR_Exception_DoubleFault, ISR_Exception_Copro_Seg_Overrun, ISR_Exception_Invalid_Tss, ISR_Exception_NotPresent_Seg,
	ISR_Exception_StackFault, ISR_Exception_GprotectionFault, ISR_Exception_PageFault, ISR_Exception_Reserved, ISR_Exception_FPU, ISR_Exception_Check_Alignment,
	ISR_Exception_Check_Mach, ISR_Exception_SIMD,
	ISR_Reserved_20_31, ISR_Reserved_20_31, ISR_Reserved_20_31, ISR_Reserved_20_31, ISR_Reserved_20_31, ISR_Reserved_20_31, 
	ISR_Reserved_20_31, ISR_Reserved_20_31, ISR_Reserved_20_31, ISR_Reserved_20_31, ISR_Reserved_20_31, ISR_Reserved_20_31, 
	ISR_Intterupt_Timer, ISR_Intterupt_KeyBoard, ISR_Intterupt_SlavePIC, ISR_Intterupt_Serial_2, ISR_Intterupt_Serial_1, ISR_Intterupt_Parallel_2,			
	ISR_Intterupt_Floppy, ISR_Intterupt_Parallel_1, ISR_Intterupt_RTC, ISR_Intterupt_Reserved, ISR_Intterupt_NonUsed_1, ISR_Intterupt_NonUser_2,
	ISR_Intterupt_Mouse, ISR_Intterupt_Coprocessor, ISR_Intterupt_Hdd_1, ISR_Intterupt_Hdd_2
};

void SetTables_GDT_IDT_TSS(void) //gdt에는 이상 없음 //idt에도 이상 없음(불확실 ) , tss 들어가는 부분 메모리 초기화를 안해줌  ,tr_의 주소값이 이상함  , 실제로 넣어준 주소값과 다르게 나옴  
{
	int i;  //gdt에 tss 디스크립터 기준주소 정확한지 확인  
	GDTR* gdtr_=(GDTR*)PAGETABLE_END;
	gdtr_->GDT_IDT_BaseAddress=(unsigned long int)(PAGETABLE_END+sizeof(GDTR));
	gdtr_->GDT_IDT_Size=(unsigned int)(DCPT_COUNT*sizeof(Dcpt)+TSS_DCPT_COUNT*sizeof(Tss_Dcpt)-1);
	
	IDTR* idtr_=(IDTR*)(gdtr_->GDT_IDT_BaseAddress+gdtr_->GDT_IDT_Size+1);
	idtr_->GDT_IDT_BaseAddress=(unsigned long int)idtr_+sizeof(IDTR);
	idtr_->GDT_IDT_Size=(unsigned int)(sizeof(IDT_Dcpt)*COUNT_ROUTINE-1);
	
	Tss_Seg* tr_=(Tss_Seg*)(idtr_->GDT_IDT_BaseAddress+idtr_->GDT_IDT_Size+1);  //괗호 씌우니까 이제야 같아짐 뭐지  
	
	Set_Dcpt((Dcpt*)(gdtr_->GDT_IDT_BaseAddress),0x0,0x0,0x0,0x0);
	Set_Dcpt((Dcpt*)(gdtr_->GDT_IDT_BaseAddress+8),0x00000000,0x0000,0x9A,0x20);
	Set_Dcpt((Dcpt*)(gdtr_->GDT_IDT_BaseAddress+16),0x00000000,0x0000,0x92,0x20);
	Set_TSSDcpt((Tss_Dcpt*)(gdtr_->GDT_IDT_BaseAddress+24),
	idtr_->GDT_IDT_BaseAddress+COUNT_ROUTINE*sizeof(IDT_Dcpt),(sizeof(Tss_Seg)-1),0x89,((sizeof(Tss_Seg)-1)>>16)|0x80); //여기도 이상,tr_이 idt 다음에 있으므로 baseaddress를 다르게 설정해줘야됨 // 이 함수에 문제 있는거일수도 
	
	for(i=0; i<sizeof(Tss_Seg); i++)
		*((char *)(tr_+i))=0; 
	tr_->IST[0]=IST_START+IST_SIZE;
	tr_->BaseAddress_BitMap=0xFFFF;
	
	for(i=0; i<48; i++)
	{  
		Set_Idt((IDT_Dcpt*)(idtr_->GDT_IDT_BaseAddress+sizeof(IDT_Dcpt)*i),(unsigned long long int)g_ISR_Vector[i],IA_32e_CodeDescriptor,1,0x8E);
	}
	for(i=48; i<COUNT_ROUTINE; i++)
	{
		g_ISR_Vector[i]=ISR_Intterupt_Other;
		Set_Idt((IDT_Dcpt*)(idtr_->GDT_IDT_BaseAddress+sizeof(IDT_Dcpt)*i),(unsigned long long int)g_ISR_Vector[i],IA_32e_CodeDescriptor,1,0x8E);
	}
	LGDT_LIDT_TS((unsigned long long int)gdtr_,(unsigned long long int)idtr_,(unsigned short)Tss_Descriptor); 
}

void Set_TSSDcpt(Tss_Dcpt* tss_dcpt_,unsigned long long int BaseAddress,unsigned short Limit,unsigned char Flag_1,unsigned char Limit_Flag_2)
{
	tss_dcpt_->Limit_1=Limit;
	tss_dcpt_->BaseAddress_1=BaseAddress&0xFFFF;
	tss_dcpt_->BaseAddress_2=(BaseAddress>>16)&0xFF;
	tss_dcpt_->Flag_1=Flag_1;
	tss_dcpt_->Limit_Flag_2=Limit_Flag_2;
	tss_dcpt_->BaseAddress_3=(BaseAddress>>24)&0xFF;
	tss_dcpt_->BaseAddress_4=BaseAddress>>32;
	tss_dcpt_->Reserved=0;
}

void Set_Dcpt(Dcpt* dcpt_,unsigned int BaseAddress,unsigned short Limit,unsigned char Flag_1,unsigned char Limit_Flag_2)
{
	dcpt_->Limit_1=Limit;
	dcpt_->BaseAddress_1=BaseAddress&0xFFFF;
	dcpt_->BaseAddress_2=(BaseAddress>>16)&0xFF;
	dcpt_->Flag_1=Flag_1;
	dcpt_->Limit_Flag_2=Limit_Flag_2;
	dcpt_->BaseAddress_3=(BaseAddress>>24)&0xFF;
}

void Set_Idt(IDT_Dcpt* idt_dcpt_,unsigned long long int HandlerOffset,unsigned short Selector,unsigned char Ist_,unsigned char Flag_)
{ 
	idt_dcpt_->HandlerOffset_1=HandlerOffset&0xFFFF;
	idt_dcpt_->Selector=Selector;
	idt_dcpt_->Ist_0=Ist_&0x07;
	idt_dcpt_->Flags=Flag_&0xEF;
	idt_dcpt_->HandlerOffset_2=(HandlerOffset>>16)&0xFFFF;
	idt_dcpt_->HandlerOffset_3=(HandlerOffset>>32);
	idt_dcpt_->Reserved=0;
}

void LGDT_LIDT_TS(unsigned long long int _GDTR,unsigned long long int _IDTR,unsigned short _TR)
{
	__asm__ __volatile__
	(
		"lgdt [rdi];"
		"lidt [rsi];"
		"ltr dx;"
	);
}
