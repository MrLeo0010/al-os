#include "../utils/string.h"
#include "../kernel.h"
#include "../fs/fs.h"
#include "../drivers/vga.h"

void cmd_mv(const char* args) {
    if (!args) { vga_print_color("Usage: mv <src> <dest>\n", 0x0C); return; }
    char* dest = strchr(args, ' ');
    if (!dest) { vga_print_color("Usage: mv <src> <dest>\n", 0x0C); return; }
    *dest = '\0'; dest++;
    while (*dest == ' ') dest++;

    fs_node* src = resolve_path(args, fs_current);
    if (!src) { vga_print_color("Source not found\n", 0x0C); return; }
    const char* new_name = dest;
    char* slash = strrchr(dest, '/');
    if (slash) new_name = slash + 1;
    strcpy(src->name, new_name);
}
