#include <stddef.h>
#include "kernel.h"
#include "drivers/vga.h"
#include "utils/time.h"
#include "utils/init.h"
#include "utils/shell.h"
#include "drivers/vga.h"
#include "utils/colors.h"
#include "utils/ports.h"
#include "arch/i686/gdt/gdt.h"
#include "arch/i686/idt/idt.h"
#include "arch/i686/pic/pic.h"
#include "arch/i686/timer/timer.h"



char user[32] = "root";
long boot_seconds = 0;

void tests()
{
    // Говорим PIC1: включить только IRQ0 (таймер) и IRQ1 (клавиатура)
    outb(0x21, 0xFC);
    // Говорим PIC2: отключить пока все его прерывания
    outb(0xA1, 0xFF);

    vga_print("Waiting for 3 seconds...\n");
    sleep(3000);
    vga_print("3 seconds passed! Timer works perfectly!\n");

}

void kernel_main(void)
{
    pic_remap(32, 40);
    init_gdt();
    init_idt();
    init_timer(100);
    __asm__ __volatile__("sti");
    tests();

    init_system_base();

    shell_main_loop();
    vga_print_color("Shell exited.", LIGHT_RED);
}
