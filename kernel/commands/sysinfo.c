#include "../drivers/vga.h"
#include "all_commands.h"

void cmd_sysinfo(void) {
    vga_print_color("=== AL-OS ===\n", 0x0D);
    vga_print_color("Arch: i686\nBuild: v0.4.2 - bugfixes & improved API\n", 0x0F);
}
