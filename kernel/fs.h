#ifndef FS_H
#define FS_H

void fs_list();
void fs_create(const char* name);
void fs_write(const char* name, const char* text);
void fs_cat(const char* name);

#endif
