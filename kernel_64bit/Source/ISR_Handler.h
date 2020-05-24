#ifndef _ISR_HANDLER_H_
#define _ISR_HANDLER_H_

void ISR_Timer(unsigned char ISR_line);
void ISR_Keyboard(unsigned char ISR_line);
void ISR_HARDDISK(unsigned int ISR_line);

void ISR_FPU_ContextSwitching(unsigned char ISR_line);
void Exception_Routine(void);
void Intterupt_Routine(unsigned char ISR_line);

#endif
