[bits 64]

extern Exception_Routine, Intterupt_Routine ,ISR_Timer ,ISR_Keyboard, ISR_FPU_ContextSwitching,ISR_HardDisk

global ISR_Exception_Div_by0, ISR_Exception_Debug,	ISR_Exception_NMI, ISR_Exception_BreakPoint, ISR_Exception_INTO_Overflow, ISR_Exception_Bounds_Range_exceeded
global ISR_Exception_Invalid_Op, ISR_Exception_Disavailable_Device, ISR_Exception_DoubleFault, ISR_Exception_Copro_Seg_Overrun, ISR_Exception_Invalid_Tss, ISR_Exception_NotPresent_Seg
global ISR_Exception_StackFault, ISR_Exception_GprotectionFault, ISR_Exception_PageFault, ISR_Exception_Reserved, ISR_Exception_FPU, ISR_Exception_Check_Alignment
global ISR_Exception_Check_Mach, ISR_Exception_SIMD, ISR_Reserved_20_31
global ISR_Intterupt_Timer, ISR_Intterupt_KeyBoard, ISR_Intterupt_SlavePIC, ISR_Intterupt_Serial_2, ISR_Intterupt_Serial_1, ISR_Intterupt_Parallel_2
global ISR_Intterupt_Floppy, ISR_Intterupt_Parallel_1, ISR_Intterupt_RTC, ISR_Intterupt_Reserved, ISR_Intterupt_NonUsed_1, ISR_Intterupt_NonUser_2
global ISR_Intterupt_Mouse, ISR_Intterupt_Coprocessor, ISR_Intterupt_Hdd_1, ISR_Intterupt_Hdd_2, ISR_Intterupt_Other

%macro SaveContext 0
    push rbp
    push rax
    push rbx
    push rcx
    push rdx
    push rdi
    push rsi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    mov ax,ds
    push rax
    mov ax,es
    push rax
    push fs
    push gs

    mov ax,0x10
    mov ds,ax
    mov es,ax
    mov gs,ax
    mov fs,ax
%endmacro

%macro LoadContext 0
    pop gs
    pop fs
    pop rax
    mov es,ax
    pop rax
    mov ds,ax

    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rsi
    pop rdi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    pop rbp
%endmacro

ISR_Exception_Div_by0:
     SaveContext
     push rdi
     mov rdi,0

     call Exception_Routine

     pop rdi
     LoadContext
    iretq

ISR_Exception_Debug:
     SaveContext
     push rdi
     mov rdi,1

     call Exception_Routine

     pop rdi

     LoadContext
    iretq

ISR_Exception_NMI:
     SaveContext

     push rdi
     mov rdi,2

     call Exception_Routine

     pop rdi

     LoadContext
    iretq

ISR_Exception_BreakPoint:
     SaveContext

     push rdi
     mov rdi,3

     call Exception_Routine

     pop rdi

     LoadContext
    iretq

ISR_Exception_INTO_Overflow:
     SaveContext

    push rdi
     mov rdi,4

     call Exception_Routine

     pop rdi

     LoadContext
    iretq

ISR_Exception_Bounds_Range_exceeded:
     SaveContext

     push rdi
     mov rdi,5

     call Exception_Routine

     pop rdi

     LoadContext
    iretq

ISR_Exception_Invalid_Op:
     SaveContext

     push rdi
     mov rdi,6

     call Exception_Routine

     pop rdi

     LoadContext
    iretq

ISR_Exception_Disavailable_Device:
     SaveContext

     push rdi
     mov rdi,7

     call ISR_FPU_ContextSwitching

     pop rdi

     LoadContext
    iretq

ISR_Exception_DoubleFault:
     SaveContext

     push rdi
     mov rdi,8

     call Exception_Routine

     pop rdi
     add rsp,8
     LoadContext
    iretq

ISR_Exception_Copro_Seg_Overrun:
     SaveContext

     push rdi
     mov rdi,9

     call Exception_Routine

     pop rdi

     LoadContext
    iretq

ISR_Exception_Invalid_Tss:
     SaveContext

     push rdi
     mov rdi,10

     call Exception_Routine

     pop rdi
     add rsp,8
     LoadContext
    iretq

ISR_Exception_NotPresent_Seg:
     SaveContext

     push rdi
     mov rdi,11

     call Exception_Routine

     pop rdi
     add rsp,8
     LoadContext
    iretq

ISR_Exception_StackFault:
     SaveContext

     push rdi
     mov rdi,12

     call Exception_Routine

     pop rdi
     add rsp,8
     LoadContext
    iretq

ISR_Exception_GprotectionFault:
     SaveContext

     push rdi
     mov rdi,13

     call Exception_Routine

     pop rdi
      add rsp,8
     LoadContext
    iretq

ISR_Exception_PageFault:
     SaveContext

     push rdi
     mov rdi,14

     call Exception_Routine

     pop rdi
     add rsp,8
     LoadContext
    iretq

ISR_Exception_Reserved:
     SaveContext

     push rdi
     mov rdi,15

     call Exception_Routine

     pop rdi

     LoadContext
    iretq

ISR_Exception_FPU:
     SaveContext

     push rdi
     mov rdi,16

     call Exception_Routine

     pop rdi

     LoadContext
    iretq

ISR_Exception_Check_Alignment:
     SaveContext

     push rdi
     mov rdi,17

     call Exception_Routine

     pop rdi
     add rsp,8
     LoadContext
    iretq

ISR_Exception_Check_Mach:
     SaveContext

    push rdi
     mov rdi,18

     call Exception_Routine

     pop rdi
     LoadContext
    iretq

ISR_Exception_SIMD:
     SaveContext

     push rdi
     mov rdi,19

     call Exception_Routine

     pop rdi

     LoadContext
    iretq

ISR_Reserved_20_31:
     SaveContext

     push rdi
     mov rdi,20

     call Exception_Routine

     pop rdi

     LoadContext
    iretq

;/*************************************************/;

ISR_Intterupt_Timer:
     SaveContext

     mov rdi,32
     call ISR_Timer

     LoadContext
    iretq

ISR_Intterupt_KeyBoard:
    SaveContext

    mov rdi,33
    call ISR_Keyboard

    LoadContext
    iretq

ISR_Intterupt_SlavePIC:
     SaveContext

     mov rdi,34
     call Intterupt_Routine

     LoadContext
    iretq

ISR_Intterupt_Serial_2:
     SaveContext

     mov rdi,35
     call Intterupt_Routine

     LoadContext
    iretq

ISR_Intterupt_Serial_1:
     SaveContext

     mov rdi,36
     call Intterupt_Routine

     LoadContext
    iretq

ISR_Intterupt_Parallel_2:
     SaveContext

     mov rdi,37
     call Intterupt_Routine

     LoadContext
    iretq

ISR_Intterupt_Floppy:
     SaveContext

     mov rdi,38
     call Intterupt_Routine

     LoadContext
    iretq

ISR_Intterupt_Parallel_1:
     SaveContext

     mov rdi,39
     call Intterupt_Routine

     LoadContext
    iretq

ISR_Intterupt_RTC:
     SaveContext

     mov rdi,40
     call Intterupt_Routine

     LoadContext
    iretq

ISR_Intterupt_Reserved:
     SaveContext

     mov rdi,41
     call Intterupt_Routine

     LoadContext
    iretq

ISR_Intterupt_NonUsed_1:
     SaveContext

     mov rdi,42
     call Intterupt_Routine

     LoadContext
    iretq

ISR_Intterupt_NonUser_2:
     SaveContext

     mov rdi,43
     call Intterupt_Routine

     LoadContext
    iretq

ISR_Intterupt_Mouse:
     SaveContext

     mov rdi,44
     call Intterupt_Routine

     LoadContext
    iretq

ISR_Intterupt_Coprocessor:
     SaveContext

     mov rdi,45
     call Intterupt_Routine

     LoadContext
    iretq

ISR_Intterupt_Hdd_1:
     SaveContext

     mov rdi,46
     call ISR_HardDisk

     LoadContext
    iretq

ISR_Intterupt_Hdd_2:
     SaveContext

     mov rdi,47
     call ISR_HardDisk

     LoadContext
    iretq

ISR_Intterupt_Other:
     SaveContext

     mov rdi,48
     call Intterupt_Routine

     LoadContext
    iretq

