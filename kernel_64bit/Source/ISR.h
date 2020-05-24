#ifndef _ISR_H_
#define _ISR_H_

void ISR_Exception_Div_by0(void); 				//0
void ISR_Exception_Debug(void);					//1
void ISR_Exception_NMI(void);					//2
void ISR_Exception_BreakPoint(void);			//3
void ISR_Exception_INTO_Overflow(void);			//4
void ISR_Exception_Bounds_Range_exceeded(void);	//5
void ISR_Exception_Invalid_Op(void);			//6
void ISR_Exception_Disavailable_Device(void);	//7
void ISR_Exception_DoubleFault(void);			//8
void ISR_Exception_Copro_Seg_Overrun(void);		//9
void ISR_Exception_Invalid_Tss(void);			//10
void ISR_Exception_NotPresent_Seg(void);		//11
void ISR_Exception_StackFault(void);			//12
void ISR_Exception_GprotectionFault(void);		//13
void ISR_Exception_PageFault(void);				//14
void ISR_Exception_Reserved(void);				//15
void ISR_Exception_FPU(void);					//16
void ISR_Exception_Check_Alignment(void);		//17
void ISR_Exception_Check_Mach(void);			//18
void ISR_Exception_SIMD(void);					//19
void ISR_Reserved_20_31(void);					//20~31

void ISR_Intterupt_Timer(void);					//32
void ISR_Intterupt_KeyBoard(void);				//33
void ISR_Intterupt_SlavePIC(void);				//34
void ISR_Intterupt_Serial_2(void);				//35
void ISR_Intterupt_Serial_1(void);				//36
void ISR_Intterupt_Parallel_2(void);			//37
void ISR_Intterupt_Floppy(void);				//38
void ISR_Intterupt_Parallel_1(void);			//39
void ISR_Intterupt_RTC(void);					//40
void ISR_Intterupt_Reserved(void);				//41
void ISR_Intterupt_NonUsed_1(void);				//42
void ISR_Intterupt_NonUser_2(void);				//43
void ISR_Intterupt_Mouse(void);					//44
void ISR_Intterupt_Coprocessor(void);			//45
void ISR_Intterupt_Hdd_1(void);					//46
void ISR_Intterupt_Hdd_2(void);					//47
void ISR_Intterupt_Other(void);					//48~255

#endif 
