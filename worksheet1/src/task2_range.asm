; Azan Tariq - Worksheet 1
%include "asm_io.inc"

section .data
    array_ready     db  "Array initialized!", 10, 0
    get_start       db  "From index: ", 0
    get_end         db  "To index: ", 0
    show_sum        db  "Partial sum: ", 0
    invalid_from    db  "Invalid start!", 10, 0
    invalid_to      db  "Invalid end!", 10, 0
    invalid_order   db  "Start > end!", 10, 0
    ARRAY_SIZE      equ 100

section .bss
    values          resd ARRAY_SIZE
    from_idx        resd 1
    to_idx          resd 1
    partial_sum     resd 1

section .text
    global _asm_main

_asm_main:
    push    ebp
    mov     ebp, esp
    pushad

    mov     ebx, 1
    lea     edi, [values]
populate:
    mov     dword [edi], ebx
    add     edi, 4
    inc     ebx
    cmp     ebx, ARRAY_SIZE
    jle     populate

    lea     eax, [array_ready]
    call    print_string

    lea     eax, [get_start]
    call    print_string
    call    read_int
    mov     dword [from_idx], eax
    cmp     eax, 1
    jl      bad_from
    cmp     eax, ARRAY_SIZE
    jg      bad_from

    lea     eax, [get_end]
    call    print_string
    call    read_int
    mov     dword [to_idx], eax
    cmp     eax, 1
    jl      bad_to
    cmp     eax, ARRAY_SIZE
    jg      bad_to

    mov     ebx, dword [from_idx]
    cmp     ebx, dword [to_idx]
    jg      bad_order

    xor     eax, eax
    mov     ecx, dword [from_idx]
compute:
    mov     edx, ecx
    dec     edx
    shl     edx, 2
    lea     esi, [values]
    add     esi, edx
    add     eax, dword [esi]
    cmp     ecx, dword [to_idx]
    jge     display
    inc     ecx
    jmp     compute

display:
    mov     dword [partial_sum], eax
    lea     eax, [show_sum]
    call    print_string
    mov     eax, dword [partial_sum]
    call    print_int
    call    print_nl
    jmp     cleanup

bad_from:
    lea     eax, [invalid_from]
    call    print_string
    jmp     cleanup
bad_to:
    lea     eax, [invalid_to]
    call    print_string
    jmp     cleanup
bad_order:
    lea     eax, [invalid_order]
    call    print_string

cleanup:
    popad
    mov     eax, 0
    pop     ebp
    ret

