#include <stddef.h>
#include "drivers/vga.h"
#include "drivers/keyboard.h"
#include "fs/fs.h"
#include "utils/ports.h"
// #include "utils/string.h"
#include "commands/execute_commands.h"
// #include "commands/help.h"
#include "kernel.h"


char user[32] = "root";
long boot_seconds = 0;

char history[HISTORY_SIZE][HISTORY_LEN] = {0};
int history_count = 0;
int history_nav = -1;
void print(const char *str);

char* strstr(const char* haystack, const char* needle) {
    if (!*needle) return (char*)haystack;
    while (*haystack) {
        const char* h = haystack;
        const char* n = needle;
        while (*h && *n && *h == *n) { h++; n++; }
        if (!*n) return (char*)haystack;
        haystack++;
    }
    return NULL;
}

char* strrchr(const char* s, int c) {
    const char* last = NULL;
    while (*s) {
        if (*s == (char)c) last = s;
        s++;
    }
    if (*s == (char)c) last = s;
    return (char*)last;
}

size_t strlen(const char *str) {
    const char *s = str;
    while (*s) s++;
    return (size_t)(s - str);
}

void keyboard_history_add(const char* cmd);

static void show_prompt(void) {
    vga_print_color(user, 0x0A);
    vga_print_color("@al-os", 0x0B);
    vga_print_color(current_path, 0x0F);
    vga_print_color("> ", 0x07);
}

static int bcd2bin(int v) { return (v & 0x0F) + ((v >> 4) * 10); }

void rtc_read(rtc_time* t) {
    unsigned char status;
    do { outb(0x70, 0x0A); status = inb(0x71); } while (status & 0x80);
    outb(0x70, 0x00); unsigned char sec = inb(0x71);
    outb(0x70, 0x02); unsigned char min = inb(0x71);
    outb(0x70, 0x04); unsigned char hour = inb(0x71);
    outb(0x70, 0x07); unsigned char day = inb(0x71);
    outb(0x70, 0x08); unsigned char month = inb(0x71);
    outb(0x70, 0x09); unsigned char year = inb(0x71);
    outb(0x70, 0x0B); unsigned char statusB = inb(0x71);

    if (!(statusB & 0x04)) {
        t->sec = bcd2bin(sec);
        t->min = bcd2bin(min);
        t->hour = bcd2bin(hour);
        t->day = bcd2bin(day);
        t->month = bcd2bin(month);
        t->year = bcd2bin(year) + 2000;
    } else {
        t->sec = sec; t->min = min; t->hour = hour; t->day = day; t->month = month; t->year = 2000 + year;
    }
}

static long days_from_civil(int y, unsigned m, unsigned d) {
    y -= m <= 2;
    long era = (y >= 0 ? y : y-399) / 400;
    unsigned yoe = (unsigned)(y - era * 400);
    unsigned doy = (153*(m + (m > 2 ? -3 : 9)) + 2)/5 + d - 1;
    unsigned doe = yoe * 365 + yoe/4 - yoe/100 + doy;
    return era * 146097 + (long)doe - 719468;
}

long time_to_seconds(const rtc_time* t) {
    long days = days_from_civil(t->year, t->month, t->day);
    return days * 86400 + t->hour * 3600 + t->min * 60 + t->sec;
}

void wait_cycles(uint32_t cycles) {
    for (volatile uint32_t i = 0; i < cycles; i++);
}

void rtrim_spaces(char* s) {
    int len = strlen(s);
    while (len > 0 && (s[len - 1] == ' ' || s[len - 1] == '\t')) {
        s[--len] = '\0';
    }
}

void kernel_main(void)
{
    vga_clear();
    vga_print_color("Welcome to AL-OS!\n", 0x0A);
    vga_print_color("Type 'help' to see available commands\n\n", 0x0F);

    fs_init();
    fs_cd("/home");

    rtc_time boot_time;
    rtc_read(&boot_time);
    boot_seconds = time_to_seconds(&boot_time);

    // int cmd_count = (int)(sizeof(help_table) / sizeof(help_table[0]));
    // for (int i = 0; i < cmd_count; i++)
    // {
    //     char path[64];
    //     strcpy(path, "/bin/");
    //     size_t len = strlen(path);
    //     size_t remain = sizeof(path) - len - 1;
    //     size_t to_copy = strlen(help_table[i].cmd);
    //     if (to_copy > remain) to_copy = remain;

    //     int j;
    //     for (j = 0; j < (int)to_copy; j++)
    //         path[len + j] = help_table[i].cmd[j];
    //     path[len + j] = '\0';
    // }

    char cmd[MAX_CMD_LEN];

    while (1)
    {
        show_prompt();
        keyboard_read_line(cmd, MAX_CMD_LEN);

        if (cmd[0] == '\0' || cmd[0] == '\n')
            continue;

        keyboard_history_add(cmd);

        char* current_cmd = cmd;
        while (current_cmd != NULL && *current_cmd != '\0') {

            char* next_separator = strstr(current_cmd, "&&");

            if (next_separator) {
                *next_separator = '\0';
                execute_command(current_cmd);
                current_cmd = next_separator + 2;
            } else {
                execute_command(current_cmd);
                current_cmd = NULL;
            }
        }
    }
}
