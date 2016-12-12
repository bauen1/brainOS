# Makefile
#HOME=$HOME
ROOT_DIR:=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
HOME=ROOT_DIR/toolchain
PREFIX=$(HOME)/opt/cross
TARGET ?=i686
TOOLS=$(PREFIX)/bin/$(TARGET)-elf

cc=$(TOOLS)-gcc
c++=$(TOOLS)-g++
ld=$(TOOLS)-ld
asm=nasm
grub-mkrescue=$(HOME)/opt/cross/bin/grub-mkrescue #fixme

cflags=-nostdlib -std=gnu99 -O2 -ffreestanding
cflags +=-Wall -Wextra -Wno-unused-function -Wno-unused-parameter -Wno-format
asmflags=-w+orphan-labels

isodir=./iso
iso=image.iso

.DEFAULT: all
.PHONY: all
all: image

.PHONY: clean
clean:
	rm -rf $(iso)
	rm -rf */*.o *.o
	rm -rf */*.bin *.bin

.PHONY: qemu
qemu: $(iso)
	qemu-system-x86_64 -drive file=$(iso),format=raw -m 256M -s

iso: $(iso)

$(iso): kernel/kernel.bin
	mkdir -p $(isodir)/boot/grub
	cp grub.cfg $(isodir)/boot/grub/grub.cfg
	cp kernel/kernel.bin $(isodir)/boot/grub/kernel.bin
	$(grub-mkrescue) -o $(iso) $(isodir)

kernel/kernel.bin: kernel/arch/$(TARGET)/linker.ld kernel/boot.o kernel/kmain.c kernel/*.c | kernel/*.h
	$(cc) $(cflags) -o $@ -T $^

kernel/boot.o: kernel/arch/$(TARGET)/boot.s
	$(asm) $(asmflags) -felf -o $@ $<

toolchain: toolchain/build.sh
	@cd toolchain ; ./build.sh
