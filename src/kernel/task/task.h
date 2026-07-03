#ifndef TASK_H
#define TASK_H

#include <stdint.h>

#define MAX_TASKS 4
#define STACK_SIZE 4096

// Контекст процессора на стеке задачи
struct __attribute__((packed)) cpu_context {
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t eip;
};

// Thread Control Block
struct task {
    int id;
    uint32_t esp;
    uint8_t stack[STACK_SIZE];
    int is_running;
};

void init_multitasking();
int create_task(void (*entry_point)());
void schedule();

// Ассемблерный переключатель
// extern void switch_context(uint32_t* old_esp, uint32_t new_esp);
__attribute__((cdecl)) extern void switch_context(uint32_t* old_esp, uint32_t new_esp);

#endif
