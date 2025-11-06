#include "keyboard.h"
#include "vga.h"

#define KBD_DATA 0x60
#define KBD_STATUS 0x64

static const char keymap[128] = {
    0,27,'1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',0,
    'a','s','d','f','g','h','j','k','l',';','\'','`',0,'\\',
    'z','x','c','v','b','n','m',',','.','/',0,'*',0,' ',
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

static const char keymap_shift[128] = {
    0,27,'!','@','#','$','%','^','&','*','(',')','_','+','\b',
    '\t','Q','W','E','R','T','Y','U','I','O','P','{','}','\n',0,
    'A','S','D','F','G','H','J','K','L',':','"','~',0,'|',
    'Z','X','C','V','B','N','M','<','>','?',0,'*',0,' ',
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

static int shift_pressed = 0;

static inline unsigned char inb(unsigned short port) {
    unsigned char ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static void wait_for_keypress() {
    unsigned char st;
    do { st = inb(KBD_STATUS); } while (!(st & 0x01));
}

static void keyboard_flush() {
    while (inb(KBD_STATUS) & 0x01) inb(KBD_DATA);
}

char keyboard_read_char() {
    wait_for_keypress();
    unsigned char sc = inb(KBD_DATA);

    if (sc & 0x80) {
        if (sc == 0xAA || sc == 0xB6) shift_pressed = 0;
        return 0;
    }

    if (sc == 0x2A || sc == 0x36) {
        shift_pressed = 1;
        return 0;
    }

    if (sc < 128) {
        if (shift_pressed) return keymap_shift[sc];
        return keymap[sc];
    }
    return 0;
}

int keyboard_read_line(char* buffer, int max_len) {
    int pos = 0;
    while (1) {
        char c = keyboard_read_char();
        if (!c) continue;

        if (c == '\n') {
            buffer[pos] = 0;
            vga_putc('\n');
            keyboard_flush();
            return pos;
        }

        if (c == '\b' && pos > 0) {
            pos--;
            if (cursor_x > 0) {
                cursor_x--;
            } else if (cursor_y > 0) {
                cursor_y--;
                cursor_x = WIDTH - 1;
            } else {
                cursor_x = 0;
            }
            VGA_MEMORY[cursor_y * WIDTH + cursor_x] = (0x07 << 8) | ' ';
            update_cursor();
            continue;
        }

        if (c >= 32 && c <= 126 && pos < max_len - 1) {
            buffer[pos++] = c;
            vga_setcolor(0x0A);
            vga_putc(c);
            vga_setcolor(0x07);
        }
    }
}
