.code
PUBLIC extended_opcode_switch
extended_opcode_switch PROC
    sub     r14d, 60h
    lea     rax, jump_table
    jmp     QWORD PTR [rax + r14*8]

;; regs we can destroy: r14, r15, rsi, rbx, rdi, rax
; r14 = opcode
; rsi = destination
; rbx = value to check register 
; rdi = symbol table start (low)

QEX_InRangeI_handler::
    mov rdx, QWORD PTR [rbp + rbx*8 - 38h]  ; value to check

    mov rcx, QWORD PTR [rsp + 38h]          ; load symbol table pointer 
    mov rax, QWORD PTR [rcx + rdi*8]        ; lower bound
    mov rcx, QWORD PTR [rcx + rdi*8 + 8]    ; upper bound


    sub rdx, rax                            ; value - lo
    sub rcx, rax                            ; hi - lo
    xor eax, eax                            ; zero rax (also clears upper 32)
    cmp rdx, rcx
    setbe al                                ; al = (value-lo) u<= (hi-lo)
    mov [rbp + rsi*8 - 38h], rax
    ret
extended_opcode_switch ENDP

ALIGN 8
jump_table LABEL QWORD
    QWORD QEX_InRangeI_handler
END