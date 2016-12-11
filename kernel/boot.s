MBALIGN equ 1<<0
MEMINFO equ 1<<1
FLAGS equ MBALIGN | MEMINFO
MAGIC equ 0x1BADB002
CHECKSUM equ -(MAGIC + FLAGS)

section .multiboot
align 4
      dd MAGIC
      dd FLAGS
      dd CHECKSUM

section .bss
stack_bottom:
resb 8192 ; 8kb
stack_top:

section .text
global _start:function (_start.end - _start)
_start:
      mov esp, stack_top

      extern kmain
      call kmain

      cli
.hang:hlt
      jmp .hang
.end:
