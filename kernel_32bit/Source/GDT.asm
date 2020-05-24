[org 0x10000]
[bits 16]

jmp _Start

_Start:
    mov ax,cs
    mov ds,ax

_BIOSint:
    mov ax,0x2401
    int 0x15
    jc _SysCtrlPort
    mov byte[IsOnA20],1
    jmp _RealMode

_SysCtrlPort:
    in al,0x92
    or al,0x2
    and al,0xFE
    out 0x92,al

    call CheckA20
    cmp byte[IsOnA20],0
    jz _KeyBoardController
    jmp _RealMode

_KeyBoardController:
    cli

    call CheckInput
    mov al,0xAD
    out 0x64,al

    call CheckInput
    mov al,0xD0
    out 0x64,al

    call CheckOutput
    in al,0x60
    push ax

    call CheckInput
    mov al,0xD1
    out 0x64,al

    call CheckInput
    pop ax
    or al,2
    out 0x60,al

    call CheckInput
    mov al,0xAE
    out 0x64,al

    sti
    call CheckA20
    jmp _RealMode

    CheckInput:
        in al,0x64
        test al,2
        jnz CheckInput
        ret
    CheckOutput:
        in al,0x64
        test al,1
        jz CheckOutput
        ret

CheckA20:
    push es
    push ax
    push si

    mov ax,0x0
    mov es,ax
    mov si,0x500
    mov word[es:si],0x0000

    mov ax,0xFFFF
    mov es,ax
    mov si,0x510
    mov word[es:si],0x0000

    mov ax,0xFFFF
    mov word[es:si],ax

    mov ax,0x0
    mov es,ax
    mov si,0x500

    mov ax,word[es:si]
    cmp ax,0xFFFF
    jz _NotOnA20

    _OnA20:
        pop si
        pop ax
        pop es
        mov byte[IsOnA20],1
    ret
    _NotOnA20:
        pop si
        pop ax
        pop es
        mov byte[IsOnA20],0
    ret

_RealMode:
cli
lgdt[GDTR]

mov eax,0x4000003B
mov cr0,eax

jmp $+2
nop
nop

jmp dword KernelCodeDescriptor:_ProtectMode

[bits 32]
_ProtectMode:
    mov ax,KernelDataDescriptor
    mov es,ax
    mov ds,ax
    mov fs,ax
    mov gs,ax
    mov ss,ax
    mov esp,0xfffe
    mov ebp,0xfffe

    cmp byte[IsOnA20],0
    jz _ErrorOccured
    mov edi,SuccessMessage
    call _PrintMessage

    jmp dword KernelCodeDescriptor:0x10200

_ErrorOccured:
    mov edi,ErrorMessage
    call _PrintMessage
    jmp $

_PrintMessage:
    push es
    push esi
    push eax

    mov eax,VideoDescriptor
    mov es,eax

    mov esi,160*4

_PrintLoop:
    mov al,byte[edi]
    cmp al,0
    jz _MessageEnd

    mov byte[es:esi],al
    mov byte[es:esi+1],0x09
    inc edi
    add esi,2

    jmp _PrintLoop
_MessageEnd:
    pop eax
    pop esi
    pop es
    ret

GDTR:
    dw GDT_END-GDT-1
    dd GDT

align 8

GDT:
NullDescriptor equ 0x00
    dw 0x0 ; 세그먼트 크기
    dw 0x0 ; 기준 주소 16~31(16bit)
    db 0x0 ; 기준 주소 32~39(8bit)
    db 0x0 ; P(1bit), DPL(2bit), S(1bit) + type(4bit) = (8bit)
    db 0x0 ; G, D/B, L, AVL(4bit) + 세그먼트 크기 36~39(4bit)=(8bit)
    db 0x0 ; 기준 주소 63~56(8bit)
IA_32e_CodeDescriptor equ 0x08
    dw 0x0000
    dw 0x0000
    db 0x00
    db 0x9A ; 1001 1010 | P=1 | DPL=0 | S=1 (Code&Data Segment) | type=1010 (non-conforming,readable)
    db 0x20
    db 0x00
IA_32e_DataDescriptor equ 0x10
    dw 0x0000
    dw 0x0000
    db 0x00
    db 0x92 ; 1001 0010 | P=1 | DPL=0 | S=1 (Code&Data Segment) | type=0010 (read&Write)
    db 0x20
    db 0x00
KernelCodeDescriptor equ 0x18
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0x9A ; 1001 1010 | P=1 | DPL=0 | S=1 (Code&Data Segment) | type=1010 (non-conforming,readable)
    db 0xCF ; 1100 1111 | G=1 | D/B=1 | L=0(32bit) | AVL=0 | Segment Size=0xFFFF*0xF=0xFFFFF,0xFFFFF*0xFFF(4KB=4096B)=0xFFFFFFF
    db 0x00
KernelDataDescriptor equ 0x20
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0x92 ; 1001 0010 | P=1 | DPL=0 | S=1 (Code&Data Segment) | type=0010 (read&Write)
    db 0xCF ; 1100 1111 | G=1 | D/B=1 | L=0(32bit) | AVL=0 | Segment Size=0xFFFF*0xF=0xFFFFF,0xFFFFF*0xFFF(4KB=4096B)=0xFFFFFFF
    db 0x00
VideoDescriptor equ 0x28
    dw 0xFFFF
    dw 0x8000
    db 0x0B
    db 0x92
    db 0xCF
    db 0x00
GDT_END:

SuccessMessage db 'Success turning A20 GATE on!',0
ErrorMessage db 'Error is occured while turning A20 GATE on!',0

IsOnA20 db 0

times 512-($-$$) db 0
