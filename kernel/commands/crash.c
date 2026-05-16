#include "all_commands.h"
#include "../utils/crash.h"

void cmd_crash(void) {
    crash_system();
}
