# Makefile
ROOT_DIR :=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
PREFIX=$(ROOT_DIR)/toolchain/opt/cross
TARGET ?=i686
ARCH = $(TARGET)
TOOLS=$(PREFIX)/bin/$(TARGET)-elf

cc=$(TOOLS)-gcc
c++=$(TOOLS)-g++
objcopy=$(TOOLS)-objcopy
ld=$(TOOLS)-ld
nasm=nasm
grub-mkrescue=/Users/jh/opt/cross/bin/grub-mkrescue #fixme

cflags  =-std=gnu99 -O2 -ffreestanding
cflags +=-Wall -Wextra -Wno-unused-function -Wno-unused-parameter -Wno-format
cflags +=-g
nasmflags=-w+orphan-labels

isodir=./iso
iso=image.iso

qemuflags ?= -drive file=$(iso),format=raw -s -net none

.DEFAULT: all
.PHONY: all
all: image

.PHONY: clean
clean:
	rm -rf $(iso)
	rm -rf */*.o *.o
	rm -rf */*.bin *.bin
	rm -rf kernel/kernel.elf kernel/kernel.sym
	rm -rf kernel/arch/*/*.o
	rm -rf iso/*

.PHONY: qemu
qemu: $(iso) kernel/kernel.sym
	qemu-system-x86_64 $(qemuflags)

.PHONY: debug
debug: kernel/kernel.sym
	gdb -ex "target remote localhost:1234" -ex "symbol-file kernel/kernel.elf"

.PHONY: toolchain
toolchain: toolchain/build.sh
	@cd toolchain ; ./build.sh

iso: $(iso)

$(iso): $(isodir)/boot/grub/grub.cfg $(isodir)/boot/brainOS/kernel.elf LICENCE
	mkdir -p $(isodir)/boot/grub
	mkdir -p $(isodir)/boot/brainOS
	cp LICENCE $(isodir)/boot/brainOS/LICENCE
	$(grub-mkrescue) -o $(iso) $(isodir)

$(isodir)/boot/brainOS/kernel.elf: kernel/kernel.elf
	mkdir -p $(@D)
	$(objcopy) --strip-debug $< $@

$(isodir)/boot/grub/grub.cfg: grub.cfg
	mkdir -p $(isodir)/boot/grub
	cp grub.cfg $(isodir)/boot/grub/grub.cfg

kernel/kernel.sym: kernel/kernel.elf
	$(objcopy) --only-keep-debug $< $@

kernel/kernel.elf: kernel/arch/$(TARGET)/linker.ld kernel/arch/$(TARGET)/boot.o kernel/kmain.c kernel/*.c | kernel/*.h kernel/arch/$(TARGET)/*
	$(cc) $(cflags) -nostdlib -o $@ -T $^

kernel/%.o: kernel/%.c
	$(cc) $(cflags) -nostdlib -c $< -o $@

kernel/arch/$(TARGET)/boot.o: kernel/arch/$(TARGET)/boot.s kernel/arch/$(TARGET)/*.s
	$(nasm) $(nasmflags) -felf $< -o $@

kernel/arch/$(ARCH)/%.o: kernel/arch/$(ARCH)/%.s | kernel/arch/$(ARCH)/*.s # This is deliberatly a * because we don't really have a nice way to detect %include in assembly
	$(nasm) $(nasmflags) -felf $< -o $@
