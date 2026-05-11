
#include "history.h"

char history[HISTORY_SIZE][HISTORY_LEN] = {0};
int history_count = 0;
int history_nav = -1;
void print(const char *str);
