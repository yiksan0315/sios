#ifndef _IO_H_
#define _IO_H_

#include "Data Types.h"
#include "Assembly_Func.h"
#include <stdarg.h>

//Video Memory
#define VIDEO 0xB8000
#define WIDTH 80
#define HEIGHT 25
#define CURSOR_LIMIT WIDTH*HEIGHT-1

//Color
#define BLACK 0x00
#define BLUE 0x01
#define GRENN 0x02
#define CYAN 0x03
#define RED 0x04
#define MAGENTA 0x05
#define BROWN 0x06
#define GRAY 0x07
#define _GRAY 0x08
#define _BLUE 0x09
#define _GREEN 0x0A
#define _CYAN 0x0B
#define _RED 0x0C
#define _MAGENTA 0x0D
#define _YELLOW 0x0E
#define _WHITE 0x0F

#define DEFAULT_COLOR GRAY
#define PATH_COLOR _CYAN

#define In_Buf 0x2
#define Out_Buf 0x1

#define ACK 0xFA
#define EOF -1

#define KEY_NONE        0x00
#define KEY_ENTER       '\n'
#define KEY_TAB         '\t'
#define KEY_ESC         0x1B
#define KEY_BACKSPACE   0x08

#define KEY_CTRL        0x81
#define KEY_LSHIFT      0x82
#define KEY_RSHIFT      0x83
#define KEY_PRINTSCREEN 0x84
#define KEY_LALT        0x85
#define KEY_CAPSLOCK    0x86
#define KEY_F1          0x87
#define KEY_F2          0x88
#define KEY_F3          0x89
#define KEY_F4          0x8A
#define KEY_F5          0x8B
#define KEY_F6          0x8C
#define KEY_F7          0x8D
#define KEY_F8          0x8E
#define KEY_F9          0x8F
#define KEY_F10         0x90
#define KEY_NUMLOCK     0x91
#define KEY_SCROLLLOCK  0x92
#define KEY_HOME        0x93
#define KEY_UP          0x94
#define KEY_PAGEUP      0x95
#define KEY_LEFT        0x96
#define KEY_CENTER      0x97
#define KEY_RIGHT       0x98
#define KEY_END         0x99
#define KEY_DOWN        0x9A
#define KEY_PAGEDOWN    0x9B
#define KEY_INS         0x9C
#define KEY_DEL         0x9D
#define KEY_F11         0x9E
#define KEY_F12         0x9F
#define KEY_PAUSE       0xA0

#define BUFFER_SIZE 100

typedef struct KeyTable
{
	uchar NormalKey;
	uchar Shift_Num_LockKey;
}KeyTable;

//			 	   Console IO		      	   //
/***********************************************/
int printf(const char * _Format,...);
int Arg_printf(char Arg_Type,va_list Arg_,char **Video_Mem);

void putchar(const char _Format);

void LineFeed(char **Video_Mem);
void BackSpace(char **Video_Mem);

int PrintString(const char* Message,char **Video_Mem);
void Num_to_String(long long int Number,char **Video_Mem);

void SetCursor(int X);
void SetColor_Path(int Path_Length);
void Scroll(void);

/***********************************************/

//				     BUFFER	            	   //
/***********************************************/
unsigned char GetKeyBuf(unsigned char* _Flag);
unsigned char PutKeyBuf(unsigned char KEY);
/***********************************************/

//				      IO   					   //
/***********************************************/
void printk(const char *Message,int line,int Offset,uint Color);
void cleark();
void asd(int Number,int Offset);
/***********************************************/

//				   Keyboard					   //
/***********************************************/
bool Is_Ack(void);
bool IsBuf_full(uint In_Out); 

bool EnableKeyBoard(void);
bool On_Off_LED(uchar Num_Lock,uchar Caps_Lock,uchar Scroll_Lock);

bool Scan_Convert_ASCII(uchar ScanCode,uchar *ASCII_Code);
bool CheckisDual(uchar ScanCode);
bool CheckisNumPad(uchar ScanCode);
/***********************************************/

#endif
