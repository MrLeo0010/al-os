#include "../drivers/vga.h"
#include "all_commands.h"
#include "../drivers/keyboard.h"
#include "../utils/colors.h"


static void slowprint_line(const char* str, uint8_t color, unsigned int delay) {
    uint8_t old = vga_color;
    vga_color = color;
    for (int i = 0; str[i]; i++) {
        vga_putc(str[i]);
        for (volatile unsigned int j = 0; j < delay; j++) {
            if ((j & 0xFFF) == 0) {
                keyboard_poll();
                if (keyboard_sigint_check()) {
                    vga_print_color("\nOperation cancelled\n", LIGHT_RED);
                    vga_color = old;
                    return;
                }
            }
        }
    }
    vga_putc('\n');
    vga_color = old;
}

void cmd_slowfetch(void) {
    slowprint_line("", 0x0B, 50000);
    slowprint_line("      _    _         ___  ____  ", 0x0B, 50000);
    slowprint_line("     / \\  | |       / _ \\/ ___| ", 0x0B, 50000);
    slowprint_line("    / _ \\ | |      | | | \\___ \\ ", 0x0B, 50000);
    slowprint_line("   / ___ \\| |___   | |_| |___) |", 0x0B, 50000);
    slowprint_line("  /_/   \\_\\_____|   \\___/|____/ ", 0x0B, 50000);
    slowprint_line("", 0x07, 50000);
    cmd_sysinfo();
}
