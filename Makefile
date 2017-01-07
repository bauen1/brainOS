# Makefile
root_dir :=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
prefix ?=$(root_dir)/toolchain/opt/cross
arch ?=i686
target = $(arch)-elf
tools=$(prefix)/bin/$(target)

cc=$(tools)-gcc
c++=$(tools)-g++
objcopy=$(tools)-objcopy
ld=$(tools)-ld
nasm=nasm
grub-mkrescue ?=grub-mkrescue
grub-file ?=grub-file

cflags  = -std=gnu99 -O2 -ffreestanding
cflags += -Wall -Wextra -Wno-unused-function -Wno-unused-parameter -Wno-format
cflags += -g
cflags += -fstack-protector

kernel_cflags = -D__BRAINOS_KERNEL__
kernel_cflags += -D__ARCH__=$(arch)
kernel_cflags += -D__TARGET__=$(target)
modules_cflags = -D__BRAINOS_MODULE__
modules_cflags += -D__ARCH__=$(arch)
modules_cflags += -D__TARGET__=$(target)

nasmflags=-w+orphan-labels

isodir=./iso
iso=image.iso

qemuflags = -drive file=$(iso),format=raw -s -device rtl8139,netdev=vlan0 -netdev user,restrict=y,id=vlan0 -net none -net dump,file=/tmp/vm0.pcap -m 32M -d guest_errors -name "brainOS development vm" -trace net*

.DEFAULT: all
.PHONY: all
all: iso

.PHONY: clean
clean:
	rm -rf $(isodir)
	rm -rf $(iso)
	rm -rf */*.o *.o
	rm -rf */*.bin *.bin
	rm -rf kernel/kernel.elf kernel/kernel.sym
	rm -rf kernel/arch/*/*.o
	rm -rf modules/*.elf modules/*.sym

.PHONY: tests multiboot-test
tests: multiboot-test

# TODO: make this a bit cleaner and add more tests
multiboot-test:	$(isodir)/boot/brainOS/kernel.elf
	@$(grub-file) --is-x86-multiboot $< && echo "[test] kernel is multiboot 1"
	@#$(grub-file) --is-x86-multiboot2 $< && echo "[test] kernel is multiboot 2"

.PHONY: qemu
qemu: $(iso) kernel/kernel.sym
	qemu-system-x86_64 $(qemuflags) $(user_qemuflags)

.PHONY: debug
debug: kernel/kernel.sym
	gdb -ex "target remote localhost:1234" -ex "symbol-file kernel/kernel.elf"

.PHONY: toolchain
toolchain: $(cc)

$(cc): toolchain/build.sh
	cd toolchain ; ./build.sh

iso: $(iso)

$(iso): $(isodir)/boot/grub/grub.cfg $(isodir)/boot/brainOS/kernel.elf $(isodir)/boot/brainOS/LICENCE $(isodir)/boot/brainOS/modules/
	$(grub-mkrescue) -o $(iso) $(isodir)

$(isodir)/boot/grub/grub.cfg: grub.cfg
	mkdir -p $(@D)
	cp $< $@

$(isodir)/boot/brainOS/LICENCE: LICENCE
	mkdir -p $(@D)
	cp $< $@

$(isodir)/boot/brainOS/kernel.elf: kernel/kernel.elf
	mkdir -p $(@D)
	$(objcopy) --strip-debug $< $@

# FIXME
$(isodir)/boot/brainOS/modules/: $(isodir)/boot/brainOS/modules/hello_world.elf
	mkdir -p $(@D)

$(isodir)/boot/brainOS/modules/%.elf: modules/%.elf
	mkdir -p $(@D)
	#cp $< $@
	$(objcopy) --strip-debug $< $@

kernel/kernel.sym: kernel/kernel.elf
	$(objcopy) --only-keep-debug $< $@

kernel/kernel.elf: kernel/arch/$(arch)/linker.ld kernel/arch/$(arch)/boot.o kernel/kmain.c kernel/*.c | include/kernel/*.h
	time $(cc) $(cflags) $(kernel_cflags) -I./include/kernel -nostdlib -o $@ -T $^

# TODO: use gcc to create object files for each c file to speed up building
kernel/%.o: kernel/%.c | include/kernel/*.h
	$(cc) $(cflags) $(kernel_cflags) -I./include/kernel -nostdlib -c $< -o $@

kernel/arch/$(arch)/%.o: kernel/arch/$(arch)/%.s | kernel/arch/$(arch)/*.s # This is deliberatly a * because we don't really have a nice way to detect %include in assembly
	$(nasm) $(nasmflags) -felf $< -o $@

modules/%.elf: modules/linker.ld modules/%.c | include/kernel/*.h
	time $(cc) $(cflags) $(modules_cflags) -I./include/kernel -nostdlib -o $@ -r -T $^
