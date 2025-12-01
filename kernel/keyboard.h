#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

char keyboard_read_char(void);
void keyboard_init(void);
void keyboard_read_line(char* buffer, int max_len);

#endif