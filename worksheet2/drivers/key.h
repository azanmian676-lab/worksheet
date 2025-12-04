#ifndef KEY_H
#define KEY_H

#define KEY_DATA 0x60
#define KEY_STATUS 0x64

#define PIC1_CTRL 0x20
#define PIC1_DAT 0x21
#define PIC2_CTRL 0xA0
#define PIC2_DAT 0xA1

#define PIC_EOI_CMD 0x20

#define KEY_BS_CODE 0x0E
#define KEY_ENTER_CODE 0x1C
#define KEY_LSHIFT_CODE 0x2A
#define KEY_RSHIFT_CODE 0x36
#define KEY_LSHIFT_UP_CODE 0xAA
#define KEY_RSHIFT_UP_CODE 0xB6

void key_init(void);
char key_get(void);
void key_line(char* buf, int max);
int key_pending(void);

#endif

