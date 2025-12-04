global io_write
global io_read

io_write:
    mov al, [esp + 8]
    mov dx, [esp + 4]
    out dx, al
    ret

io_read:
    mov dx, [esp + 4]
    in al, dx
    ret

