[org 0]
[bits 16]

SECTION .text

jmp 0x07c0:Start

Start:
    mov bx,cs
    mov ds,bx ;ds에 현재 주소를 옮겨줏어야 한다-> 그래야 변수들의 값을 참조할 수 있음
    mov bx ,0xB800
    mov es ,bx

_Stack:
    mov ax,0x0000
    mov ss,ax
    mov sp,0xFFFE  ;64kb

mov si,0

_ClearScreen:
    mov byte [es:si], 0
    inc si
    mov byte [es:si], 0x07
    inc si

    cmp si,2000*2
    jl _ClearScreen

_BootStart:
    mov bx,0xb800
    mov es,bx

    mov di,BootingMessage
    call _PrintMessage

_DiskReset:
    mov di,BIOSserviceMessage
    call _PrintMessage

    mov ah,0x0
    mov dl,0x80
    int 0x13
    jc _ErrorOccured

    mov ax,0x500
    mov es,ax
_LoadSectors:
    mov ah,0x10
    mov [es:0],ah

    mov ah,0
    mov [es:1],ah

    mov ax,word[OsImageSize_64]
    mov [es:2],ax

    mov ax,0x0000
    mov [es:4],ax

    mov ax,0x1000
    mov [es:6],ax

    mov eax,1
    mov [es:8],eax

    mov ax,0x500
    mov ds,ax
    mov ax,0
    mov si,ax

    mov ah,0x42
    mov dl,0x80
    int 0x13

    mov bx,cs
    mov ds,bx

    jc _ErrorOccured

_Success:
    mov di,SuccessMessage
    call _PrintMessage

    jmp 0x1000:0000

_ErrorOccured:
    mov di,ErrorMessage
    call _PrintMessage

    jmp $

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
_PrintMessage:
    push es
    push si
    push ax
    push cx
    push dx

    mov si,0xB800
    mov es,si

    mov ax,word[PrintLine]
    mov si,160
    mul si
    mov si,ax
_MessageLoop:
    mov cl,byte[di]
    cmp cl,0
    jz _MessageEnd

    mov byte[es:si],cl
    inc di
    add si,2

    jmp _MessageLoop
_MessageEnd:
    inc word[PrintLine]
    pop dx
    pop cx
    pop ax
    pop si
    pop es
    ret

BootingMessage db 'OS is Booting... Please Wait...',0
BIOSserviceMessage db 'Executing BIOS service...',0
SuccessMessage db 'Load Os image Successfully!',0
ErrorMessage db 'Error is occured while load Os image!',0

OsImageSize dw 5
OsImageSize_64 dw 93

IsFormated db 0

PrintLine dw 0

times 510-($-$$) db 0
dw 0xAA55; 리틀 엔디안->x86_64(AMD,Intel)-> 따라서 바이트 단위로 거꾸로 들어감
