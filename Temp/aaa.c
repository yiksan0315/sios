#include "aaa.h"
#include "IO.h"

//==============================================================================

//  GDT �� TSS

//==============================================================================



/**

 *  GDT ���̺��� �ʱ�ȭ

 */

 void kMemSet( void* pvDestination, BYTE bData, int iSize );
 
 void kMemSet( void* pvDestination, BYTE bData, int iSize )
{

    int i;

    

    for( i = 0 ; i < iSize ; i++ )

    {

        ( ( char* ) pvDestination )[ i ] = bData;

    }

}

void kInitializeGDTTableAndTSS( void )
{

    GDTR* pstGDTR;

    GDTENTRY8* pstEntry;

    TSSSEGMENT* pstTSS;

    int i;

    

    // GDTR ����

    pstGDTR = ( GDTR* ) GDTR_STARTADDRESS;

    pstEntry = ( GDTENTRY8* ) ( GDTR_STARTADDRESS + sizeof( GDTR ) );

    pstGDTR->wLimit = GDT_TABLESIZE - 1;

    pstGDTR->qwBaseAddress = ( QWORD ) pstEntry;

    // TSS ���� ����

    pstTSS = ( TSSSEGMENT* ) ( ( QWORD ) pstEntry + GDT_TABLESIZE );
    
    Set_Dcpt((Dcpt*)(pstGDTR->qwBaseAddress),0x0,0x0,0x0,0x0);
	Set_Dcpt((Dcpt*)(pstGDTR->qwBaseAddress+8),0x00000000,0x0000,0x9A,0x20);
	Set_Dcpt((Dcpt*)(pstGDTR->qwBaseAddress+16),0x00000000,0x0000,0x92,0x20);
	Set_TSSDcpt((Tss_Dcpt*)(pstGDTR->qwBaseAddress+24),
	( unsigned long long int )pstTSS,(sizeof(TSSSEGMENT)-1)&0xFFFF,0x89,((sizeof(TSSSEGMENT)-1)>>16)|0x80);
	printk_Num(((sizeof(TSSSEGMENT)-1)>>16)|0x80,2);
	printk_Num(GDT_FLAGS_UPPER_TSS|( ( (sizeof( TSSSEGMENT ) - 1) >> 16 ) & 0xFF ),3);
	printk_Num((unsigned long int)(pstGDTR->qwBaseAddress+24),3);
	printk_Num((unsigned long int)&( pstEntry[ 3 ] ),4);

//pstGDTR->qwBaseAddress+(pstGDTR->wLimit+1)
    // NULL, 64��Ʈ Code/Data, TSS�� ���� �� 4���� ���׸�Ʈ�� �����Ѵ�.

/*    kSetGDTEntry8( &( pstEntry[ 0 ] ), 0, 0, 0, 0, 0 );

    kSetGDTEntry8( &( pstEntry[ 1 ] ), 0, 0xFFFFF, GDT_FLAGS_UPPER_CODE, 
            GDT_FLAGS_LOWER_KERNELCODE, GDT_TYPE_CODE );

    kSetGDTEntry8( &( pstEntry[ 2 ] ), 0, 0xFFFFF, GDT_FLAGS_UPPER_DATA,
            GDT_FLAGS_LOWER_KERNELDATA, GDT_TYPE_DATA );

    kSetGDTEntry16( ( GDTENTRY16* ) &( pstEntry[ 3 ] ), ( QWORD ) pstTSS, 
            sizeof( TSSSEGMENT ) - 1, GDT_FLAGS_UPPER_TSS, GDT_FLAGS_LOWER_TSS, 
            GDT_TYPE_TSS ); */

    

    // TSS �ʱ�ȭ GDT ���� ������ �����

    kInitializeTSSSegment( pstTSS );

}



/**

 *  8����Ʈ ũ���� GDT ��Ʈ���� ���� ����

 *      �ڵ�� ������ ���׸�Ʈ ��ũ���͸� �����ϴµ� ���

 */

void kSetGDTEntry8( GDTENTRY8* pstEntry, DWORD dwBaseAddress, DWORD dwLimit,
        BYTE bUpperFlags, BYTE bLowerFlags, BYTE bType )
{

    pstEntry->wLowerLimit = dwLimit & 0xFFFF;

    pstEntry->wLowerBaseAddress = dwBaseAddress & 0xFFFF;

    pstEntry->bUpperBaseAddress1 = ( dwBaseAddress >> 16 ) & 0xFF;

    pstEntry->bTypeAndLowerFlag = bLowerFlags | bType;

    pstEntry->bUpperLimitAndUpperFlag = ( ( dwLimit >> 16 ) & 0xFF ) | 
        bUpperFlags;

    pstEntry->bUpperBaseAddress2 = ( dwBaseAddress >> 24 ) & 0xFF;

}



/**

 *  16����Ʈ ũ���� GDT ��Ʈ���� ���� ����

 *      TSS ���׸�Ʈ ��ũ���͸� �����ϴµ� ���

 */

void kSetGDTEntry16( GDTENTRY16* pstEntry, QWORD qwBaseAddress, DWORD dwLimit,
        BYTE bUpperFlags, BYTE bLowerFlags, BYTE bType )
{

    pstEntry->wLowerLimit = dwLimit & 0xFFFF;

    pstEntry->wLowerBaseAddress = qwBaseAddress & 0xFFFF;

    pstEntry->bMiddleBaseAddress1 = ( qwBaseAddress >> 16 ) & 0xFF;

    pstEntry->bTypeAndLowerFlag = bLowerFlags | bType;

    pstEntry->bUpperLimitAndUpperFlag = ( ( dwLimit >> 16 ) & 0xFF ) | 
        bUpperFlags;

    pstEntry->bMiddleBaseAddress2 = ( qwBaseAddress >> 24 ) & 0xFF;

    pstEntry->dwUpperBaseAddress = qwBaseAddress >> 32;

    pstEntry->dwReserved = 0;

}



/**

 *  TSS ���׸�Ʈ�� ������ �ʱ�ȭ

 */

void kInitializeTSSSegment( TSSSEGMENT* pstTSS )
{

    kMemSet( pstTSS, 0, sizeof( TSSSEGMENT ) );

    pstTSS->qwIST[ 0 ] = IST_STARTADDRESS + IST_SIZE;

    // IO �� TSS�� limit ������ ũ�� ���������ν� IO Map�� ������� �ʵ��� ��

    pstTSS->wIOMapBaseAddress = 0xFFFF;

}





//==============================================================================

//  IDT

//==============================================================================

/**

 *  IDT ���̺��� �ʱ�ȭ

 */

void kInitializeIDTTables( void )
{

    IDTR* pstIDTR;

    IDTENTRY* pstEntry;

    int i;

        

    // IDTR�� ���� ��巹��

    pstIDTR = ( IDTR* ) IDTR_STARTADDRESS;

    // IDT ���̺��� ���� ����

    pstEntry = ( IDTENTRY* ) ( IDTR_STARTADDRESS + sizeof( IDTR ) );

    pstIDTR->qwBaseAddress = ( QWORD ) pstEntry;

    pstIDTR->wLimit = IDT_TABLESIZE - 1;

    

    // 0~99���� ���͸� ��� DummyHandler�� ����

    for( i = 0 ; i < IDT_MAXENTRYCOUNT ; i++ )
    {
        kSetIDTEntry( &( pstEntry[ i ] ), kDummyHandler, 0x08, IDT_FLAGS_IST1, 
            IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    }

}



/**

 *  IDT ����Ʈ ��ũ���Ϳ� ���� ����

 */

void kSetIDTEntry( IDTENTRY* pstEntry, void* pvHandler, WORD wSelector, 
        BYTE bIST, BYTE bFlags, BYTE bType )
{

    pstEntry->wLowerBaseAddress = ( QWORD ) pvHandler & 0xFFFF;

    pstEntry->wSegmentSelector = wSelector;

    pstEntry->bIST = bIST & 0x3;

    pstEntry->bTypeAndFlags = bType | bFlags;

    pstEntry->wMiddleBaseAddress = ( ( QWORD ) pvHandler >> 16 ) & 0xFFFF;

    pstEntry->dwUpperBaseAddress = ( QWORD ) pvHandler >> 32;

    pstEntry->dwReserved = 0;

}

void Set_TSSDcpt(Tss_Dcpt* tss_dcpt_,unsigned long long int BaseAddress,unsigned short Limit,unsigned char Flag_1,unsigned char Limit_Flag_2)
{
	tss_dcpt_->Limit_1=Limit;
	tss_dcpt_->BaseAddress_1=BaseAddress&0x000000FFFF;
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
	dcpt_->BaseAddress_1=BaseAddress&0x0000FFFF;
	dcpt_->BaseAddress_2=BaseAddress&0x00FF0000;
	dcpt_->Flag_1=Flag_1;
	dcpt_->Limit_Flag_2=Limit_Flag_2;
	dcpt_->BaseAddress_3=BaseAddress&0xFF000000;
}



/**

 *  �ӽ� ���� �Ǵ� ���ͷ�Ʈ �ڵ鷯

 */

void kDummyHandler( void )

{

    printk("====================================================",0,0,_MAGENTA);

    while( 1 ) ;

}
