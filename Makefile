# Makefile
#HOME=$HOME
ROOT_DIR :=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
HOME=$(ROOT_DIR)/toolchain
PREFIX=$(HOME)/opt/cross
TARGET ?=i686
ARCH = $(TARGET)
TOOLS=$(PREFIX)/bin/$(TARGET)-elf

cc=$(TOOLS)-gcc
c++=$(TOOLS)-g++
ld=$(TOOLS)-ld
asm=nasm
grub-mkrescue=/Users/jh/opt/cross/bin/grub-mkrescue #fixme

cflags=-nostdlib -std=gnu99 -O2 -ffreestanding
cflags +=-Wall -Wextra -Wno-unused-function -Wno-unused-parameter -Wno-format
asmflags=-w+orphan-labels

isodir=./iso
iso=image.iso

qemuflags ?= -drive file=$(iso),format=raw -s -no-reboot

.DEFAULT: all
.PHONY: all
all: image

.PHONY: clean
clean:
	rm -rf $(iso)
	rm -rf */*.o *.o
	rm -rf */*.bin *.bin
	rm -rf kernel/arch/*/*.o
	rm -rf iso/*

.PHONY: qemu
qemu: $(iso)
	qemu-system-x86_64 $(qemuflags)

iso: $(iso)

$(iso): $(isodir)/boot/grub/grub.cfg kernel/kernel.bin LICENCE
	mkdir -p $(isodir)/boot/grub
	mkdir -p $(isodir)/boot/brainOS
	cp kernel/kernel.bin $(isodir)/boot/brainOS/kernel.bin
	cp LICENCE $(isodir)/boot/brainOS/LICENCE
	$(grub-mkrescue) -o $(iso) $(isodir)

$(isodir)/boot/grub/grub.cfg: grub.cfg
	mkdir -p $(isodir)/boot/grub
	cp grub.cfg $(isodir)/boot/grub/grub.cfg

kernel/kernel.bin: kernel/arch/$(TARGET)/linker.ld kernel/arch/$(TARGET)/boot.o kernel/kmain.c kernel/*.c | kernel/*.h kernel/arch/$(TARGET)/*
	$(cc) $(cflags) -o $@ -T $^

kernel/%.o: kernel/%.c
	$(cc) $(cflags) -c $< -o $@

kernel/%.o: kernel/%.s
	$(asm) $(asmflags) -felf $< -o $@

kernel/arch/$(TARGET)/boot.o: kernel/arch/$(TARGET)/boot.s kernel/arch/$(TARGET)/*.s |  kernel/arch/$(TARGET)/linker.ld
	$(asm) $(asmflags) -felf $< -o $@

toolchain: toolchain/build.sh
	@cd toolchain ; ./build.sh
