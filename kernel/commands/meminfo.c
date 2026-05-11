#include "all_commands.h"
#include "../utils/string.h"
#include "../drivers/vga.h"

void meminfo_cmd()
{
    extern char _start; extern char end;
    char buf[32];
    itoa((int)&_start, buf, 16); vga_print_color("kernel start: 0x", 0x0E); vga_print_color(buf, 0x0F); vga_putc('\n');
    itoa((int)&end, buf, 16); vga_print_color("kernel end: 0x", 0x0E); vga_print_color(buf, 0x0F); vga_putc('\n');
    int ksize = (int)&end - (int)&_start; itoa(ksize, buf, 10); vga_print_color("size: ", 0x0E); vga_print_color(buf, 0x0F); vga_putc('\n');
}
