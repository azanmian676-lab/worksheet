global start
extern core_main

MB_SIGNATURE equ 0x1BADB002
MB_OPTIONS equ 0x0
MB_CHECKSUM equ -MB_SIGNATURE

STACK_BYTES equ 4096

section .bss
align 4
stack_memory:
    resb STACK_BYTES

section .text
align 4
    dd MB_SIGNATURE
    dd MB_OPTIONS
    dd MB_CHECKSUM

start:
    mov esp, stack_memory + STACK_BYTES
    mov eax, 0xCAFEBABE
    call core_main

infinite:
    cli
    hlt
    jmp infinite

