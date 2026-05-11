#ifndef KERNEL_H
#define KERNEL_H

#define HISTORY_SIZE 10
#define HISTORY_LEN 128
#define MAX_CMD_LEN 128

// === Глобальные переменные ===
extern int history_count;
extern int history_nav;
extern char history[HISTORY_SIZE][HISTORY_LEN];
extern char user[32];
extern long boot_seconds;

// === Функции ===
void keyboard_history_add(const char* cmd);

#endif
