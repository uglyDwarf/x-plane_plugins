
BITS 64

section .data
        global trampoline1
align 4096
trampoline1:
        times 64 db 0


section .text
        extern kuk

%macro HOOK 1
        global hook%1

hook%1:
        ;rax is saved as a part of function redirection
        ;(it is used to store the full 64 bit hook address)

        ;no 64bit pushad analog, have to save
        ;registers manually
        push rcx
        push rdx
        push r8
        push r9
        push r10
        push r11
        push rdi
        push rsi
        ;first params are passed in registers,
        ;so no need to copy anything
        mov rax, qword [rel kuk wrt ..gotpc]
        call [rax]
        ;restore the registers
        pop rsi
        pop rdi
        pop r11
        pop r10
        pop r9
        pop r8
        pop rdx
        pop rcx
        pop rax
        ;now lets jump to our trampoline
        jmp [rel trampoline%1 wrt ..gotpc]

%endmacro

        HOOK 1

