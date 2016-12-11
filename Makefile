# Makefile
#HOME=$HOME
PREFIX=$(HOME)/opt/cross
TARGET=i686-elf

TOOLS=$(PREFIX)/bin/$(TARGET)
CC=$(TOOLS)-gcc
C++=$(TOOLS)-g++
CFLAGS=-nostdlib
NASM=nasm
NASMFLAGS=-w+orphan-labels
GRUB-MKRESCUE=$(PREFIX)/bin/grub-mkrescue

ISODIR=./iso
ISOFILE=image.iso




# QEMU specific

MEMORY=256M

.DEFAULT: all
.PHONY: all
all: image

.PHONY: clean
clean:
	@echo "Cleaning..."

.PHONY: qemu
qemu: iso
	qemu-system-x86_64 -drive file=$(ISOFILE),format=raw -m $(MEMORY) -s

iso: kernel/kernel.bin
	mkdir -p $(ISODIR)/boot/grub
	cp grub.cfg $(ISODIR)/boot/grub/grub.cfg
	cp kernel/kernel.bin $(ISODIR)/boot/grub/kernel.bin
	$(grub-mkrescue) -o $(ISOFILE) $(ISODIR)

kernel/kernel.bin: kernel/boot.o kernel/linker.ld kernel/kmain.o
	$(CC) $(CFLAGS) -T kernel/linker.ld -ffreestanding -O2 -o kernel/kernel.bin kernel/boot.o kernel/kmain.o -lgcc

kernel/kmain.o: kernel/kmain.c kernel/kmain.h
	$(CC) $(CFLAGS) -c kernel/kmain.c -o kernel/kmain.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

kernel/boot.o:	kernel/boot.s
	$(NASM) $(NASMFLAGS) -felf kernel/boot.s -o kernel/boot.o
