[bits 64]

extern main

_Entry_IA_32e:
    mov ax,0x10
    mov ds,ax
    mov es,ax
    mov fs,ax
    mov gs,ax
    mov ss,ax
    mov rsp,0x6FFFF8
    mov rsp,0x6FFFF8

    call main

jmp $

