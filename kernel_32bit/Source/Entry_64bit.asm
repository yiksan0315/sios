[bits 32]

global _CPUID,Switch_IA_32e_Mode

_CPUID:
    push ebp
    mov ebp,esp
    push eax
    push ebx
    push ecx
    push edx
    push esi

    mov eax,dword[ebp+8]
    cpuid

    mov esi,dword[ebp+12]
    mov dword[esi],eax

     mov esi,dword[ebp+16]
    mov dword[esi],ebx

    mov esi,dword[ebp+20]
    mov dword[esi],ecx

    mov esi,dword[ebp+24]
    mov dword[esi],edx

    pop esi
    pop edx
    pop ecx
    pop ebx
    pop eax
    pop ebp
    ret

Switch_IA_32e_Mode:
    mov eax,cr4
    or eax,0x20
    mov cr4,eax

    mov eax,0x100000
    mov cr3,eax

    mov ecx,0xC0000080
    rdmsr
    or eax,0x0100
    wrmsr

    mov eax,cr0
    and eax,0x0000003B ;페이징 활성화시 오류가 남
    or eax,0x80000000
    mov cr0,eax
    jmp 0x08:0x200000

jmp $
