#ifndef SCREEN_H
#define SCREEN_H

#define SCR_BASE_ADDR 0x000B8000
#define SCR_COLUMNS 80
#define SCR_LINES 25

#define SCR_CMD 0x3D4
#define SCR_DATA 0x3D5
#define SCR_HIGH_REG 14
#define SCR_LOW_REG 15

#define SCR_BLACK 0
#define SCR_BLUE 1
#define SCR_GREEN 2
#define SCR_CYAN 3
#define SCR_RED 4
#define SCR_MAGENTA 5
#define SCR_BROWN 6
#define SCR_LIGHT_GREY 7
#define SCR_DARK_GREY 8
#define SCR_LIGHT_BLUE 9
#define SCR_LIGHT_GREEN 10
#define SCR_LIGHT_CYAN 11
#define SCR_LIGHT_RED 12
#define SCR_LIGHT_MAGENTA 13
#define SCR_YELLOW 14
#define SCR_WHITE 15

void screen_wipe(void);
void screen_pos(unsigned short c, unsigned short l);
unsigned short screen_col(void);
unsigned short screen_line(void);
void screen_color(unsigned char f, unsigned char b);
void screen_char(char ch);
void screen_text(const char* txt);
void screen_number(int num);
void screen_hex(unsigned int num);
void screen_up(void);
void screen_erase(void);

#endif

