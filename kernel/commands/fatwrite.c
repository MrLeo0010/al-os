#include "all_commands.h"
#include "../fs/fat.h"
#include "../drivers/vga.h"
#include "../utils/string.h"
#include "../utils/colors.h"

void cmd_fatwrite(char* args) {
    char* text = strchr(args, ' ');
    if (text) {
        *text = '\0';
        text++;
        fat_write(args, text, strlen(text));
    } else {
        vga_print_color("Usage: fatwrite <file> <text>\n", LIGHT_RED);
    }
}
