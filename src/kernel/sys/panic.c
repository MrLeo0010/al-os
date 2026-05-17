#include <stdint.h>
#include "../drivers/vga/vga.h"
#include "../utils/ports.h"


#define BLUE_BG_WHITE 0x1F
#define BLUE_BG_RED 0x1C
#define BLUE_BG_YELLOW 0x1E

// Функция задержки в миллисекундах БЕЗ использования прерываний
void panic_sleep_ms(uint32_t ms) {
    for (uint32_t i = 0; i < ms; i++) {
        uint8_t low1 = inb(0x40);
        uint8_t high1 = inb(0x40);
        uint16_t count1 = (high1 << 8) | low1;

        while (1) {
            uint8_t low2 = inb(0x40);
            uint8_t high2 = inb(0x40);
            uint16_t count2 = (high2 << 8) | low2;

            int32_t diff = (int32_t)count1 - (int32_t)count2;
            if (diff < 0) {
                diff += 65536;
            }

            // Увеличиваем лимит в 2 раза, чтобы компенсировать Режим 3 PIT
            if (diff >= 2386) {
                break;
            }

            asm volatile("pause");
        }
    }
}

__attribute__((noreturn))
void panic(const char *module, const char *reason, const char *function) {
    asm volatile("cli");

    fill_screen_with_color(BLUE_BG_WHITE);

    vga_print_centered("KERNEL PANIC", 6, BLUE_BG_RED);

    char msg[96] = {0};
    int pos = 0;
    if (module && *module) {
        while (*module && pos < 70) msg[pos++] = *module++;
        if (pos < 70) msg[pos++] = ':';
        if (pos < 70) msg[pos++] = ' ';
    }
    if (reason && *reason) {
        while (*reason && pos < 70) msg[pos++] = *reason++;
    }
    vga_print_centered(msg[0] ? msg : "Unknown fatal error", 10, BLUE_BG_WHITE);

    if (function && *function) {
        vga_print_centered("in function:", 14, BLUE_BG_YELLOW);
        vga_print_centered(function, 16, BLUE_BG_YELLOW);
    }

    int seconds = 10;
        while (seconds >= 0) {
            char buf[32] = "Reboot in ";
            int i = 10;
            int t = seconds;
            if (t == 0) {
                buf[i++] = '0';
            } else {
                char tmp[4];
                int j = 0;
                while (t > 0) {
                    tmp[j++] = '0' + (t % 10);
                    t /= 10;
                }
                while (j > 0) buf[i++] = tmp[--j];
            }
            buf[i++] = 's';
            buf[i] = 0;
            vga_print_centered(buf, 22, BLUE_BG_WHITE);

            // Заменяем тупой цикл на честную задержку в 1 секунду (1000 мс)
            panic_sleep_ms(1000);

            seconds--;
        }

    asm volatile(
        "mov $0xFE, %%al;"
        "out %%al, $0x64;"
        ::: "eax"
    );

    asm volatile(
        "mov $0x02, %%al;"
        "out %%al, $0x92;"
        ::: "eax"
    );

    while (1) asm("hlt");
}
