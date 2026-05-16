#ifndef PIC_H
#define PIC_H

#include <stdint.h>

// Порты ввода-вывода для PIC
#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1

// Команда End of Interrupt (EOI)
#define PIC_EOI      0x20

void pic_remap(int offset1, int offset2);
void pic_send_eoi(uint8_t irq);

#endif
