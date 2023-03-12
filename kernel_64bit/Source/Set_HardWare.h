#ifndef _SET_HARDWARE_H_
#define _SET_HARDWARE_H_

#define MASTER_ICW1 0x20
#define MASTER_OCW2 0x20
#define MASTER_OCW3 0x20

#define MASTER_ICW2 0x21
#define MASTER_ICW3 0x21
#define MASTER_ICW4 0x21
#define MASTER_OCW1 0x21

#define SLAVE_ICW1 0xA0
#define SLAVE_OCW2 0xA0
#define SLAVE_OCW3 0xA0

#define SLAVE_ICW2 0xA1
#define SLAVE_ICW3 0xA1
#define SLAVE_ICW4 0xA1
#define SLAVE_OCW1 0xA1

#define PIC_EOI 0x20

#define PIT_CONTROL 0x43
#define PIT_COUTER0 0x40
#define PIT_COUTER1 0x41
#define PIT_COUTER2 0x42

#define PIT_CR_COUTER0 0x00
#define PIT_CR_COUTER1 0x40
#define PIT_CR_COUTER2 0x80

#define PIT_ONESECOND 1193182

#define LATCH 0x00
#define ACCESS_LOW_HIGH 0x30

#define MODE0 0x00 
#define MODE1 0x02
#define MODE2 0x04
#define MODE3 0x06 
#define MODE4 0x08
#define MODE5 0x0A

#define BCD_MODE 0x1
#define BIN_MODE 0x0

#define CMOS_PORT 0x70
#define CMOS_DATA_PORT 0x71

#define RTC_SECOND 0x00
#define RTC_MINUTE 0x02
#define RTC_HOUR 0x04
#define RTC_DAYOFWEEK 0x06
#define RTC_DAY 0x07
#define RTC_MONTH 0x08
#define RTC_YEAR 0x09

void Init_PIC(void);
void Send_EOI_PIC(unsigned char IRQ);
void Set_Mask_IRQ(unsigned short IRQ);

void Set_PIT(unsigned short Count,unsigned char Loop);
unsigned short ReadCounter_0(void);
unsigned long long int rdtsc(void);
unsigned char GetRTC(unsigned char Date_or_Time);
#endif 
