extern _printf
extern _scanf
extern _putchar
extern _getchar

section .data
    int_format      db "%d", 0
    string_format   db "%s", 0
    read_int_format db "%d", 0
    newline         db 10, 0
    dump_format     db "Register Dump # %d", 10
                    db "EAX = %.8X EBX = %.8X ECX = %.8X EDX = %.8X", 10
                    db "ESI = %.8X EDI = %.8X EBP = %.8X ESP = %.8X", 10
                    db "EIP = %.8X FLAGS = %.4X %s %s %s %s %s", 10, 0
    carry_flag      db "CF", 0
    no_carry        db "  ", 0
    zero_flag       db "ZF", 0
    no_zero         db "  ", 0
    sign_flag       db "SF", 0
    no_sign         db "  ", 0
    parity_flag     db "PF", 0
    no_parity       db "  ", 0
    overflow_flag   db "OF", 0
    no_overflow     db "  ", 0
    mem_format1     db "Memory Dump # %d Address = %.8X", 10, 0
    mem_format2     db "%.8X ", 0
    mem_format3     db "%.2X ", 0
    mem_format4     db "'%c' ", 0
    mem_format5     db "? ", 0
    
section .bss
    int_buffer      resd 1

section .text
    global _read_int, _print_int, _print_string, _read_char, _print_char, _print_nl
    global _dump_regs, _dump_mem, _dump_math, _dump_stack

_read_int:
    enter   0, 0
    pusha
    push    int_buffer
    push    read_int_format
    call    _scanf
    add     esp, 8
    popa
    mov     eax, [int_buffer]
    leave
    ret

_print_int:
    enter   0, 0
    pusha
    push    eax
    push    int_format
    call    _printf
    add     esp, 8
    popa
    leave
    ret

_print_string:
    enter   0, 0
    pusha
    push    eax
    push    string_format
    call    _printf
    add     esp, 8
    popa
    leave
    ret

_read_char:
    enter   0, 0
    push    ebx
    push    ecx
    push    edx
    call    _getchar
    pop     edx
    pop     ecx
    pop     ebx
    leave
    ret

_print_char:
    enter   0, 0
    pusha
    and     eax, 0xFF
    push    eax
    call    _putchar
    add     esp, 4
    popa
    leave
    ret

_print_nl:
    enter   0, 0
    pusha
    push    10
    call    _putchar
    add     esp, 4
    popa
    leave
    ret

_dump_regs:
    enter   0, 0
    pusha
    push    dword [ebp+8]
    push    dump_format
    call    _printf
    add     esp, 8
    popa
    leave
    ret

_dump_mem:
    enter   0, 0
    pusha
    push    dword [ebp+8]
    push    dword [ebp+16]
    push    mem_format1
    call    _printf
    add     esp, 12
    popa
    leave
    ret

_dump_math:
    enter   0, 0
    pusha
    popa
    leave
    ret

_dump_stack:
    enter   0, 0
    pusha
    popa
    leave
    ret
