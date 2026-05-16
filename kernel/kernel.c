#include <stddef.h>
#include "kernel.h"
#include "utils/time.h"
#include "utils/init.h"
#include "utils/shell.h"
#include "drivers/vga.h"
#include "utils/colors.h"
#include "arch/i686/gdt/gdt.h"
#include "arch/i686/idt/idt.h"


char user[32] = "root";
long boot_seconds = 0;

void print(const char *str);

void kernel_main(void)
{
    init_gdt();
    init_idt();

    init_system_base();

    shell_main_loop();
    vga_print_color("Shell exited.", LIGHT_RED);
}
