#include "key.h"
#include "screen.h"

extern void io_write(unsigned short port, unsigned char data);
extern unsigned char io_read(unsigned short port);

static int shift_flag = 0;

static char normal_keys[] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0,
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' '
};

static char shift_keys[] = {
    0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 0,
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
    '*', 0, ' '
};

void key_init(void) {
    io_write(PIC1_CTRL, 0x11);
    io_write(PIC2_CTRL, 0x11);
    
    io_write(PIC1_DAT, 0x20);
    io_write(PIC2_DAT, 0x28);
    
    io_write(PIC1_DAT, 0x04);
    io_write(PIC2_DAT, 0x02);
    
    io_write(PIC1_DAT, 0x01);
    io_write(PIC2_DAT, 0x01);
    
    io_write(PIC1_DAT, 0xFD);
    io_write(PIC2_DAT, 0xFF);
}

int key_pending(void) {
    return (io_read(KEY_STATUS) & 0x01);
}

char key_get(void) {
    unsigned char code;
    char ch = 0;
    
    while (1) {
        while (!(io_read(KEY_STATUS) & 0x01));
        
        code = io_read(KEY_DATA);
        
        if (code == KEY_LSHIFT_CODE || code == KEY_RSHIFT_CODE) {
            shift_flag = 1;
            continue;
        }
        if (code == KEY_LSHIFT_UP_CODE || code == KEY_RSHIFT_UP_CODE) {
            shift_flag = 0;
            continue;
        }
        
        if (code & 0x80) {
            continue;
        }
        
        if (code < sizeof(normal_keys)) {
            if (shift_flag) {
                ch = shift_keys[code];
            } else {
                ch = normal_keys[code];
            }
            
            if (ch != 0) {
                return ch;
            }
        }
        
        if (code == KEY_BS_CODE) {
            return '\b';
        }
    }
}

void key_line(char* buf, int max) {
    int pos = 0;
    char ch;
    
    while (pos < max - 1) {
        ch = key_get();
        
        if (ch == '\n') {
            screen_char('\n');
            break;
        } else if (ch == '\b') {
            if (pos > 0) {
                pos--;
                screen_erase();
            }
        } else if (ch >= ' ') {
            buf[pos++] = ch;
            screen_char(ch);
        }
    }
    
    buf[pos] = '\0';
}

