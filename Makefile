# Makefile
.DEFAULT: all
.PHONY: all
all: boot.bin image

.PHONY: clean
clean:
	rm boot.bin
	rm image

.PHONY: test
test: all
	qemu-system-x86_64 -drive file=image,format=raw -m 2M

debug: all
	qemu-system-x86_64 -drive file=image,format=raw -m 2M -s

boot.bin: boot.s
	nasm -f bin boot.s -o boot.bin

image: boot.bin data
	touch image
	dd if=/dev/zero of=image bs=512 count=4
	dd if=boot.bin of=image bs=512
	dd if=data of=image bs=512 oseek=1 count=3
