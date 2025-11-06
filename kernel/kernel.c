#include <stddef.h>
#include "vga.h"
#include "keyboard.h"
#include "fs.h"

#define MAX_CMD_LEN 128

int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) { s1++; s2++; }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

char* strchr(const char* s, int c) {
    while (*s) { if (*s == (char)c) return (char*)s; s++; }
    return NULL;
}

void kernel_main() {
    vga_clear();
    vga_print_color("Kernel initialized.\n", 0x0A);
    for (volatile int i = 0; i < 20000000; i++);
    vga_print_color("AlMUS Shell v0.4\nType 'help' to list commands.\n", 0x0A);

    char cmd[MAX_CMD_LEN];

    while (1) {
        vga_print_color("AlMUS@system> ", 0x0A);
        keyboard_read_line(cmd, MAX_CMD_LEN);

        char* space = strchr(cmd, ' ');
        char* args = "";
        if (space) { *space = 0; args = space + 1; }

        if (strcmp(cmd, "help") == 0) {
            vga_print_color("=== Command list ===\n", 0x0A);
            vga_print_color("help - show this list\n", 0x0A);
            vga_print_color("about - info about system\n", 0x0A);
            vga_print_color("echo - print text\n", 0x0A);
            vga_print_color("clear - clear screen\n", 0x0A);
            vga_print_color("ls - list files\n", 0x0A);
            vga_print_color("create - create file\n", 0x0A);
            vga_print_color("write - write file\n", 0x0A);
            vga_print_color("cat - show file\n", 0x0A);
            vga_print_color("time - show current time\n", 0x0A);
        } else if (strcmp(cmd, "about") == 0) {
            vga_print_color("AlMUS Shell v0.4 | Mini OS\n", 0x0A);
        } else if (strcmp(cmd, "echo") == 0) {
            vga_print_color(args, 0x09); vga_putc('\n');
        } else if (strcmp(cmd, "clear") == 0) {
            vga_clear();
        } else if (strcmp(cmd, "ls") == 0) {
            fs_list();
        } else if (strcmp(cmd, "create") == 0) {
            if (!args[0]) vga_print_color("Usage: create <name>\n", 0x0C); else fs_create(args);
        } else if (strcmp(cmd, "write") == 0) {
            char* s2 = strchr(args, ' ');
            if (!s2) vga_print_color("Usage: write <name> <text>\n", 0x0C);
            else { *s2 = 0; fs_write(args, s2 + 1); }
        } else if (strcmp(cmd, "cat") == 0) {
            if (!args[0]) vga_print_color("Usage: cat <name>\n", 0x0C); else fs_cat(args);
        } else if (strcmp(cmd, "time") == 0) {
            vga_print_color("Time not implemented.\n", 0x0C);
        } else if (cmd[0] == 0) {
            /* empty */
        } else {
            vga_print_color("Error: command not found\n", 0x0C);
        }
    }
}
