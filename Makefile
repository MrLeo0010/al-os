CC = gcc
ASM = nasm
LD = ld

CFLAGS = -ffreestanding -O2 -Wall -Wextra -m32 -nostdlib -Ikernel
ASFLAGS = -f elf32

TARGET = kernel.elf
ISO = AL-OS.iso

comma := ,
DRIVE_ARG := $(if $(wildcard fat32_min_4k.img),-drive file=fat32_min_4k.img$(comma)format=raw)

# Находим ВСЕ файлы на Си
C_SRCS := $(shell find kernel -name '*.c')
C_OBJS := $(C_SRCS:.c=.o)

# Находим ВСЕ файлы на Ассемблере (и в boot, и в kernel)
ASM_SRCS := $(shell find boot kernel -name '*.asm')
ASM_OBJS := $(ASM_SRCS:.asm=.o)

# Все объектники
OBJS := $(ASM_OBJS) $(C_OBJS)

all: $(TARGET)

boot/kernel_entry.o: boot/kernel_entry.asm
	$(ASM) $(ASFLAGS) $< -o $@

# Универсальное правило
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Правило для сборки ЛЮБОГО .asm файла в .o
%.o: %.asm
	$(ASM) $(ASFLAGS) $< -o $@

$(TARGET): $(OBJS)
	$(LD) -T linker.ld -m elf_i386 -o $(TARGET) $(OBJS)

iso: $(TARGET)
	mkdir -p iso/boot/grub
	cp $(TARGET) iso/boot/kernel.elf
	printf 'set timeout=1\nset default=0\nmenuentry "Boot Al-OS" {\n    multiboot /boot/kernel.elf\n    boot\n}\n' > iso/boot/grub/grub.cfg
	grub-mkrescue -o $(ISO) iso

clean-all:
	rm -f $(OBJS) $(TARGET) $(ISO)
	rm -rf iso

clean:
	rm -f $(OBJS) $(TARGET)
	rm -rf iso

run:
	qemu-system-i386 \
		-m 64M \
		-cdrom AL-OS.iso \
		$(DRIVE_ARG) \
		-boot d \
		-display gtk

.PHONY: all iso clean clean-all run
