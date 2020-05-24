#ifndef _PAGE_H_
#define _PAGE_H_

#include "Data Types.h"

#define ENTRY_COUNT 512
#define PAGE_TABLE_SIZE 0x1000
#define PAGE_SIZE 0x200000 

typedef struct PageEntryStruct
{
  /*uchar Page_Present : 1;
	uchar Page_RW : 1;
	uchar Page_US : 1;
	uchar Page_PWT : 1;
	uchar Page_PCD : 1;
	uchar Page_Accessed : 1;
	uchar Page_Ignored_Dont_Modify_1 : 1;
	uchar Page_Reserved_1 : 1; //¹«Á¶°Ç 0 
	uchar Page_Ignored_Dont_Modify_2 : 4;
	uint Page_LowerBaseAddress : 20;
	uint Page_UpperBaseAddress : 8;
	uint Page_Reserved_2: 12;
	uint Page_Ignored_Dont_Modify_3 : 11;
	uchar Page_EXD : 1;*/
	uint LowerBaseAddress;
	uint UpperBaseAddress;
}*PML4T,*PDPT,*PDT,*PET;

/*typedef struct PageEntryStruct
{
	uchar Page_Present : 1;
	uchar Page_RW : 1;
	uchar Page_US : 1;
	uchar Page_PWT : 1;
	uchar Page_PCD : 1;
	uchar Page_Accessed : 1;
	uchar Page_Dirty : 1;
	uchar Page_Page_Size : 1; 
	uchar Page_Global : 1;
	uchar Page_Reserved_1 : 3;
	uchar Page_PAT : 1;
	uchar Page_Ignored_Dont_Modify_1 : 8;
	uint Page_LowerBaseAddress : 11;
	uint Page_UpperBaseAddress : 8;
	uint Page_Ignored_Dont_Modify_2 : 12;
	uint Page_Reserved_2 : 11;
	uchar Page_EXD : 1;
	uint LowerBaseAddress;
	uint UpperBaseAddress;
}*PDT;*/

void MakePageTable(void);

void SetEntry(PET PageEntry,uint UpperAddress,uint LowerAddress,uint UpperAttr,uint LowerAttr);

//void SetEntry_NonUse(PET PageEntry);

//void SetEntry_Default(PET PageEntry,uint UpperAddress,uint LowerAddress);

//void PDT_SetEntry_Default(PDT PageEntry,uint UpperAddress,uint LowerAddress);

#endif 
