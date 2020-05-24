#include "FileSystem.h"
#include "IO.h"
#include "string.h"

superblock Super;

char pwd_Path[1024]={"root"};

bool Format(void)
{
	if(CheckIsFmat())
	{
		ReadSector(PATA_PORT_1,true,FILESYSTEM_OFFSET+1,SECTOR/SECTOR,&Super);
		return true;
	}
	ulld i;
	ulld DiskSize=return_diskinfo()->TotalSectors;
	if(DiskSize<=(1024*1024*128)/SECTOR)  //디스크의 크기는 반드시 8Mb배수 여야함 
		return false;
	ulld KernelOff=FILESYSTEM_OFFSET;
	
	SetSuperBlock(&Super);
	uchar BitMap_block[(Super.block_count)/BYTE_BIT]; SetBitmap_block(BitMap_block,&Super);
	uchar BitMap_inode[(Super.inode_count)/BYTE_BIT];
	inode inodetable[Super.inode_count];

	KernelOff++;
	WriteSector(PATA_PORT_1,true,KernelOff,1,&Super);
	
	KernelOff++;
	WriteSector(PATA_PORT_1,true,KernelOff,
	((Super.block_count)/BYTE_BIT/SECTOR),BitMap_block);

	KernelOff+=((Super.block_count)/BYTE_BIT/SECTOR);
	WriteSector(PATA_PORT_1,true,KernelOff,
	((Super.inode_count)/BYTE_BIT/SECTOR),BitMap_inode);
	
	KernelOff+=((Super.inode_count)/BYTE_BIT/SECTOR);
	WriteSector(PATA_PORT_1,true,KernelOff,Super.inode_count/8,inodetable);
	
	Alloc_inode();
	MakeRootDir();
	SetIsFmat();
	return true;
}


bool CheckIsFmat(void)
{
	uchar Bootstrap[512];
	ReadSector(PATA_PORT_1,true,0,SECTOR/SECTOR,Bootstrap);
	return Bootstrap[ISFORMATED];
}

uint Alloc_Block(void)
{
	uint i,j,Bit,Count=0,Block=0;
	uchar Block_bitmap[BLOCK_SIZE];
	while(Count<((Super.block_count)/BYTE_BIT/BLOCK_SIZE))
	{
		ReadSector(PATA_PORT_1,true,Super.block_bitmap+Count*2,BLOCK_SIZE/SECTOR,Block_bitmap);
		for(i=0; i<BLOCK_SIZE; i++)
		{
			Bit=1;
			for(j=0; j<BYTE_BIT; j++)
			{
				if(!(Block_bitmap[i]&Bit))
				{
					Block_bitmap[i]|=Bit;
					WriteSector(PATA_PORT_1,true,Super.block_bitmap+Count*2,BLOCK_SIZE/SECTOR,Block_bitmap);
					return Block;
				}
				if(++Block>=Super.block_count)
				{
					return NIL;
				}
				Bit=Bit<<1;
			}
		}
		Count++;
	}
	return NIL;
}

uint Alloc_inode(void)
{
	uint i,j,Bit,Count=0,inode=0;
	uchar inode_bitmap[BLOCK_SIZE];
	while(Count<((Super.inode_count)/BYTE_BIT/BLOCK_SIZE))
	{
		ReadSector(PATA_PORT_1,true,Super.inode_bitmap+Count*2,BLOCK_SIZE/SECTOR,inode_bitmap);
		for(i=0; i<BLOCK_SIZE; i++)
		{
			Bit=1;
			for(j=0; j<BYTE_BIT; j++)
			{
				if(!(inode_bitmap[i]&Bit))
				{
					inode_bitmap[i]|=Bit;
					WriteSector(PATA_PORT_1,true,Super.inode_bitmap+Count*2,BLOCK_SIZE/SECTOR,inode_bitmap);
					return inode;
				}
				if(++inode>=Super.inode_count)
				{
					return INODE_ENTRY_BIT;
				}
				Bit=Bit<<1;
			}
		}
		Count++;
	}
	return INODE_ENTRY_BIT;
}

bool Free_Block(uint block)
{
	if(block<0||block>=Super.block_count)
		return false;
	uint i,Byte,Bit,Block_Bit;
	uchar Bit_bitmap=1;
	uchar Block_bitmap[BLOCK_SIZE];
	
	Block_Bit=block/8192;
	Byte=block/BYTE_BIT;
	Bit=block%BYTE_BIT;
	
	ReadSector(PATA_PORT_1,true,Super.block_bitmap+Block_Bit*2,BLOCK_SIZE/SECTOR,Block_bitmap);
	Bit_bitmap=1<<Bit;
	Block_bitmap[Byte]&=(~Bit_bitmap);
	WriteSector(PATA_PORT_1,true,Super.block_bitmap+Block_Bit*2,BLOCK_SIZE/SECTOR,Block_bitmap);
	
	for(i=0; i<1024; i++)	Block_bitmap[i]=0x00;
	WriteSector(PATA_PORT_1,true,Super.block_table+block,BLOCK_SIZE/SECTOR,Block_bitmap);

	return true;
}

bool Free_inode(uint inode)
{
	if(inode<0||inode>=Super.inode_count)
		return false;
	uint i,Byte,Bit,Block_Bit;
	uchar Bit_bitmap=1;
	uchar inode_bitmap[BLOCK_SIZE];
	uchar inode_table[SECTOR];
	
	Block_Bit=inode/8192;
	Byte=inode/BYTE_BIT;
	Bit=inode%BYTE_BIT;
	
	ReadSector(PATA_PORT_1,true,Super.inode_bitmap+Block_Bit*2,BLOCK_SIZE/SECTOR,inode_bitmap);
	Bit_bitmap=1<<Bit;
	inode_bitmap[Byte]&=(~Bit_bitmap);
	WriteSector(PATA_PORT_1,true,Super.inode_bitmap+Block_Bit*2,BLOCK_SIZE/SECTOR,inode_bitmap);
	
	ReadSector(PATA_PORT_1,true,Super.inode_table+inode/4,SECTOR/SECTOR,inode_table);
	
	for(i=(inode%4)*sizeof(inode); i<sizeof(inode); i++)	inode_table[i]=0x00;
	WriteSector(PATA_PORT_1,true,Super.inode_table+inode/4,SECTOR/SECTOR,inode_table);
	return true;
}


void MakeRootDir(void)
{
	int i;
	uint _inode=Alloc_inode();
	inode inode_table[4];
	Dir_Entry entry_root[ENTRY_COUNT]={0,};
	ReadSector(PATA_PORT_1,true,Super.inode_table,1,inode_table);
	
	inode_table[_inode].mode=MODE_DIR;
	inode_table[_inode].links_count=12;
	inode_table[_inode].size=0;
	for(i=0; i<12; i++)
	{
		inode_table[_inode].block[i]=Alloc_Block();
	}
	WriteSector(PATA_PORT_1,true,Super.inode_table,SECTOR/SECTOR,inode_table);
	
	for(i=0; i<12; i++)
	{
		entry_root[0].file_type=0x00|0x01;	
		entry_root[0].inode=INODE_ENTRY_BIT;
		WriteSector(PATA_PORT_1,true,Super.block_table+inode_table[_inode].block[i],BLOCK_SIZE/SECTOR,entry_root);
	}
}

ERROR CreateFile(const char *_FileName)
{
	if(strlen(_FileName)>=57||strlen(_FileName)<=0)
	{
		return NAME_OVER;
	 } 
	uint Index,Block_Index;
	Find_withFilename(_FileName,&Block_Index,&Index);
	if(Index!=INODE_ENTRY_BIT||Block_Index!=BLOCK_INDEX_NIL)
	{
		return SAME_NAME;
	}
	Index=0;Block_Index=0;
	inode inode_table[4];
	ReadSector(PATA_PORT_1,true,Super.inode_table+*(Return_pwd_inode())/4,SECTOR/SECTOR,inode_table);
	Dir_Entry entry_dir[ENTRY_COUNT];
	while(Block_Index<12)
	{
		ReadSector(PATA_PORT_1,true,Super.block_table+inode_table[*(Return_pwd_inode())%4].block[Block_Index],
		BLOCK_SIZE/SECTOR,entry_dir);
		if(!Check_isFull(entry_dir))
		{
			Index=Return_EmptyIndex(entry_dir);
			entry_dir[Index].inode=Alloc_inode();
			if(entry_dir[Index].inode==INODE_ENTRY_BIT)
				return NOT_ENOUGH;
			entry_dir[Index].file_type=FILETYPE_FILE;
			strcpy(entry_dir[Index].name,_FileName);
			entry_dir[Index].name_len=strlen(_FileName);
			
			WriteSector(PATA_PORT_1,true,Super.block_table+inode_table[*(Return_pwd_inode())%4].block[Block_Index],
			BLOCK_SIZE/SECTOR,entry_dir);
			
			ReadSector(PATA_PORT_1,true,Super.inode_table+entry_dir[Index].inode/4,SECTOR/SECTOR,inode_table);
			inode_table[entry_dir[Index].inode%4].links_count=0;
			inode_table[entry_dir[Index].inode%4].mode=MODE_FILE;
			inode_table[entry_dir[Index].inode%4].size=0;
			
			WriteSector(PATA_PORT_1,true,Super.inode_table+entry_dir[Index].inode/4,SECTOR/SECTOR,inode_table);
			return SUCCESS;
		}
		Block_Index++;
	}
	return NOT_ENOUGH;
}

ERROR DeleteFile(const char *_FileName)// 앞에 있는 파일들이 사라지면 탐색이 안됨  
{
	uint i,Bit_bitmap; 
	uint Index,Block_Index,file_inode,inode_pwd=*Return_pwd_inode();
	Find_withFilename(_FileName,&Block_Index,&Index);
	if(Index==INODE_ENTRY_BIT||Block_Index==BLOCK_INDEX_NIL)
	{
		return NO_FILE;
	}
	inode inode_table[4];
	Dir_Entry entry_dir[ENTRY_COUNT];
	ReadSector(PATA_PORT_1,true,Super.inode_table+inode_pwd/4,SECTOR/SECTOR,inode_table);
	ReadSector(PATA_PORT_1,true,Super.block_table+inode_table[inode_pwd%4].block[Block_Index],
	BLOCK_SIZE/SECTOR,entry_dir);
	
	if(entry_dir[Index].file_type&FILETYPE_DIR)
	{
		return NOT_FILE;
	}
	
	file_inode=entry_dir[Index].inode;
	
	entry_dir[Index].file_type&=0x00; //file entry 초기화 
	for(i=0; i<entry_dir[Index].name_len; i++)
		 entry_dir[Index].name[i]=0x00;
	entry_dir[Index].name[i]=0x00;
	entry_dir[Index].name_len=0;
	entry_dir[Index].inode=INODE_ENTRY_BIT;

			//file entry 0번째에서 비트 제거  
	Bit_bitmap=1<<Index;
	entry_dir[0].file_type&=(~Bit_bitmap);
			
	WriteSector(PATA_PORT_1,true,Super.block_table+inode_table[inode_pwd%4].block[Block_Index],
	BLOCK_SIZE/SECTOR,entry_dir);
			
	ReadSector(PATA_PORT_1,true,Super.inode_table+file_inode/4,SECTOR/SECTOR,inode_table);
	for(i=0; i<inode_table[file_inode%4].links_count; i++)	Free_Block(inode_table[file_inode%4].block[i]);
															Free_inode(file_inode);
	return SUCCESS;
}


ERROR MakeDir(const char *_DirName)
{
	if(strlen(_DirName)>=57||strlen(_DirName)<=0)
	{
		return NAME_OVER;
	} 
	int i;
	uint Index,Block_Index;
	Find_withFilename(_DirName,&Block_Index,&Index);
	if(Index!=INODE_ENTRY_BIT||Block_Index!=BLOCK_INDEX_NIL)
	{
		return SAME_NAME;
	}
	Index=0;Block_Index=0;
	inode inode_table[4];
	ReadSector(PATA_PORT_1,true,Super.inode_table+*(Return_pwd_inode())/4,SECTOR/SECTOR,inode_table);
	Dir_Entry entry_dir[ENTRY_COUNT];
	while(Block_Index<12)
	{
		ReadSector(PATA_PORT_1,true,Super.block_table+inode_table[*(Return_pwd_inode())%4].block[Block_Index],
		BLOCK_SIZE/SECTOR,entry_dir);
		if(!Check_isFull(entry_dir))
		{
			Index=Return_EmptyIndex(entry_dir);
			entry_dir[Index].inode=Alloc_inode();
			if(entry_dir[Index].inode==INODE_ENTRY_BIT)
				return NOT_ENOUGH;
			entry_dir[Index].file_type=FILETYPE_DIR;
			strcpy(entry_dir[Index].name,_DirName);
			entry_dir[Index].name_len=strlen(_DirName);
			
			WriteSector(PATA_PORT_1,true,Super.block_table+inode_table[*(Return_pwd_inode())%4].block[Block_Index],
			BLOCK_SIZE/SECTOR,entry_dir);
			
			ReadSector(PATA_PORT_1,true,Super.inode_table+entry_dir[Index].inode/4,SECTOR/SECTOR,inode_table);
			
			Dir_Entry entry_dir_make[ENTRY_COUNT]={0,};
			entry_dir_make[0].file_type=0x00|0x01;	
			entry_dir_make[0].inode=INODE_ENTRY_BIT;
			// 현재디렉토리랑 부모디렉토리 추가  
			for(i=0; i<12; i++)
			{
				inode_table[entry_dir[Index].inode%4].block[i]=Alloc_Block();
				if(inode_table[entry_dir[Index].inode%4].block[i]==NIL)
				{
					Free_inode(entry_dir[Index].inode);
					for(i; i>=0; i--)
						Free_Block(inode_table[entry_dir[Index].inode%4].block[i]);
					return NOT_ENOUGH;
				}
				WriteSector(PATA_PORT_1,true,Super.block_table+inode_table[entry_dir[Index].inode%4].block[i],
				BLOCK_SIZE/SECTOR,entry_dir_make);
			}
			inode_table[entry_dir[Index].inode%4].links_count=12;
			inode_table[entry_dir[Index].inode%4].mode=MODE_DIR;
			inode_table[entry_dir[Index].inode%4].size=0;
			
			WriteSector(PATA_PORT_1,true,Super.inode_table+entry_dir[Index].inode/4,SECTOR/SECTOR,inode_table);
			return SUCCESS;
		}
		Block_Index++;
	}
	return NOT_ENOUGH;
}

ERROR RemoveDir(const char *_DirName)
{
	uint i,Bit_bitmap; 
	uint Index,Block_Index,file_inode,inode_pwd=*Return_pwd_inode();
	Find_withFilename(_DirName,&Block_Index,&Index);
	if(Index==INODE_ENTRY_BIT||Block_Index==BLOCK_INDEX_NIL)
	{
		return NO_FILE;
	}
	inode inode_table[4];
	inode inode_table_remove[4];
	Dir_Entry entry_dir[ENTRY_COUNT];
	Dir_Entry entry_dir_remove[ENTRY_COUNT];
	ReadSector(PATA_PORT_1,true,Super.inode_table+inode_pwd/4,SECTOR/SECTOR,inode_table);
	ReadSector(PATA_PORT_1,true,Super.block_table+inode_table[inode_pwd%4].block[Block_Index],
	BLOCK_SIZE/SECTOR,entry_dir);
	
	if(entry_dir[Index].file_type&FILETYPE_FILE)
	{
		return NOT_FILE;
	}
	else if(entry_dir[Index].file_type&FILETYPE_DISRM) 
	{
		return NOT_ENOUGH; //.. 혹은 . 
	} 
	file_inode=entry_dir[Index].inode;
	ReadSector(PATA_PORT_1,true,Super.inode_table+file_inode/4,SECTOR/SECTOR,inode_table_remove);
	ReadSector(PATA_PORT_1,true,Super.block_table+inode_table_remove[file_inode%4].block[0],
	BLOCK_SIZE/SECTOR,entry_dir_remove);
	if(entry_dir_remove[0].file_type&0xF8)
		return NAME_OVER;
	for(i=1; i<12; i++)
	{
		if(entry_dir_remove[0].file_type&0xFE)
			return NAME_OVER;
	}
	
	entry_dir[Index].file_type&=0x00; //file entry 초기화 
	for(i=0; i<entry_dir[Index].name_len; i++)
		 entry_dir[Index].name[i]=0x00;
	entry_dir[Index].name[i]=0x00;
	entry_dir[Index].name_len=0;
	entry_dir[Index].inode=INODE_ENTRY_BIT;

			//file entry 0번째에서 비트 제거  
	Bit_bitmap=1<<Index;
	entry_dir[0].file_type&=(~Bit_bitmap);
			
	WriteSector(PATA_PORT_1,true,Super.block_table+inode_table[inode_pwd%4].block[Block_Index],
	BLOCK_SIZE/SECTOR,entry_dir);

	for(i=0; i<inode_table_remove[file_inode%4].links_count; i++)	Free_Block(inode_table_remove[file_inode%4].block[i]);
																	Free_inode(file_inode);
	return SUCCESS;
}

ERROR ChangeDirectory(const char *_DirName)
{
	int i;
	uint inode_pwd=*Return_pwd_inode();
	uchar Bit,Block_Index=0;
	Dir_Entry entry_dir[ENTRY_COUNT];
	inode inode_table[4];
	ReadSector(PATA_PORT_1,true,Super.inode_table+inode_pwd/4,SECTOR/SECTOR,inode_table);
	while(Block_Index<12)
	{
		ReadSector(PATA_PORT_1,true,Super.block_table+inode_table[inode_pwd%4].block[Block_Index],	
		BLOCK_SIZE/SECTOR,entry_dir);
		Bit=1<<1;
		for(i=1; i<ENTRY_COUNT; i++)
		{
			if(entry_dir[0].file_type&Bit)
			{
				if(strcmp(entry_dir[i].name,_DirName)&&entry_dir[i].file_type&FILETYPE_DIR)
				{
					Super.inode_NowDir=entry_dir[i].inode;
					stradd(pwd_Path,"/");
					stradd(pwd_Path,entry_dir[i].name);
					return SUCCESS;
				}
			}
			Bit=Bit<<1;
		}
		Block_Index++;
	}
	return NO_FILE;
}

ERROR RenameFile(const char *_FileName_old,const char *_FileName_new)
{
	if(strlen(_FileName_new)>=57||strlen(_FileName_new)<=0)
	{
		return NAME_OVER;
	} 
	uint inode_pwd=*Return_pwd_inode();
	uint Index,Block_Index,new_Index,new_Block_Index;
	Find_withFilename(_FileName_old,&Block_Index,&Index);
	Find_withFilename(_FileName_new,&new_Block_Index,&new_Index);
	if(Index==INODE_ENTRY_BIT||Block_Index==BLOCK_INDEX_NIL)
	{
		return NO_FILE;
	}
	if(new_Index!=INODE_ENTRY_BIT||new_Block_Index!=BLOCK_INDEX_NIL)
	{
		return SAME_NAME;
	}
	inode inode_table[4];
	Dir_Entry entry_dir[ENTRY_COUNT];
	ReadSector(PATA_PORT_1,true,Super.inode_table+inode_pwd/4,SECTOR/SECTOR,inode_table);
	ReadSector(PATA_PORT_1,true,Super.block_table+inode_table[inode_pwd%4].block[Block_Index],
	BLOCK_SIZE/SECTOR,entry_dir);
	
	strcpy(entry_dir[Index].name,_FileName_new);
	entry_dir[Index].name_len=strlen(_FileName_new);
	
	WriteSector(PATA_PORT_1,true,Super.block_table+inode_table[inode_pwd%4].block[Block_Index],
	BLOCK_SIZE/SECTOR,entry_dir);
	return SUCCESS;
}

ERROR Concatenate(const char *_FileName)
{
	//파일 없음,파일 아님,성공 
	uint i,Bit_bitmap; 
	uint Index,Block_Index,file_inode,inode_pwd=*Return_pwd_inode();
	Find_withFilename(_FileName,&Block_Index,&Index);
	if(Index==INODE_ENTRY_BIT||Block_Index==BLOCK_INDEX_NIL)
	{
		return NO_FILE;
	}
	inode inode_table[4];
	char File_Content[BLOCK_SIZE+1];
	Dir_Entry entry_dir[ENTRY_COUNT];
	ReadSector(PATA_PORT_1,true,Super.inode_table+inode_pwd/4,SECTOR/SECTOR,inode_table);
	ReadSector(PATA_PORT_1,true,Super.block_table+inode_table[inode_pwd%4].block[Block_Index],
	BLOCK_SIZE/SECTOR,entry_dir);
	if(entry_dir[Index].file_type&FILETYPE_DIR)
	{
		return NOT_FILE;
	}
	file_inode=entry_dir[Index].inode;
	ReadSector(PATA_PORT_1,true,Super.inode_table+file_inode/4,SECTOR/SECTOR,inode_table);
	for(i=0; i<12; i++)
	{
		ReadSector(PATA_PORT_1,true,Super.block_table+inode_table[file_inode%4].block[i],
		BLOCK_SIZE/SECTOR,File_Content);
		File_Content[1024]='\0';
		if(printf("%s",File_Content)==EOF)
		{
			putchar('\n');
			break;
		}
	}
	return SUCCESS;
}

ERROR WriteFile(const char *_FileName,const char* Content)
{
	uint i,Bit_bitmap; 
	uint Index,Block_Index,file_inode,inode_pwd=*Return_pwd_inode();
	Find_withFilename(_FileName,&Block_Index,&Index);
	if(Index==INODE_ENTRY_BIT||Block_Index==BLOCK_INDEX_NIL)
	{
		return NO_FILE;
	}
	inode inode_table[4];
	char File_Content[BLOCK_SIZE];
	Dir_Entry entry_dir[ENTRY_COUNT];
	ReadSector(PATA_PORT_1,true,Super.inode_table+inode_pwd/4,SECTOR/SECTOR,inode_table);
	ReadSector(PATA_PORT_1,true,Super.block_table+inode_table[inode_pwd%4].block[Block_Index],
	BLOCK_SIZE/SECTOR,entry_dir);
	if(entry_dir[Index].file_type&FILETYPE_DIR)
	{
		return NOT_FILE;
	}
	file_inode=entry_dir[Index].inode;
	ReadSector(PATA_PORT_1,true,Super.inode_table+file_inode/4,SECTOR/SECTOR,inode_table);
	
	Block_Index=0;
	ReadSector(PATA_PORT_1,true,Super.block_table+inode_table[file_inode%4].block[Block_Index],
	BLOCK_SIZE/SECTOR,File_Content);
	for(i=0; Content[i]!='\0'; i++)
	{		
		File_Content[i]=Content[i];
		if(i/BLOCK_SIZE)
		{
			WriteSector(PATA_PORT_1,true,Super.block_table+inode_table[file_inode%4].block[Block_Index++],
			BLOCK_SIZE/SECTOR,File_Content);
			if(Block_Index>=12)
				return NOT_ENOUGH;
			ReadSector(PATA_PORT_1,true,Super.block_table+inode_table[file_inode%4].block[Block_Index],
			BLOCK_SIZE/SECTOR,File_Content);
		}
	}
	File_Content[i]=EOF;
	WriteSector(PATA_PORT_1,true,Super.block_table+inode_table[file_inode%4].block[Block_Index],
	BLOCK_SIZE/SECTOR,File_Content);
	return SUCCESS;
}

void Find_withFilename(const char* FileName,uint *_block_index,uint *_entry_index)//이거에서 문제 있는거 확실  
{
	int i;
	uint inode_pwd=*Return_pwd_inode();
	uchar Bit,Block_Index=0;
	Dir_Entry entry_dir[ENTRY_COUNT];
	inode inode_table[4];
	ReadSector(PATA_PORT_1,true,Super.inode_table+inode_pwd/4,SECTOR/SECTOR,inode_table);
	while(Block_Index<12)
	{
		ReadSector(PATA_PORT_1,true,Super.block_table+inode_table[inode_pwd%4].block[Block_Index],	
		BLOCK_SIZE/SECTOR,entry_dir);
		Bit=1<<1;
		for(i=1; i<ENTRY_COUNT; i++)
		{
			if(entry_dir[0].file_type&Bit)
			{
				if(strcmp(entry_dir[i].name,FileName))
				{
					*_block_index=Block_Index;
					*_entry_index=i;
					return ;
				}
			}
			Bit=Bit<<1;
		}
		Block_Index++;
	}
	*_block_index=BLOCK_INDEX_NIL;
	*_entry_index=INODE_ENTRY_BIT;
}

bool Check_isFull(Dir_Entry *Bit_Entry)
{
	return (Bit_Entry->file_type==0xFFFF);
}

uchar Return_EmptyIndex(Dir_Entry *Bit_Entry)
{
	uchar i;
	ushort Bit=1;
	for(i=0; i<ENTRY_COUNT; i++)
	{
		if(!(Bit_Entry[0].file_type&Bit))
		{
			Bit_Entry[0].file_type|=Bit;
			return i;
		}
		Bit=Bit<<1;
	}
	return 0;
}

void SetSuperBlock(superblock *Super)
{
	ulld KernelOff=FILESYSTEM_OFFSET;
	
	Super->block_count=(return_diskinfo()->TotalSectors-KernelOff)/2;
	Super->inode_count=Super->block_count/8;

	Super->free_block_count=Super->block_count;
	Super->free_inode_count=Super->inode_count;

	Super->inode_size=sizeof(inode);
	Super->block_size=BLOCK_SIZE;
	
	
	KernelOff+=2;
	Super->block_bitmap=KernelOff;
	
	KernelOff+=((Super->block_count)/BYTE_BIT/SECTOR);
	Super->inode_bitmap=KernelOff;
	
	KernelOff+=((Super->inode_count)/BYTE_BIT/SECTOR);
	Super->inode_table=KernelOff;
	
	KernelOff+=Super->inode_count/8;
	Super->block_table=KernelOff;
	
	Super->free_block_count-=(KernelOff-FILESYSTEM_OFFSET)/2;
	
	Super->inode_NowDir=INODE_ROOTDIR;
}

void SetBitmap_block(uchar *Bitmap_block,superblock *super)
{
	int i,j;
	char Bit=1;
	for(i=0; i<(super->block_count-super->free_block_count)/8; i++)
	{
		for(j=0; j<BYTE_BIT; j++)
		{
			Bit=Bit|(Bit<<1);
		}
		Bitmap_block[i]|=Bit;
		Bit=1;
	}
}


void SetIsFmat(void)
{
	uchar Bootstrap[512];
	ReadSector(PATA_PORT_1,true,0,SECTOR/SECTOR,Bootstrap);
	Bootstrap[ISFORMATED]=true;
	WriteSector(PATA_PORT_1,true,0,SECTOR/SECTOR,Bootstrap);
}

char *return_pwd(void)
{
	return pwd_Path;
}

uint* Return_pwd_inode(void)
{
	return &(Super.inode_NowDir);
}

superblock* return_Super(void)
{
	return &Super;
}

