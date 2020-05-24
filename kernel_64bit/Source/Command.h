#ifndef _COMMAND_H_
#define _COMMAND_H_

#include "IO.h"
#include "Set_HardWare.h"
#include "DiskDriver.h"

#define COMMAND_COUNT 16

void help(int argc,char* argv[]);
void Clear_Command(int argc,char * argv[]);
void kill(int argc,char* argv[]);

void date(int argc,char** argv);
void shutdown(int argc,char** argv);
void systeminfo(int argc,char** argv);

void ls(int argc,char **argv);
void touch(int argc,char **argv);
void rm(int argc,char **argv);
void mkdir(int argc,char **argv);
void rmdir(int argc,char **argv);
void cd(int argc,char **argv);
void ren(int argc,char **argv);
void cat(int argc,char **argv);
void write(int argc,char **argv);

void Process_Game(int argc,char **argv);
void Process_TextEditor(int argc,char **argv);

void cpuid_(ulld code,ulld ebx,ulld ecx,ulld edx);

#endif
