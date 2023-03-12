#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_

#define DCPT_COUNT 3
#define TSS_DCPT_COUNT 1
#define PAGETABLE_END 0x142000
#define COUNT_ROUTINE 256

#define NullDescriptor 0x00
#define IA_32e_CodeDescriptor 0x08
#define IA_32e_DataDescriptor 0x10
#define Tss_Descriptor 0x18

#define IST_START 0x700000
#define IST_SIZE 0x100000

#pragma pack( push, 1 )

typedef struct struct_IDTR_GDTR
{
	unsigned short GDT_IDT_Size;
	unsigned long long int GDT_IDT_BaseAddress;
}IDTR,GDTR;

typedef struct struct_Descriptor
{
	unsigned short Limit_1;
	unsigned short BaseAddress_1;
	unsigned char BaseAddress_2;
	unsigned char Flag_1;
	unsigned char Limit_Flag_2;
	unsigned char BaseAddress_3;
}Dcpt;

typedef struct struct_TSS_Descriptor
{
	unsigned short Limit_1;
    unsigned short BaseAddress_1;
    unsigned char BaseAddress_2;
    // 4비트 Type, 1비트 0, 2비트 DPL, 1비트 P
    unsigned char Flag_1;
    // 4비트 Segment Limit, 1비트 AVL, 0, 0, G
    unsigned char Limit_Flag_2;
    unsigned char BaseAddress_3;
    unsigned int BaseAddress_4;
    unsigned int Reserved;
}Tss_Dcpt;

typedef struct struct_Tss_Segment
{
	unsigned int Reserved_1;
	unsigned long long int _RSP[3];
	unsigned long long int Reserved_2;
	unsigned long long int IST[7];
	unsigned long long int Reserved_3;
	unsigned short Reserved_4;
	unsigned short BaseAddress_BitMap;
}Tss_Seg;

typedef struct struct_IDT_Descriptor
{
	unsigned short HandlerOffset_1;
	unsigned short Selector;
	unsigned char Ist_0;
	unsigned char Flags;
	unsigned short HandlerOffset_2;
	unsigned int HandlerOffset_3;
	unsigned int Reserved; 
}IDT_Dcpt;

#pragma pack ( pop )

void SetTables_GDT_IDT_TSS(void);
void Set_TSSDcpt(Tss_Dcpt* tss_dcpt_,unsigned long long int BaseAddress,unsigned short Limit,unsigned char Flag_1,unsigned char Limit_Flag_2);
void Set_Dcpt(Dcpt* dcpt_,unsigned int BaseAddress,unsigned short Limit,unsigned char Flag_1,unsigned char Limit_Flag_2);
void Set_Idt(IDT_Dcpt* idt_dcpt_,unsigned long long int HandlerOffset,unsigned short Selector,unsigned char Ist_,unsigned char Flag_);
void LGDT_LIDT_TS(unsigned long long int _GDTR,unsigned long long int _IDTR,unsigned short _TR);

#endif
