#ifndef _ASSEMBLY_FUNC_H_
#define _ASSEMBLY_FUNC_H_

unsigned char ReadPort_In(unsigned int Port);
void WritePort_Out(unsigned int Port,unsigned char Data);
void WritePort_Out_word(unsigned int Port,unsigned short Data);
unsigned short ReadPort_In_word(unsigned int Port);
unsigned long long int Read_Rflags(void);
void stts(void);
void clts(void);

#endif
