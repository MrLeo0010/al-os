#include <stddef.h>
#include "drivers/vga.h"
#include "drivers/keyboard.h"
#include "fs/fs.h"
#include "commands/execute_commands.h"
#include "kernel.h"
#include "utils/time.h"
#include "utils/init.h"
#include "utils/string.h"


char user[32] = "root";
long boot_seconds = 0;

char history[HISTORY_SIZE][HISTORY_LEN] = {0};
int history_count = 0;
int history_nav = -1;
void print(const char *str);

void keyboard_history_add(const char* cmd);

static void show_prompt(void) {
    vga_print_color(user, 0x0A);
    vga_print_color("@al-os", 0x0B);
    vga_print_color(current_path, 0x0F);
    vga_print_color("> ", 0x07);
}

void kernel_main(void)
{
    init();

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
