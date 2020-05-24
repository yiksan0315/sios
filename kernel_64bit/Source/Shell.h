#ifndef _SHELL_H_
#define _SHELL_H_

#define ARG_COUNT 60
#define COMMAND_MAX 1024

typedef struct struct_Command_buffer
{
	char Buffer_Command[COMMAND_MAX];
	int Length;	
}Command_buffer;

typedef struct struct_Command_Table
{
	char Command[80];
	void (*Command_Pointer)(int argc,char** argv);
}Command_Table;

void Shell_Start(void);
unsigned char Filter_Is_ASCII(unsigned char Code);
char GetCommand(Command_buffer* _Command);
void Put_Com_toConsole(Command_buffer *_Command,unsigned char* Flag_);
void RemoveCommand(Command_buffer* _Command);

int GetParameter(char *Com,const char* Parameter,char **argv);
Command_Table* return_comtable(void);

#endif 
