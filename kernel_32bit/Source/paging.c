#include "Page.h"

void MakePageTable(void)
{
	int i,j;
	uint PageAddress=0;
	PML4T PML4_Table;
	PDPT PageDirPtr_Table;
	PDT PageDir_Table;
	
 	PML4_Table=(PML4T)0x100000;
 	SetEntry(PML4_Table,0x00,0x101000,0,0x3);
 	for(i=1; i<ENTRY_COUNT; i++)
 	{
 		SetEntry(&PML4_Table[i],0,0,0,0);
	}
	
	PageDirPtr_Table=(PDPT)0x101000;
	for(i=0; i<64; i++)
	{
		SetEntry(&PageDirPtr_Table[i],0x00,0x102000+i * PAGE_TABLE_SIZE,0,0x3);
	}
	for(i=64; i<ENTRY_COUNT; i++)
	{
		SetEntry(&PageDirPtr_Table[i],0,0,0,0);
	}
	
	PageDir_Table=(PDT)0x102000;
	for(i=0; i<ENTRY_COUNT*64; i++)
	{
		if(PageAddress<0x2000000)
		{
			SetEntry(&PageDir_Table[i],i>>(13-(PAGE_SIZE>>20)),PageAddress,0,0x1|0x2|0x80); 
		}
		else
		{
			SetEntry(&PageDir_Table[i],i>>(13-(PAGE_SIZE>>20)),PageAddress,0,0x1|0x2|0x80|0x4); 
		}
			PageAddress+=PAGE_SIZE;
		/*kSetPageEntryData( &( pstPDEntry[ i ] ), 
                ( i * ( PAGE_DEFAULTSIZE >> 20 ) ) >> 12, dwMappingAddress, 
                PAGE_FLAGS_DEFAULT | PAGE_FLAGS_PS, 0 );*/ 
                
        //32��Ʈ unsigned int ���� ǥ�� ������ �ִ� �ּҴ� 4GB �̰�, �� �̻��� �����÷ο�� �ٽ� 0���� �������� �� 
		//���� dwMappingAddress ���� 4GB(32bit) / 2MB(21bit) = 2Kb(,2048,11bit) 
		//�׸��� ������ �������� 2MB �� >>20 ������ ���� ���� �׻� 2, �׸��� >>12�� ������ ���� 1 �̻��Ϸ��� 2�� 12���� 4096(4KB), i�� 4096�̾����
		// �ٵ�  ������ �������� 2MB �� >>20 ������ ���� ���� �׻� 2�̹Ƿ� i�� 2048 �϶� i�� 4096�� �� 
		// ���� ������ ���͸� ���̺��� ��Ʈ���� 512��, ���̺��� ������ 64��, ���̺� �ϳ��� 2Mb * 512 �� 1GB �� ǥ�� ���� 
		//���� i�� 2048(*2 ->������ �������� 2MB �� >>20 ������ ���� �� 2 )�϶� i�� ������ ���͸� ���̺��� ��Ʈ�� ������ 512�� ������ 4, �� 4GB 
		// �̶� 32��Ʈ�� 4GB ������ ǥ�� �����ϴ� dwMappingAddress ������ �����÷ο�� 0�̵ǰ�, �̿����� ���� ���� �ּҿ� ���� �־� 4GB �̻��� ǥ��  
		
		//PDT		//    �鿩������� �Ӽ� ��Ʈ,21 ��Ʈ  
		/*0000 0000 001 | 0 0000 0000 0000 0000 0011      2Mb | Page_Present=1,Page_RW=1 
  		   1    2    3       4    5    6    7    8    */
	}
}

void SetEntry(PET PageEntry,uint UpperAddress,uint LowerAddress,uint UpperAttr,uint LowerAttr)
{
	PageEntry->LowerBaseAddress=LowerAddress|LowerAttr;
	PageEntry->UpperBaseAddress=(UpperAddress&0xFF)|UpperAttr;
}

/*void SetEntry_NonUse(PET PageEntry)
{
	PageEntry->Page_Present=0;
	PageEntry->Page_RW=0;
	PageEntry->Page_US=0;
	PageEntry->Page_PWT=0;
	PageEntry->Page_PCD=0;
	PageEntry->Page_Accessed=0;
	PageEntry->Page_Ignored_Dont_Modify_1=0;
	PageEntry->Page_Reserved_1=0;
	PageEntry->Page_Ignored_Dont_Modify_2=0;
	PageEntry->Page_LowerBaseAddress=0;
	PageEntry->Page_UpperBaseAddress=0;
	PageEntry->Page_Reserved_2=0;
	PageEntry->Page_Ignored_Dont_Modify_3=0;
	PageEntry->Page_EXD=0;
	PageEntry->LowerBaseAddress=0;
	PageEntry->UpperBaseAddress=0;
}

void SetEntry_Default(PET PageEntry,uint UpperAddress,uint LowerAddress)
{
	PageEntry->Page_Present=1;
	PageEntry->Page_RW=1;
	PageEntry->Page_US=0;
	PageEntry->Page_PWT=0;
	PageEntry->Page_PCD=0;
	PageEntry->Page_Accessed=0;
	PageEntry->Page_Ignored_Dont_Modify_1=0;
	PageEntry->Page_Reserved_1=0;
	PageEntry->Page_Ignored_Dont_Modify_2=0;
	PageEntry->Page_LowerBaseAddress=LowerAddress;//>>12;
	PageEntry->Page_UpperBaseAddress=UpperAddress;
	PageEntry->Page_Reserved_2=0;
	PageEntry->Page_Ignored_Dont_Modify_3=0;
	PageEntry->Page_EXD=0;
	PageEntry->LowerBaseAddress=LowerAddress | 3;
    PageEntry->UpperBaseAddress=( UpperAddress & 0xFF );
}

void PDT_SetEntry_Default(PDT PageEntry,uint UpperAddress,uint LowerAddress)
{
	PageEntry->Page_Present=1;
	PageEntry->Page_RW=1;
	PageEntry->Page_US=0;
	PageEntry->Page_PWT=0;
	PageEntry->Page_PCD=0;
	PageEntry->Page_Accessed=0;
	PageEntry->Page_Dirty=0;
	PageEntry->Page_Page_Size=1;
	PageEntry->Page_Global=0;
	PageEntry->Page_Reserved_1=0;
	PageEntry->Page_PAT=0;
	PageEntry->Page_Ignored_Dont_Modify_1=0;
	PageEntry->Page_LowerBaseAddress=LowerAddress;//>>21;
	PageEntry->Page_UpperBaseAddress=UpperAddress;
	PageEntry->Page_Ignored_Dont_Modify_2=0;
	PageEntry->Page_Reserved_2=0;
	PageEntry->Page_EXD=0;
	PageEntry->LowerBaseAddress=LowerAddress | 0x3 |0x80;
    PageEntry->UpperBaseAddress=( UpperAddress & 0xFF );
}*/

/*
typedef struct PageEntryStruct
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
	uchar Page_Reserved_1: 3;
	uchar Page_PAT : 1;
	uchar Page_Ignored_Dont_Modify_1 : 8;
	uint Page_LowerBaseAddress : 11;
	uint Page_UpperBaseAddress : 8;
	uint Page_Ignored_Dont_Modify_2 : 12;
	uint Page_Reserved_2 : 11;
	uchar Page_EXD : 1;
}*PDT; */

/*
typedef struct PageEntryStruct
{
	uchar Page_Present : 1;
	uchar Page_RW : 1;
	uchar Page_US : 1;
	uchar Page_PWT : 1;
	uchar Page_PCD : 1;
	uchar Page_Accessed : 1;
	uchar Page_Ignored_Dont_Modify_1 : 1;
	uchar Page_Reserved_1 : 1; //������ 0 
	uchar Page_Ignored_Dont_Modify_2 : 4;
	uchar Page_LowerBaseAddress : 20;
	uint Page_UpperBaseAddress : 8;
	uint Page_Reserved_2: 12;
	uint Page_Ignored_Dont_Modify_3 : 11;
	uchar Page_EXD : 1;
}*PML4T,*PDPT,*PET; */

