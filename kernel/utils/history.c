#include "history.h"
#include "../utils/colors.h"


char history[HISTORY_SIZE][HISTORY_LEN] = {0};
int history_count = 0;
int history_nav = -1;
void print(const char *str);

#include "../drivers/vga.h"
#include "../kernel.h"
#include "../utils/string.h"

void print_history(void) {
    if (history_count == 0) {
        vga_print_color("History is empty\n", YELLOW);
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
