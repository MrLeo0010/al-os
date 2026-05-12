#include "../drivers/vga.h"
#include "../utils/colors.h"
#include "all_commands.h"

void cmd_aarch() {
    vga_print_color("Architecture: i686\n", YELLOW);
    vga_print_color("Mode: 32-bit\n", 0x0F);
    vga_print_color("Endianness: little\n", 0x0F);
}
