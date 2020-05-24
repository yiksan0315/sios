#include "IO.h"

void printk(const char *Message,int line,uint Color)
{
	int i;
	char* Video=(char*)(VIDEO+160*line);
	
	for(i=0; Message[i]!='\0'; i++)
	{
		*Video=Message[i];
		Video++;
		*Video=Color;
		Video++;
	}
	return ;
}

