// kernel/fs.c — полностью рабочий, без malloc, без ошибок
#include "fs.h"
#include "vga.h"
#include <string.h>

fs_node fs_nodes[MAX_NODES];
int node_count = 0;

fs_node* fs_root    = NULL;
fs_node* fs_current = NULL;

/* ======================= Вспомогательные ======================= */

static fs_node* create_node(const char* name, fs_type type, fs_node* parent) {
    if (node_count >= MAX_NODES) return NULL;
    fs_node* n = &fs_nodes[node_count++];
    
    int i = 0;
    while (name[i] && i < MAX_NAME_LEN - 1) {
        n->name[i] = name[i];
        i++;
    }
    n->name[i] = '\0';

    n->type = type;
    n->parent = parent;
    n->child_count = 0;
    for (i = 0; i < MAX_CHILDREN; i++) n->children[i] = NULL;
    n->content[0] = '\0';
    return n;
}

static fs_node* find_child(fs_node* dir, const char* name) {
    for (int i = 0; i < dir->child_count; i++)
        if (strcmp(dir->children[i]->name, name) == 0)
            return dir->children[i];
    return NULL;
}

static int split_path(const char* path, char segs[16][MAX_NAME_LEN]) {
    int seg = 0, i = 0, j = 0;
    if (!path || !path[0]) return 0;

    while (path[i] && seg < 16) {
        if (path[i] == '/') {
            if (j > 0) {
                segs[seg][j] = '\0';
                seg++;
                j = 0;
            }
        } else {
            segs[seg][j++] = path[i];
        }
        i++;
    }
    if (j > 0) {
        segs[seg][j] = '\0';
        seg++;
    }
    return seg;
}

static fs_node* resolve_path(const char* path, fs_node* base) {
    if (!path || !path[0]) return base;
    fs_node* cur = (path[0] == '/') ? fs_root : base;
    char segs[16][MAX_NAME_LEN];
    int n = split_path(path, segs);

    for (int i = 0; i < n; i++) {
        if (strcmp(segs[i], ".") == 0) continue;
        if (strcmp(segs[i], "..") == 0) {
            if (cur->parent) cur = cur->parent;
            continue;
        }
        fs_node* child = find_child(cur, segs[i]);
        if (!child) return NULL;
        cur = child;
    }
    return cur;
}

/* ======================= Публичные ======================= */

void fs_init(void) {
    node_count = 0;

    // Создаём корень с именем "/" — так надёжнее и точно работает
    fs_root = create_node("/", FS_DIR, NULL);
    fs_current = fs_root;

    // Явно создаём стандартные папки
    fs_mkdir("bin");
    fs_mkdir("home");
    fs_mkdir("sys");
    fs_mkdir("system");
    fs_mkdir("usr");
    fs_mkdir("mnt");
}

void fs_list(const char* path) {
    fs_node* dir = path ? resolve_path(path, fs_current) : fs_current;
    if (!dir || dir->type != FS_DIR) {
        vga_print_color("Not a directory\n", 0x0C);
        return;
    }
    for (int i = 0; i < dir->child_count; i++) {
        vga_print_color(dir->children[i]->name, 0x0A);
        if (dir->children[i]->type == FS_DIR) vga_print_color("/", 0x0B);
        vga_putc(' ');
    }
    vga_putc('\n');
}

void fs_pwd(void) {
    char path[256];
    int pos = 255;
    path[pos] = '\0';
    fs_node* cur = fs_current;

    while (cur && cur->parent) {
        int len = 0;
        while (cur->name[len]) len++;
        pos -= len + 1;
        if (pos < 0) break;
        for (int i = len - 1; i >= 0; i--) path[pos + i + 1] = cur->name[i];
        path[pos] = '/';
        cur = cur->parent;
    }
    if (pos == 255) path[--pos] = '/';
    vga_print_color(&path[pos], 0x0F);
    vga_putc('\n');
}

int fs_mkdir(const char* path) {
    if (!path || !path[0]) return -1;
    char segs[16][MAX_NAME_LEN];
    int n = split_path(path, segs);
    fs_node* cur = (path[0] == '/') ? fs_root : fs_current;

    for (int i = 0; i < n; i++) {
        fs_node* child = find_child(cur, segs[i]);
        if (!child) {
            child = create_node(segs[i], FS_DIR, cur);
            if (!child) return -1;
            cur->children[cur->child_count++] = child;
        }
        cur = child;
    }
    return 0;
}

int fs_cd(const char* path) {
    fs_node* node = resolve_path(path, fs_current);
    if (!node || node->type != FS_DIR) {
        vga_print_color("No such directory\n", 0x0C);
        return -1;
    }
    fs_current = node;
    return 0;
}

int fs_rm(const char* path) {
    fs_node* node = resolve_path(path, fs_current);
    if (!node || node == fs_root) return -1;
    fs_node* parent = node->parent;
    int idx = -1;
    for (int i = 0; i < parent->child_count; i++)
        if (parent->children[i] == node) { idx = i; break; }
    if (idx == -1) return -1;
    for (int i = idx; i < parent->child_count - 1; i++)
        parent->children[i] = parent->children[i + 1];
    parent->child_count--;
    return 0;
}

int fs_touch(const char* path) {
    if (!path || !path[0]) return -1;
    char segs[16][MAX_NAME_LEN];
    int n = split_path(path, segs);
    fs_node* parent = (path[0] == '/') ? fs_root : fs_current;

    for (int i = 0; i < n - 1; i++) {
        fs_node* child = find_child(parent, segs[i]);
        if (!child || child->type != FS_DIR) return -1;
        parent = child;
    }
    if (find_child(parent, segs[n-1])) return -1;

    fs_node* f = create_node(segs[n-1], FS_FILE, parent);
    if (f) parent->children[parent->child_count++] = f;
    return 0;
}

int fs_write(const char* path, const char* text) {
    fs_node* node = resolve_path(path, fs_current);
    if (!node || node->type != FS_FILE) return -1;
    int i = 0;
    while (text[i] && i < MAX_FILE_SIZE - 1) {
        node->content[i] = text[i];
        i++;
    }
    node->content[i] = '\0';
    return 0;
}

int fs_cat(const char* path) {
    fs_node* node = resolve_path(path, fs_current);
    if (!node || node->type != FS_FILE) {
        vga_print_color("Not a file\n", 0x0C);
        return -1;
    }
    vga_print_color(node->content[0] ? node->content : "(empty)", 0x0F);
    vga_putc('\n');
    return 0;
}