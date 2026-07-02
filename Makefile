CC = gcc
ASM = nasm
LD = ld

CUR_DIR := $(CURDIR)

CFLAGS = -ffreestanding -O2 -Wall -Wextra -m32 -nostdlib -Ikernel
ASFLAGS = -f elf32

TARGET = kernel.elf
ISO = AL-OS.iso

comma := ,
DRIVE_ARG := $(if $(wildcard fat32.img),-drive file=fat32.img$(comma)format=raw$(comma)if=ide$(comma)index=1)

# Находим ВСЕ файлы на Си
# Находим ВСЕ файлы на Си
# ... твои переменные (CC, ASM, LD, CFLAGS, ISO и т.д.) ...

# 1. ИСПРАВЛЯЕМ ПУТИ: ищем исходники там, где они реально лежат (kernel и boot)
C_SRCS := $(shell find kernel/ -name '*.c')
C_OBJS := $(C_SRCS:.c=.o)

# Если у тебя ассемблерные файлы лежат в boot/ и kernel/, ищем в обеих папках
ASM_SRCS := $(shell find boot/ kernel/ -name '*.asm' 2>/dev/null)
ASM_OBJS := $(ASM_SRCS:.asm=.o)

# Добавляем в общий список объектников те .o файлы, которые уже скомпилированы
# и лежат в дереве (на случай, если они сироты или компилировались иначе)
ALL_O_FILES := $(shell find boot/ kernel/ -name '*.o' 2>/dev/null)
OBJS := $(ASM_OBJS) $(C_OBJS)

# ... твои правила сборки (all:, %.o:, $(TARGET):, iso:) ...

# Объявляем цели как phony, чтобы make не искал файлы с именами clean/clean-all
.PHONY: clean clean-all

# Быстрый clean: чистит вообще ВСЕ файлы .o в проекте и бинарник ядра
clean:
	rm -f $(ALL_O_FILES) $(TARGET)
	rm -rf iso

# Полный clean: чистит объектники, ядро, ISO-образ И запускает очистку проекта на Rust
clean-all: clean
	rm -f $(ISO)
	@if [ -d "rust_core" ]; then \
		echo "Cleaning Rust target..."; \
		cd rust_core && cargo clean; \
	fi

run:
	qemu-system-i386 \
		-m 64M \
		-cdrom AL-OS.iso \
		$(DRIVE_ARG) \
		-boot d \
		-display gtk

run_net:
	qemu-system-i386 \
		-m 64M \
		-cdrom AL-OS.iso \
		$(DRIVE_ARG) \
		-net nic,model=rtl8139 -net user \
		-boot d \
		-display gtk

.PHONY: all iso clean clean-all run run_net
