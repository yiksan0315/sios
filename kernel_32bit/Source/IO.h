#ifndef _IO_H_
#define _IO_H_

#include "Data Types.h"

//Video Memory
#define VIDEO 0xB8000

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

void printk(const char *Message,int line,uint Color);

#endif
