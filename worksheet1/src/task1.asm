; Azan Tariq - Worksheet 1
%include "asm_io.inc"

section .data
    first_number    dd  35
    second_number   dd  65
    opening_msg     db  "Computing: ", 0
    plus_symbol     db  " + ", 0
    equals_symbol   db  " = ", 0

section .bss
    computed_sum    resd 1

section .text
    global _asm_main

_asm_main:
    push    ebp
    mov     ebp, esp
    pushad

    lea     eax, [opening_msg]
    call    print_string

    mov     eax, dword [first_number]
    call    print_int

    lea     eax, [plus_symbol]
    call    print_string

    mov     eax, dword [second_number]
    call    print_int

    lea     eax, [equals_symbol]
    call    print_string

    mov     ebx, dword [first_number]
    mov     ecx, dword [second_number]
    add     ebx, ecx
    mov     dword [computed_sum], ebx
    mov     eax, ebx
    call    print_int
    call    print_nl

    popad
    mov     eax, 0
    pop     ebp
    ret

