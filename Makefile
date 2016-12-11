# Makefile
#HOME=$HOME
PREFIX=$(HOME)/opt/cross
TARGET=i686-elf
TOOLS=$(PREFIX)/bin/$(TARGET)

cc=$(TOOLS)-gcc
c++=$(TOOLS)-g++
ld=$(TOOLS)-ld
cflags=-nostdlib -std=gnu99 -O2 -ffreestanding
cpreflags=-Wall -Wextra
asm=nasm
asmflags=-w+orphan-labels
grub-mkrescue=$(PREFIX)/bin/grub-mkrescue

isodir=./iso
iso=image.iso

.DEFAULT: all
.PHONY: all
all: image

.PHONY: clean
clean:
	rm -rf $(iso)
	rm -rf *.o
	rm -rf *.bin

.PHONY: qemu
qemu: $(iso)
	qemu-system-x86_64 -drive file=$(iso),format=raw -m 256M -s

iso: $(iso)

$(iso): kernel/kernel.bin
	mkdir -p $(isodir)/boot/grub
	cp grub.cfg $(isodir)/boot/grub/grub.cfg
	cp kernel/kernel.bin $(isodir)/boot/grub/kernel.bin
	$(grub-mkrescue) -o $(iso) $(isodir)

kernel/kernel.bin: kernel/boot.o kernel/linker.ld kernel/kmain.o
	$(cc) $(cflags) -T kernel/linker.ld -ffreestanding -O2 -o kernel/kernel.bin -lgcc kernel/boot.o kernel/kmain.o

kernel/*.o: kernel/*.c
	$(cc) $(cflags) $(cpreflags) -o $@ -c $<

kernel/boot.o: kernel/boot.s
	$(asm) $(asmflags) -felf -o $@ $<
