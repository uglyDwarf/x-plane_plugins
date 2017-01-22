
BITS 32

section .data
        ; these hold addresses of the first few
        ; bytes of the hooked functions
        global trampoline1
align 4096
trampoline1:
        times 64 db 0

section .text
        extern kuk

%macro HOOK 1
        global hook%1
hook%1:
        ;save all registers
        pushad
        ;copy the first 4 function parameters
        ;  should be enough
        mov eax, [esp+0x30]
        push eax
        mov eax, [esp+0x30]
        push eax
        mov eax, [esp+0x30]
        push eax
        mov eax, [esp+0x30]
        push eax
        call [kuk]
        ;clean up the stack
        pop eax
        pop eax
        pop eax
        pop eax
        ;restore the registers
        popad
        ;jump to the first few bytes of the function
        jmp trampoline%1
%endmacro

        HOOK 1


