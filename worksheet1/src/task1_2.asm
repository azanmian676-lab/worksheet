; Azan Tariq - Worksheet 1
%include "asm_io.inc"

section .data
    prompt_one      db  "Please enter number 1: ", 0
    prompt_two      db  "Please enter number 2: ", 0
    result_label    db  "The sum equals: ", 0

section .bss
    user_num1       resd 1
    user_num2       resd 1
    final_sum       resd 1

section .text
    global _asm_main

_asm_main:
    push    ebp
    mov     ebp, esp
    pushad

    lea     eax, [prompt_one]
    call    print_string
    call    read_int
    mov     dword [user_num1], eax

    lea     eax, [prompt_two]
    call    print_string
    call    read_int
    mov     dword [user_num2], eax

    lea     eax, [result_label]
    call    print_string

    mov     ebx, dword [user_num1]
    add     ebx, dword [user_num2]
    mov     dword [final_sum], ebx
    mov     eax, ebx
    call    print_int
    call    print_nl

    popad
    mov     eax, 0
    pop     ebp
    ret

