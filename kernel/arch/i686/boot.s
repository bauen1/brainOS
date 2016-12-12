MB_HEADER_MAGIC equ 0x1BADB002
MB_EAX_MAGIC equ 0x2BADB002
MB_PAGE_ALIGN	equ 1<<0 ; align on 4kb boundary
MB_MEMORY_INFO	equ 1<<1 ; request mem_info struct
MB_AOUT_KLUDGE	equ 1<<16 ; specifiy location to load to
MB_HEADER_FLAGS equ MB_PAGE_ALIGN | MB_MEMORY_INFO | MB_AOUT_KLUDGE
MB_CHECKSUM equ -(MB_HEADER_MAGIC + MB_HEADER_FLAGS)

section .text
global _start
extern kmain
_start:
                  jmp start

align 4
mboot:
      dd MB_HEADER_MAGIC
      dd MB_HEADER_FLAGS
      dd MB_CHECKSUM

      extern code, bss, end
      dd mboot
      dd code
      dd bss
      dd end
      dd _start

section .text
start:
                  mov esp, stack_top
                  push esp                        ; save it for the kernel because lazy
                  push ebx                        ; multiboot header

                  cmp eax, MB_EAX_MAGIC           ; magic is in the air
                  je .call_kmain
                  mov dword [0xb8000], 0x4f4e4f4f ; NO (trying to do the least possible but still letting the user know = i'm lazy)
                  jmp halt

.call_kmain:      cli
                  call kmain

halt:             cli
.hang:            hlt
                  jmp .hang

section .bss
align 16
stack_bottom:
  resb 16384 ; 16kb (gotta think big)
stack_top:

section .text
%include "kernel/arch/i686/gdt.s"
