#ifndef VGA_H
#define VGA_H

void vga_clear();
void vga_putc(char c);
void vga_write(const char* str);
void vga_print(const char* str);
void vga_print_color(const char* str, unsigned char color);
void vga_setcolor(unsigned char color);
void update_cursor();

extern int cursor_x;
extern int cursor_y;
extern unsigned short* const VGA_MEMORY;
extern const int WIDTH;
extern const int HEIGHT;

#endif
