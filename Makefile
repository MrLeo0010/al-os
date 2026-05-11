CC = gcc
ASM = nasm
LD = ld

CFLAGS = -ffreestanding -O2 -Wall -Wextra -m32 -nostdlib -Ikernel
ASFLAGS = -f elf32

TARGET = kernel.elf
ISO = AL-OS.iso

comma := ,
DRIVE_ARG := $(if $(wildcard fat32_min_4k.img),-drive file=fat32_min_4k.img$(comma)format=raw)

OBJS = boot/kernel_entry.o \
       kernel/kernel.o \
       kernel/commands/execute_commands.o \
       kernel/commands/calc.o \
       kernel/commands/colorbar.o \
       kernel/commands/cp.o \
       kernel/commands/date.o \
       kernel/commands/echo.o \
       kernel/commands/help.o \
       kernel/commands/history.o \
       kernel/commands/memtest.o \
       kernel/commands/mv.o \
       kernel/commands/slowfetch.o \
       kernel/commands/sysinfo.o \
       kernel/commands/tree.o \
       kernel/commands/whoami.o \
       kernel/drivers/ata.o \
       kernel/drivers/keyboard.o \
       kernel/drivers/vga.o \
       kernel/exec/elf.o \
       kernel/fs/fat.o \
       kernel/fs/fs.o \
       kernel/utils/beep.o \
       kernel/utils/fat_shell.o \
       kernel/utils/fm.o \
       kernel/utils/fm_fat.o \
       kernel/utils/memtest.o \
       kernel/utils/nano.o \
       kernel/utils/panic.o \
       kernel/utils/power/reboot.o \
       kernel/utils/power/poweroff.o \
       kernel/utils/screensaver.o \
       kernel/utils/string.o \
       kernel/utils/time.o \
       kernel/commands/aarch.o \
       kernel/utils/init.o

all: $(TARGET)

boot/kernel_entry.o: boot/kernel_entry.asm
	$(ASM) $(ASFLAGS) $< -o $@

kernel/%.o: kernel/%.c
	$(CC) $(CFLAGS) -c $< -o $@

kernel/drivers/%.o: kernel/drivers/%.c
	$(CC) $(CFLAGS) -c $< -o $@

kernel/utils/%.o: kernel/utils/%.c
	$(CC) $(CFLAGS) -c $< -o $@

kernel/fs/%.o: kernel/fs/%.c
	$(CC) $(CFLAGS) -c $< -o $@

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
