#include "../drivers/vga.h"
#include "../fs/fs.h"
#include "../kernel.h"
#include "../utils/string.h"
#include "../utils/colors.h"


void cmd_cp(const char* args) {
    if (!args) { vga_print_color("Usage: cp <src> <dest>\n", LIGHT_RED); return; }
    char* dest = strchr(args, ' ');
    if (!dest) { vga_print_color("Usage: cp <src> <dest>\n", LIGHT_RED); return; }
    *dest = '\0'; dest++;
    while (*dest == ' ') dest++;

    fs_node* src = resolve_path(args, fs_current);
    if (!src || src->type != FS_FILE) { vga_print_color("Source not file\n", LIGHT_RED); return; }
    fs_touch(dest);
    fs_write(dest, src->content);
}
