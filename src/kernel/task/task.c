#include "task.h"

struct task task_table[MAX_TASKS];
int current_task_id = 0;
struct task kernel_task;

void init_multitasking() {
    for (int i = 0; i < MAX_TASKS; i++) {
        task_table[i].is_running = 0;
    }

    // Слот 0 — это наше текущее состояние (ядро/шелл)
    kernel_task.id = 0;
    kernel_task.is_running = 1;
    task_table[0] = kernel_task;
    current_task_id = 0;
}

int create_task(void (*entry_point)()) {
    for (int i = 1; i < MAX_TASKS; i++) {
        if (!task_table[i].is_running) {
            struct task* t = &task_table[i];
            t->id = i;
            t->is_running = 1;

            // Стек растет вниз. Берем самый конец массива.
            uint32_t* stack_top = (uint32_t*)(&t->stack[STACK_SIZE]);
            stack_top -= 8; // Сдвиг ровно на 8 элементов uint32_t

            struct cpu_context* ctx = (struct cpu_context*)stack_top;

            ctx->eax = 0; ctx->ecx = 0; ctx->edx = 0; ctx->ebx = 0;
            ctx->ebp = 0; ctx->esi = 0; ctx->edi = 0;
            ctx->eip = (uint32_t)entry_point;

            t->esp = (uint32_t)stack_top;
            return i;
        }
    }
    return -1;
}

void schedule() {
    int old_id = current_task_id;
    int next_id = (current_task_id + 1) % MAX_TASKS;

    while (!task_table[next_id].is_running) {
        next_id = (next_id + 1) % MAX_TASKS;
        if (next_id == current_task_id) return;
    }

    current_task_id = next_id;
    switch_context(&(task_table[old_id].esp), task_table[next_id].esp);
}
