#include "fs.h"
#include "vga.h"

#define MAX_FILES 64

struct File {
    char name[32];
    char content[256];
};

static struct File files[MAX_FILES];
static int file_count = 0;

void fs_list() {
    if (file_count == 0) { vga_print_color("(no files)\n", 0x08); return; }
    for (int i = 0; i < file_count; i++) {
        vga_print_color(files[i].name, 0x0B);
        vga_putc('\n');
    }
}

static int str_equal(const char* a, const char* b) {
    while (*a && *b && *a == *b) { a++; b++; }
    return (*a == 0 && *b == 0);
}

struct File* fs_find(const char* name) {
    for (int i = 0; i < file_count; i++) if (str_equal(files[i].name, name)) return &files[i];
    return 0;
}

void fs_create(const char* name) {
    if (file_count >= MAX_FILES) { vga_print_color("Error: no space left\n", 0x0C); return; }
    if (fs_find(name)) { vga_print_color("Error: file exists\n", 0x0C); return; }
    struct File* f = &files[file_count++];
    int i = 0;
    while (name[i] && i < 31) { f->name[i] = name[i]; i++; }
    f->name[i] = 0;
    f->content[0] = 0;
    vga_print_color("File created.\n", 0x0A);
}

void fs_write(const char* name, const char* text) {
    struct File* f = fs_find(name);
    if (!f) { vga_print_color("Error: file not found\n", 0x0C); return; }
    int i = 0;
    while (text[i] && i < 255) { f->content[i] = text[i]; i++; }
    f->content[i] = 0;
    vga_print_color("File written.\n", 0x0A);
}

void fs_cat(const char* name) {
    struct File* f = fs_find(name);
    if (!f) { vga_print_color("Error: file not found\n", 0x0C); return; }
    if (f->content[0] == 0) vga_print_color("(empty file)\n", 0x08);
    else { vga_print_color(f->content, 0x07); vga_putc('\n'); }
}
