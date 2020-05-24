#ifndef _FILESYSTEM_H_
#define _FILESYSTEM_H_

#include "DiskDriver.h"

#define FILESYSTEM_OFFSET (*(char*)(0x7D4A))
#define ISFORMATED 		  0x14C
#define NIL 0

#define BLOCK_SIZE      1024
#define ENTRY_COUNT     (BLOCK_SIZE/sizeof(Dir_Entry))

#define BLOCK_INDEX_NIL 0xFF

#define INODE_ENTRY_BIT 0x0
#define INODE_ROOTDIR   0x1

#define MODE_DIR 	    0x0
#define MODE_FILE       0x1

#define FILETYPE_FILE	0x1
#define FILETYPE_DIR	0x2
#define FILETYPE_DISRM  0x4

#define BYTE_BIT 8

#pragma pack(push,1)

typedef struct superblock
{
	uint inode_count;
	uint block_count;
	
	uint free_block_count;
	uint free_inode_count;
	
	ushort inode_size;
	ushort block_size;
	
	uint block_bitmap;
	uint inode_bitmap;
	uint inode_table;
	uint block_table;
	
	uint inode_NowDir;
	ushort Reserved[237];
}superblock;

typedef struct inode
{
	uint mode;
	ulld size;
	ushort links_count;
	uint block[15];
	ushort Padding[27];
}inode; 

typedef struct Dir_Entry
{
	uint inode;
	uchar name_len;
	ushort file_type;
	uchar name[57];
}Dir_Entry;

typedef enum ERROR
{
	SAME_NAME,
	NAME_OVER,
	NOT_ENOUGH,	
	NO_FILE,
	NOT_FILE,
	SUCCESS,
}ERROR;

#pragma pack(pop)

bool Format(void);

uint Alloc_Block(void);
uint Alloc_inode(void);
bool Free_Block(uint block);
bool Free_inode(uint inode);

void MakeRootDir(void);
ERROR CreateFile(const char *_FileName);
ERROR DeleteFile(const char *_FileName);
ERROR RemoveDir(const char *_DirName);
ERROR MakeDir(const char *_DirName);
ERROR ChangeDirectory(const char *_DirName);
ERROR RenameFile(const char *_FileName_old,const char *_FileName_new);
ERROR Concatenate(const char *_FileName);
ERROR WriteFile(const char *_FileName,const char* Content);

void Find_withFilename(const char* FileName,uint *_block_index,uint *_entry_index);
bool Check_isFull(Dir_Entry *Bit_Entry);
uchar Return_EmptyIndex(Dir_Entry *Bit_Entry);

void SetSuperBlock(superblock *Super);
void SetBitmap_block(uchar *Bitmap_block,superblock *super);

char *return_pwd(void);
void SetIsFmat(void);
bool CheckIsFmat(void);

uint* Return_pwd_inode(void);
superblock* return_Super(void);

#endif
