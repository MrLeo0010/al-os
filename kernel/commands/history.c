#include "../drivers/vga.h"
#include "../kernel.h"
#include "../utils/string.h"

void cmd_history(void) {
    if (history_count == 0) {
        vga_print_color("History is empty\n", 0x0E);
        return;
    }
    char num[8];
    for (int i = 0; i < history_count; i++) {
        if (!history[i][0]) continue;
        itoa(i + 1, num, 10);
        vga_print_color(num, 0x08);
        vga_print_color("  ", 0x08);
        vga_print_color(history[i], 0x0F);
        vga_putc('\n');
    }
}
