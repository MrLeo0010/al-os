#include "vga.h"
#include <stddef.h>

/* globals accessible from other modules */
int cursor_x = 0;
int cursor_y = 0;
unsigned short* const VGA_MEMORY = (unsigned short*)0xB8000;
const int WIDTH = 80;
const int HEIGHT = 25;

static unsigned char current_color = 0x07; // default gray

/* outb helper */
static inline void outb(unsigned short port, unsigned char val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

/* update hardware cursor */
void update_cursor() {
    unsigned short pos = cursor_y * WIDTH + cursor_x;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (unsigned char)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (unsigned char)((pos >> 8) & 0xFF));
}

/* clear screen */
void vga_clear() {
    for (int y = 0; y < HEIGHT; y++)
        for (int x = 0; x < WIDTH; x++)
            VGA_MEMORY[y * WIDTH + x] = ((unsigned short)current_color << 8) | ' ';
    cursor_x = 0;
    cursor_y = 0;
    update_cursor();
}

/* set current color */
void vga_setcolor(unsigned char color) {
    current_color = color;
}

/* scroll up one line */
static void scroll() {
    for (int y = 1; y < HEIGHT; y++)
        for (int x = 0; x < WIDTH; x++)
            VGA_MEMORY[(y - 1) * WIDTH + x] = VGA_MEMORY[y * WIDTH + x];

    for (int x = 0; x < WIDTH; x++)
        VGA_MEMORY[(HEIGHT - 1) * WIDTH + x] = ((unsigned short)current_color << 8) | ' ';

    if (cursor_y > 0) cursor_y = HEIGHT - 1;
    update_cursor();
}

/* put single character (handles newline, wrap, scroll) */
void vga_putc(char c) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else if (c == '\r') {
        cursor_x = 0;
    } else if (c == '\b') {
        if (cursor_x > 0) {
            cursor_x--;
            VGA_MEMORY[cursor_y * WIDTH + cursor_x] = ((unsigned short)current_color << 8) | ' ';
        } else if (cursor_y > 0) {
            cursor_y--;
            cursor_x = WIDTH - 1;
            VGA_MEMORY[cursor_y * WIDTH + cursor_x] = ((unsigned short)current_color << 8) | ' ';
        }
    } else {
        VGA_MEMORY[cursor_y * WIDTH + cursor_x] = ((unsigned short)current_color << 8) | c;
        cursor_x++;
    }

    if (cursor_x >= WIDTH) {
        cursor_x = 0;
        cursor_y++;
    }
    if (cursor_y >= HEIGHT) {
        scroll();
    }
    update_cursor();
}

/* write string without auto newline */
void vga_write(const char* str) {
    while (*str) vga_putc(*str++);
}

/* write string then newline */
void vga_print(const char* str) {
    vga_write(str);
    vga_putc('\n');
}

/* write colored string (temporarily change color) */
void vga_print_color(const char* str, unsigned char color) {
    unsigned char old = current_color;
    vga_setcolor(color);
    vga_write(str);
    vga_setcolor(old);
}
