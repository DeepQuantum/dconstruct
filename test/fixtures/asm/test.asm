section .text
global _start

_start:
    call test
    retn
test:
    cmp rdi, 1  
    je  C

B:
    add rax, 3
    cmp rsi, 2 
    je  C
D:
    add rax, 4
    jmp E

C:
    add rax, 2

E:
    add rax, 3
    mov DWORD [rsi], 53
    retn




