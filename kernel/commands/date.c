#include "all_commands.h"
#include "../kernel.h"
#include "../drivers/vga.h"
#include "../utils/string.h"

void cmd_date(void) {
    rtc_time now;
    rtc_read(&now);

    char buf[64];

    itoa(now.day, buf, 10);
    if (now.day < 10) vga_print("0");
    vga_print(buf);
    vga_print(".");

    itoa(now.month, buf, 10);
    if (now.month < 10) vga_print("0");
    vga_print(buf);
    vga_print(".");

    itoa(now.year, buf, 10);
    vga_print(buf);
    vga_print(" ");

    itoa(now.hour, buf, 10);
    if (now.hour < 10) vga_print("0");
    vga_print(buf);
    vga_print(":");

    itoa(now.min, buf, 10);
    if (now.min < 10) vga_print("0");
    vga_print(buf);
    vga_print(":");

    itoa(now.sec, buf, 10);
    if (now.sec < 10) vga_print("0");
    vga_print(buf);
    vga_putc('\n');
}
