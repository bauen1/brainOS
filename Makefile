# Makefile
.DEFAULT: all
.PHONY: all
all: boot.bin image

.PHONY: clean
clean:
	rm boot.bin
	rm image

boot.bin: boot.s
	nasm -f bin boot.s -o boot.bin

image: boot.bin
	touch image
	dd if=boot.bin of=image
