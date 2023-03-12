#include "Shell.h"
#include "Command.h"
#include "string.h"
#include "Scheduler.h"
#include "Memory.h"
#include "FileSystem.h"

Command_Table _ComTable[]=
{
	{"help",help},
	{"task",TestTask_Create},
	{"clear",Clear_Command},
	{"kill",kill},
	{"date",date},
	{"systeminfo",systeminfo},
	{"shutdown",shutdown},
	{"ls",ls},
	{"touch",touch},
	{"rm",rm},
	{"mkdir",mkdir},
	{"rmdir",rmdir},
	{"cd",cd},
	{"ren",ren},
	{"cat",cat},
	{"write",write}
};

Command_Table* return_comtable(void)
{
	return _ComTable;
}

void Shell_Start(void)
{
	Command_buffer _Command; _Command.Length=0;
	unsigned long long int RFLAGS_;
	bool Flag;
	cleark();
	printf("%s>",return_pwd());
	SetColor_Path(strlen(return_pwd())+1);
	while(true)
	{
		RFLAGS_=Read_Rflags();
		__asm__ __volatile__("cli;");
		_Command.Buffer_Command[_Command.Length]=GetKeyBuf(&Flag);
		if(RFLAGS_&0x200)
			__asm__ __volatile__("sti;");
		if(Flag)
		{
			if(_Command.Length>=COMMAND_MAX)
			{
				char Command=GetCommand(&_Command);
				_Command.Buffer_Command[_Command.Length]=0;
				if(Command==KEY_BACKSPACE)
				{
					putchar(Command);
					_Command.Length--;
				}
			}
			else if(Filter_Is_ASCII(GetCommand(&_Command)))
			{
				putchar(GetCommand(&_Command));
				if(GetCommand(&_Command)==KEY_BACKSPACE)
				{
					_Command.Buffer_Command[_Command.Length]=0;
					
					if(_Command.Length>0)
						_Command.Length--;//앞에 PATH가 존재해서 지우지 못했을경우 감소 되면 안됨 
				}
				else if(GetCommand(&_Command)=='\n')
				{
					_Command.Buffer_Command[_Command.Length]='\0';
					Put_Com_toConsole(&_Command,&Flag);
					printf("%s>",return_pwd());
					SetColor_Path(strlen(return_pwd())+1);
				}
				else if(GetCommand(&_Command)=='\t')
				{
					//Find_SimilarFile();
				}
				else
				{
					_Command.Length++;
				}
			}
		}
	}
}

unsigned char Filter_Is_ASCII(unsigned char Code)
{
	if(1<=Code&&Code<=126&&Code!=27)
		return true;
	return false;
}

char GetCommand(Command_buffer* _Command)
{
	return _Command->Buffer_Command[_Command->Length];
}

void Put_Com_toConsole(Command_buffer *_Command,unsigned char* Flag_)
{
	int i,argc; 
	char Com[1024]; 
	char *argv[ARG_COUNT];
	if(strlen(_Command->Buffer_Command)==0)
	{
		return ;
	}  
	for(i=1; i<2; i++)
		argv[i]=(char *)k_malloc(COMMAND_MAX);
	argc=GetParameter(Com,_Command->Buffer_Command,argv);
	for(i=0; i<COMMAND_COUNT; i++)
	{
		if(strcmp(Com,_ComTable[i].Command))
		{
			_ComTable[i].Command_Pointer(argc,argv);
			RemoveCommand(_Command);
			for(i=1; i<2; i++)
				k_free(argv[i]);
			for(i=0; Com[i]!='\0'; i++)
				Com[i]=0x00;
			putchar('\n');
			return ;
		}
	}
	for(i=1; i<2; i++)
		k_free(argv[i]);
	printf("\"%s\" is not Command or Excutable Program.\n",Com);
	for(i=0; Com[i]!='\0'; i++)
		Com[i]=0x00;
	RemoveCommand(_Command);
}

void RemoveCommand(Command_buffer* _Command)
{
	int i;
	for(i=0; i<_Command->Length; i++)
	{
		_Command->Buffer_Command[i]=0;
	}
	_Command->Buffer_Command[i]=0;
	_Command->Length=0;
}

int GetParameter(char *Com,const char* Parameter,char **argv)
{
	int i,j;
	int Index=0,argc=1;
	strcpy(argv[0],return_pwd());
	
	while(Parameter[Index]==' '){Index++;}
	for(i=0; Parameter[Index]!=' '; i++,Index++)
	{
		Com[i]=Parameter[Index];
		if(Com[i]=='\0')
		{
			return argc;
		}
	}
	while(true)
	{
		while(Parameter[Index]==' '){Index++;}
		for(i=0; Parameter[Index]!=' '; i++,Index++)
		{
			if(Parameter[Index]=='\0')
			{
				if(!i)
				{
					return argc;
				}
				argv[argc++][i]='\0';
				if(argv[argc][i-1]=='\0')
					argc--;
				return argc;
			}
			argv[argc][i]=Parameter[Index];
		}
		argv[argc++][i]='\0';
	}
}



