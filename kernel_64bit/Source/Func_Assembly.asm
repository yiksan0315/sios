[bits 64]

global WritePort_Out,ReadPort_In,Read_Rflags,WritePort_Out_word,ReadPort_In_word
global stts,clts,SaveFPU,LoadFPU,SwitchContext,cpuid_

cpuid_:
    push rax
    push rbx
    push rcx
    push rdx
    push rsi

    mov eax,edi
    cpuid

    pop rsi
    mov dword[esi],ebx
    mov rax,rdx
    pop rdx
    mov dword[edx],ecx
    pop rcx
    mov dword[ecx],eax

    pop rbx
    pop rax
    ret

WritePort_Out:
    push rax
    push rdx

    mov rdx,rdi
    mov rax,rsi

    out dx,al

    pop rdx
    pop rax

    ret

ReadPort_In:
    push rdx

    mov rax,0
    mov rdx,rdi

    in al,dx

    pop rdx
    ret

WritePort_Out_word:
    push rax
    push rdx

    mov rdx,rdi
    mov rax,rsi

    out dx,al

    pop rdx
    pop rax

    ret

ReadPort_In_word:
    push rdx

    mov rax,0
    mov rdx,rdi

    in al,dx

    pop rdx
    ret

Read_Rflags:
    pushfq
    pop rax

    ret

stts:
    push rax

    mov rax,cr0
    or rax,0x08
    mov cr0,rax

    pop rax
    ret

clts:
    clts
    ret

SaveFPU:
    fxsave[rdi]
    ret

LoadFPU:
    fxrstor[rdi]
    ret

SwitchContext:
    pushfq
    cmp rdi,0
    popfq
    je LoadContext

    push rax

    mov ax,ss
    mov qword[rdi+(23*8)],rax

    mov rax,rsp
    add rax,16
    mov qword[rdi+(22*8)],rax

    pushfq
    pop rax
    mov qword[rdi+(21*8)],rax ;rflags

    mov ax,cs
    mov qword[rdi+(20*8)],rax

    mov rax,qword[rsp+8]
    mov qword[rdi+(19*8)],rax ;rip
    pop rax

    mov qword[rdi+(18*8)],rbp

    mov qword[rdi+(17*8)],rax
    mov qword[rdi+(16*8)],rbx
    mov qword[rdi+(15*8)],rcx
    mov qword[rdi+(14*8)],rdx

    mov qword[rdi+(13*8)],rdi
    mov qword[rdi+(12*8)],rsi

    mov qword[rdi+(11*8)],r8
    mov qword[rdi+(10*8)],r9
    mov qword[rdi+(9*8)],r10
    mov qword[rdi+(8*8)],r11
    mov qword[rdi+(7*8)],r12
    mov qword[rdi+(6*8)],r13
    mov qword[rdi+(5*8)],r14
    mov qword[rdi+(4*8)],r15

    mov ax,ds
    mov qword[rdi+(3*8)],rax
    mov ax,es
    mov qword[rdi+(2*8)],rax
    mov ax,fs
    mov qword[rdi+8],rax
    mov ax,gs
    mov qword[rdi],rax
LoadContext:
    mov rbp,qword[rsi+(18*8)]

    mov rbx,qword[rsi+(16*8)]
    mov rcx,qword[rsi+(15*8)]
    mov rdx,qword[rsi+(14*8)]

    mov rdi,qword[rsi+(13*8)]

    mov r8,qword[rsi+(11*8)]
    mov r9,qword[rsi+(10*8)]
    mov r10,qword[rsi+(9*8)]
    mov r11,qword[rsi+(8*8)]
    mov r12,qword[rsi+(7*8)]
    mov r13,qword[rsi+(6*8)]
    mov r14,qword[rsi+(5*8)]
    mov r15,qword[rsi+(4*8)]

    mov rax,qword[rsi+(3*8)]
    mov ds,ax
    mov rax,qword[rsi+(2*8)]
    mov es,ax
    mov rax,qword[rsi+8]
    mov fs,ax
    mov rax,qword[rsi]
    mov gs,ax

    mov rax,qword[rsi+(17*8)]

    add rsi,(19*8)
    mov rsp,rsi
    sub rsi,(19*8)
    mov rsi,qword[rsi+(12*8)]
    iretq
