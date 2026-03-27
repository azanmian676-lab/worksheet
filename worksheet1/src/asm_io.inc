extern  _read_int, _print_int, _print_string, _read_char, _print_char, _print_nl

%define read_int _read_int
%define print_int _print_int
%define print_string _print_string
%define read_char _read_char
%define print_char _print_char
%define print_nl _print_nl

%macro  dump_regs 1
        push    dword %1
        call    _dump_regs
        add     esp, 4
%endmacro
extern _dump_regs

%macro  dump_mem 3
        push    dword %3
        push    dword %2
        push    dword %1
        call    _dump_mem
        add     esp, 12
%endmacro
extern  _dump_mem

%macro  dump_math 1
        push    dword %1
        call    _dump_math
        add     esp, 4
%endmacro
extern  _dump_math

%macro  dump_stack 3
        push    dword %3
        push    dword %2
        push    dword %1
        call    _dump_stack
        add     esp, 12
%endmacro
extern  _dump_stack
