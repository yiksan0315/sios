#include "Set_HardWare.h"
#include "Assembly_Func.h"

void Init_PIC(void)
{
	WritePort_Out(MASTER_ICW1,0x11);
	WritePort_Out(SLAVE_ICW1,0x11);
	
	WritePort_Out(MASTER_ICW2,0x20);
	WritePort_Out(SLAVE_ICW2,0x28);
	
	WritePort_Out(MASTER_ICW3,0x04);
	WritePort_Out(SLAVE_ICW3,0x02);
	
	WritePort_Out(MASTER_ICW4,0x01);
	WritePort_Out(SLAVE_ICW4,0x01);
}

void Send_EOI_PIC(unsigned char IRQ)
{

	WritePort_Out(MASTER_OCW2,PIC_EOI);
		if(IRQ>=0x28)
		WritePort_Out(SLAVE_OCW2,PIC_EOI);
}

void Set_Mask_IRQ(unsigned short IRQ)
{
	WritePort_Out(MASTER_OCW1,IRQ);
	WritePort_Out(SLAVE_OCW1,IRQ>>8);
}

void Set_PIT(unsigned short Count,unsigned char Loop)
{
	WritePort_Out(PIT_CONTROL,
	PIT_CR_COUTER0|ACCESS_LOW_HIGH|MODE0|BIN_MODE);

	if(Loop)
	{
		WritePort_Out(PIT_CONTROL,
		PIT_CR_COUTER0|ACCESS_LOW_HIGH|MODE2|BIN_MODE);
	}
	WritePort_Out(PIT_COUTER0,Count);
	WritePort_Out(PIT_COUTER0,Count>>8);
}

unsigned short ReadCounter_0(void)
{
	unsigned short Now_Count=0x0;
	WritePort_Out(PIT_CONTROL,LATCH);
	
	Now_Count|=ReadPort_In(PIT_CR_COUTER0)<<8;
	Now_Count|=ReadPort_In(PIT_CR_COUTER0);
	
	return Now_Count;
}
unsigned long long int rdtsc(void)
{
	__asm__ __volatile__(
		"push rdx;"
		"rdtsc;"
		"shl rdx,32;"
		"or rax,rdx;"
		
		"pop rdx;"
	);
}

unsigned char GetRTC(unsigned char Date_or_Time)
{
	unsigned char Temp=0;
	WritePort_Out(CMOS_PORT,Date_or_Time);
	Temp=ReadPort_In(CMOS_DATA_PORT);
	Temp=(Temp>>4)*10+(Temp&0x000F);
	return Temp;
	
}

