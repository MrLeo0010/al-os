#include <stddef.h>
#include "kernel.h"
#include "utils/time.h"
#include "utils/init.h"
#include "utils/shell.h"
#include "utils/string.h"
#include "utils/shell.h"


char user[32] = "root";
long boot_seconds = 0;

char history[HISTORY_SIZE][HISTORY_LEN] = {0};
int history_count = 0;
int history_nav = -1;
void print(const char *str);

void keyboard_history_add(const char* cmd);

void kernel_main(void)
{
    init();

    shell_main_loop();

}
