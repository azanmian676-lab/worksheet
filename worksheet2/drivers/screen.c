#include "screen.h"

extern void io_write(unsigned short port, unsigned char data);
extern unsigned char io_read(unsigned short port);

static unsigned short col = 0;
static unsigned short line = 0;

static unsigned char fg = SCR_WHITE;
static unsigned char bg = SCR_BLACK;

static char* mem = (char*)SCR_BASE_ADDR;

static unsigned char make_byte(unsigned char f, unsigned char b) {
    return (b << 4) | (f & 0x0F);
}

static void cursor_hw(void) {
    unsigned short position = line * SCR_COLUMNS + col;
    io_write(SCR_CMD, SCR_HIGH_REG);
    io_write(SCR_DATA, (position >> 8) & 0xFF);
    io_write(SCR_CMD, SCR_LOW_REG);
    io_write(SCR_DATA, position & 0xFF);
}

void screen_wipe(void) {
    unsigned char byte = make_byte(fg, bg);
    for (int i = 0; i < SCR_COLUMNS * SCR_LINES; i++) {
        mem[i * 2] = ' ';
        mem[i * 2 + 1] = byte;
    }
    col = 0;
    line = 0;
    cursor_hw();
}

void screen_pos(unsigned short c, unsigned short l) {
    if (c < SCR_COLUMNS) col = c;
    if (l < SCR_LINES) line = l;
    cursor_hw();
}

unsigned short screen_col(void) {
    return col;
}

unsigned short screen_line(void) {
    return line;
}

void screen_color(unsigned char f, unsigned char b) {
    fg = f;
    bg = b;
}

void screen_up(void) {
    unsigned char byte = make_byte(fg, bg);
    
    for (int i = 0; i < (SCR_LINES - 1) * SCR_COLUMNS; i++) {
        mem[i * 2] = mem[(i + SCR_COLUMNS) * 2];
        mem[i * 2 + 1] = mem[(i + SCR_COLUMNS) * 2 + 1];
    }
    
    for (int i = (SCR_LINES - 1) * SCR_COLUMNS; i < SCR_LINES * SCR_COLUMNS; i++) {
        mem[i * 2] = ' ';
        mem[i * 2 + 1] = byte;
    }
}

void screen_char(char ch) {
    unsigned char byte = make_byte(fg, bg);
    
    if (ch == '\n') {
        col = 0;
        line++;
    } else if (ch == '\r') {
        col = 0;
    } else if (ch == '\t') {
        col = (col + 8) & ~7;
    } else {
        unsigned int position = (line * SCR_COLUMNS + col);
        mem[position * 2] = ch;
        mem[position * 2 + 1] = byte;
        col++;
    }
    
    if (col >= SCR_COLUMNS) {
        col = 0;
        line++;
    }
    
    if (line >= SCR_LINES) {
        screen_up();
        line = SCR_LINES - 1;
    }
    
    cursor_hw();
}

void screen_text(const char* txt) {
    while (*txt) {
        screen_char(*txt);
        txt++;
    }
}

void screen_number(int num) {
    char buf[12];
    int count = 0;
    int negative = 0;
    
    if (num == 0) {
        screen_char('0');
        return;
    }
    
    if (num < 0) {
        negative = 1;
        num = -num;
    }
    
    while (num > 0) {
        buf[count++] = '0' + (num % 10);
        num /= 10;
    }
    
    if (negative) {
        screen_char('-');
    }
    
    while (count > 0) {
        screen_char(buf[--count]);
    }
}

void screen_hex(unsigned int num) {
    char digits[] = "0123456789ABCDEF";
    char buf[9];
    int count = 0;
    
    screen_text("0x");
    
    if (num == 0) {
        screen_char('0');
        return;
    }
    
    while (num > 0) {
        buf[count++] = digits[num & 0xF];
        num >>= 4;
    }
    
    while (count > 0) {
        screen_char(buf[--count]);
    }
}

void screen_erase(void) {
    if (col > 0) {
        col--;
    } else if (line > 0) {
        line--;
        col = SCR_COLUMNS - 1;
    }
    
    unsigned int position = line * SCR_COLUMNS + col;
    mem[position * 2] = ' ';
    mem[position * 2 + 1] = make_byte(fg, bg);
    cursor_hw();
}

