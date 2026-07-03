#include "../task/task.h"
#include "../drivers/vga/vga.h" // Подключи свой заголовок vga для вывода
#include "all_commands.h"

// Важно: так как мы пока тестируем кооперативную многозадачность,
// задачи должны сами вызывать schedule(), иначе система зависнет в одной задаче.
__attribute__((force_align_arg_pointer)) static void task_alpha() {
    while(1) {
        vga_print("A");
        schedule();
    }
}

__attribute__((force_align_arg_pointer)) static void task_beta() {
    while(1) {
        // vga_putc('B', 1, 0); // Замени на свою функцию вывода символа
        vga_print("B");
        schedule();
    }
}

// Точка входа для твоей команды шелла
void cmd_test() {
    // Инициализируем планировщик
    init_multitasking();

    // Создаем две тестовые задачи
    create_task(task_alpha);
    create_task(task_beta);

    // Запускаем цикл планировщика прямо внутри команды!
    // Шелл временно отдаст управление сюда, и начнется переключение.
    while(1) {
        schedule();
    }
}
