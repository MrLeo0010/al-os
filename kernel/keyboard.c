// kernel/keyboard.c — полностью рабочий с NumPad + NumLock + Shift
#include "keyboard.h"
#include "vga.h"
#include <stdbool.h>

#define KBD_DATA   0x60
#define KBD_STATUS 0x64

static bool shift_pressed = false;
static bool numlock_on    = false;   // по умолчанию выключен, как в реальной системе

// Основная раскладка (US QWERTY)
static const char keymap[128] = {
/* 0x00 */ 0,    27,   '1',  '2',  '3',  '4',  '5',  '6',  '7',  '8',  '9',  '0',  '-',  '=',  '\b', '\t',
/* 0x10 */ 'q',  'w',  'e',  'r',  't',  'y',  'u',  'i',  'o',  'p',  '[',  ']',  '\n', 0,    'a',  's',
/* 0x20 */ 'd',  'f',  'g',  'h',  'j',  'k',  'l',  ';',  '\'', '`',  0,    '\\', 'z',  'x',  'c',  'v',
/* 0x30 */ 'b',  'n',  'm',  ',',  '.',  '/',  0,    '*',  0,    ' ',  0,    0,    0,    0,    0,    0,
/* 0x40 */ 0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
/* 0x50 */ 0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
/* 0x60 */ 0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
/* 0x70 */ 0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0
};

static const char keymap_shift[128] = {
/* 0x00 */ 0,    27,   '!',  '@',  '#',  '$',  '%',  '^',  '&',  '*',  '(',  ')',  '_',  '+',  '\b', '\t',
/* 0x10 */ 'Q',  'W',  'E',  'R',  'T',  'Y',  'U',  'I',  'O',  'P',  '{',  '}',  '\n', 0,    'A',  'S',
/* 0x20 */ 'D',  'F',  'G',  'H',  'J',  'K',  'L',  ':',  '"',  '~',  0,    '|',  'Z',  'X',  'C',  'V',
/* 0x30 */ 'B',  'N',  'M',  '<',  '>',  '?',  0,    '*',  0,    ' ',  0,    0,    0,    0,    0,    0,
/* 0x40 */ 0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
/* 0x50 */ 0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
/* 0x60 */ 0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
/* 0x70 */ 0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0
};

// NumPad при NumLock ON — цифры и операции
static const char numpad_on[128] = {
    [0x47] = '7', [0x48] = '8', [0x49] = '9',
    [0x4B] = '4', [0x4C] = '5', [0x4D] = '6',
    [0x4F] = '1', [0x50] = '2', [0x51] = '3',
    [0x52] = '0', [0x53] = '.',
    [0x4E] = '+',
    [0x4A] = '-',
    [0x37] = '*',
    [0x35] = '/',   // NumPad /
    [0x1C] = '\n'   // Enter на NumPad
};

static unsigned char inb(unsigned short port) {
    unsigned char ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static void wait_for_keypress() {
    while ((inb(KBD_STATUS) & 1) == 0);
}

char keyboard_read_char() {
    wait_for_keypress();
    unsigned char sc = inb(KBD_DATA);

    // --- Обработка break-кодов (отпускание клавиши) ---
    if (sc & 0x80) {
        sc &= 0x7F;  // получаем make-код
        if (sc == 0x2A || sc == 0x36) shift_pressed = false;  // Left/Right Shift
        return 0;
    }

    // --- Make-коды ---
    // Shift
    if (sc == 0x2A || sc == 0x36) {
        shift_pressed = true;
        return 0;
    }

    // NumLock toggle
    if (sc == 0x45) {
        numlock_on = !numlock_on;
        return 0;
    }

    // --- NumPad при включённом NumLock ---
    if (numlock_on && numpad_on[sc]) {
        return numpad_on[sc];
    }

    // --- Обычные клавиши ---
    if (sc < 128 && keymap[sc]) {
        return shift_pressed ? keymap_shift[sc] : keymap[sc];
    }

    return 0;  // неизвестная клавиша
}

void vga_clear_line_from_cursor() {
    int cur = vga_get_cursor();
    int x = cur % VGA_WIDTH;
    int y = cur / VGA_WIDTH;
    for (int i = x; i < VGA_WIDTH; i++)
        vga_put_at(' ', 0x07, y * VGA_WIDTH + i);
}

void keyboard_read_line(char* buffer, int max_len) {
    int len = 0;
    int pos = 0;

    while (1) {
        char c = keyboard_read_char();
        if (c == '\n') {
            buffer[len] = '\0';
            vga_putc('\n');
            break;
        }
        else if (c == '\b') {
            if (pos > 0) {
                pos--; len--;
                for (int i = pos; i < len; i++)
                    buffer[i] = buffer[i + 1];
                buffer[len] = '\0';

                uint16_t cur = vga_get_cursor();
                if (cur % VGA_WIDTH == 0 && cur >= VGA_WIDTH) {
                    vga_set_cursor(cur - VGA_WIDTH);
                    vga_clear_line_from_cursor();
                    vga_set_cursor(cur - VGA_WIDTH);
                } else if (cur > 0) {
                    vga_set_cursor(cur - 1);
                }

                vga_clear_line_from_cursor();
                vga_set_cursor(cur - 1 - (len - pos));
                for (int i = pos; i < len; i++)
                    vga_putc(buffer[i]);
                vga_putc(' ');
                vga_set_cursor(cur - 1 - (len - pos));
            }
        }
        else if (c != 0 && len < max_len - 1) {
            for (int i = len; i > pos; i--)
                buffer[i] = buffer[i - 1];
            buffer[pos] = c;
            len++; pos++;

            uint16_t cur = vga_get_cursor();
            vga_putc(c);
            for (int i = pos; i < len; i++)
                vga_putc(buffer[i]);
            vga_putc(' ');
            vga_set_cursor(cur + 1);
        }
    }
}

void keyboard_init() {
    // Ничего не нужно — NumLock выключен по умолчанию
}