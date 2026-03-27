; Azan Tariq - Worksheet 1
%include "asm_io.inc"

section .data
    request_name    db  "Enter your name please: ", 0
    request_count   db  "Repetitions (between 50-100): ", 0
    welcome_text    db  "Welcome, ", 0
    end_mark        db  "!", 0
    error_small     db  "Number too small (min 50)", 10, 0
    error_big       db  "Number too big (max 100)", 10, 0
    MIN_VAL         equ 50
    MAX_VAL         equ 100

section .bss
    person_name     resb 64
    rep_count       resd 1
    current_rep     resd 1

section .text
    global _asm_main

_asm_main:
    push    ebp
    mov     ebp, esp
    pushad

    lea     eax, [request_name]
    call    print_string
    
    lea     edi, [person_name]
read_loop:
    call    read_char
    cmp     al, 10
    je      read_done
    cmp     al, 13
    je      read_done
    mov     byte [edi], al
    inc     edi
    jmp     read_loop
read_done:
    mov     byte [edi], 0

    lea     eax, [request_count]
    call    print_string
    call    read_int
    mov     dword [rep_count], eax

    cmp     eax, MIN_VAL
    jl      handle_small
    cmp     eax, MAX_VAL
    jg      handle_big

    mov     dword [current_rep], 0
print_loop:
    mov     ebx, dword [current_rep]
    cmp     ebx, dword [rep_count]
    jge     program_end

    lea     eax, [welcome_text]
    call    print_string
    lea     eax, [person_name]
    call    print_string
    lea     eax, [end_mark]
    call    print_string
    call    print_nl

    inc     dword [current_rep]
    jmp     print_loop

handle_small:
    lea     eax, [error_small]
    call    print_string
    jmp     program_end

handle_big:
    lea     eax, [error_big]
    call    print_string

program_end:
    popad
    mov     eax, 0
    pop     ebp
    ret

