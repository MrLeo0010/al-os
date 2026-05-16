#include "isr.h"
#include "../pic/pic.h"
#include "../../../utils/panic.h"

// Текстовые описания первых 32 исключений x86
const char *exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",
    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point",
    "Virtualization Exception",
    "Control Protection Exception",
    "Reserved", "Reserved", "Reserved", "Reserved",
    "Reserved", "Reserved", "Reserved", "Reserved",
    "Reserved", "Reserved"
};

// Сюда прыгает ассемблерный stub
void isr_handler(registers_t regs) {
    if (regs.int_no < 32) {
        panic("ISR", exception_messages[regs.int_no], "isr_handler");
    }
}

void irq_handler(registers_t regs) {
    uint8_t irq_no = regs.int_no - 32;

    if (irq_no == 0) {
        // Таймер тикает! Сюда можно будет выводить лог или инкрементировать тики
        // Пока оставим пустым, чтобы не спамить в консоль
    }
    else if (irq_no == 1) {
        // Клавиатура послала сигнал!
        // uint8_t scancode = inb(0x60); — так считывается скан-код
    }

    // Сообщаем контроллеру PIC, что прерывание обработано успешно,
    // иначе он обидится и больше ничего не пришлет
    pic_send_eoi(irq_no);
}
