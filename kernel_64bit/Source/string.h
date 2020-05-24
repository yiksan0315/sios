#ifndef _STRING_H_
#define _STRING_H_

#include "Data Types.h"

int strlen(const char* _Format);
unsigned char strcmp(const char* _Object,const char* _Subject);
void strcpy(char* _Object,const char* _Source);
void stradd(char* _Object,const char* _Source);
void trim(char * _Format);


ulld string_to_num(const char* string);

#endif
