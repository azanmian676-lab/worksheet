; Azan Tariq - Worksheet 1
%include "asm_io.inc"

section .data
    setup_msg       db  "Setting up array with 1-100", 10, 0
    adding_msg      db  "Adding all values", 10, 0
    final_msg       db  "Final result: ", 0
    ELEMENTS        equ 100

section .bss
    data_array      resd ELEMENTS
    grand_total     resd 1

section .text
    global _asm_main

_asm_main:
    push    ebp
    mov     ebp, esp
    pushad

    lea     eax, [setup_msg]
    call    print_string

    mov     ebx, 1
    lea     edi, [data_array]
setup_loop:
    mov     dword [edi], ebx
    add     edi, 4
    inc     ebx
    cmp     ebx, ELEMENTS
    jle     setup_loop

    lea     eax, [adding_msg]
    call    print_string

    xor     ebx, ebx
    xor     ecx, ecx
    lea     esi, [data_array]
add_loop:
    add     ebx, dword [esi]
    add     esi, 4
    inc     ecx
    cmp     ecx, ELEMENTS
    jl      add_loop

    mov     dword [grand_total], ebx

    lea     eax, [final_msg]
    call    print_string
    mov     eax, dword [grand_total]
    call    print_int
    call    print_nl

    popad
    mov     eax, 0
    pop     ebp
    ret

