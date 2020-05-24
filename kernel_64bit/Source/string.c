#include "string.h"

int strlen(const char* _Format)
{
	int _Strlen=0;
	while(_Format[_Strlen]!='\0'){_Strlen++;}
	
	return _Strlen;
}

unsigned char strcmp(const char* _Object,const char* _Subject)
{
	int i;
	for(i=0; _Object[i]!='\0'&& _Subject[i]!='\0'; i++)
	{
		if(_Object[i]!=_Subject[i])
			return 0;
	}
	if(_Object[i]=='\0'&&_Subject[i]=='\0')
		return 1;
	return 0;
}

void strcpy(char* _Object,const char* _Source)
{
	int i;
	for(i=0; _Source[i]!='\0'; i++)
		_Object[i]=_Source[i];
	_Object[i]='\0';
}

void stradd(char* _Object,const char* _Source)
{
	int i,Index=strlen(_Object);
	for(i=0; i<_Source[i]!='\0'; i++)
	{
		_Object[Index++]=_Source[i];
	}
	_Object[Index]=_Source[i];
}

void trim(char * _Format)
{
	int i,Char_Format=0;
	for(i=0; 1; i++)
	{
		if(_Format[i]!=' ')
		{
			_Format[Char_Format++]=_Format[i];
			if(_Format[i]=='\0')
				return ;
		}
	}
}

ulld string_to_num(const char* string)
{
	int i=strlen(string)-1;
	ulld Num=0,_10=1;
	bool is_minus=false;
	if(string[0]=='-')
	{
		is_minus=true;
	}
	for(i; i>=is_minus; i--)
	{
		if(string[i]>'9'||string[i]<'0')
			return 0;
		Num=Num+(string[i]-'0')*_10;
		_10=_10*10;
	}
	if(is_minus)
		return Num* -1;
	return Num;
}
