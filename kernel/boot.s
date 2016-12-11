MULTIBOOT_HEADER_MAGIC equ 0x1BADB002
MULTIBOOT_PAGE_ALIGN	equ 1<<0
MULTIBOOT_MEMORY_INFO	equ 1<<1
MULTIBOOT_AOUT_KLUDGE	equ 1<<16
MULTIBOOT_HEADER_FLAGS equ MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO | MULTIBOOT_AOUT_KLUDGE
MULTIBOOT_CHECKSUM equ -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

section .text
global _start
_start:
              mov esp, stack_top
              jmp start

align 4
mboot:
      dd MULTIBOOT_HEADER_MAGIC
      dd MULTIBOOT_HEADER_FLAGS
      dd MULTIBOOT_CHECKSUM

      extern code, bss, end
      dd mboot
      dd code
      dd bss
      dd end
      dd _start

section .text
start:
                  cmp eax, 0x2BADB002             ; magic is in the air
                  je .multiboot
                  xor eax, eax
                  mov al, 'M'
                  jmp panic
.multiboot:
                  call check_cpuid
                  call check_long_mode

                  extern kmain
                  call kmain

                  mov al, 'R'
                  jmp halt
.end:

check_cpuid:      pushfd                          ; http://wiki.osdev.org/Setting_Up_Long_Mode#Detection_of_CPUID
                  pop eax
                  mov ecx, eax

                  xor eax, 1 << 21

                  push eax
                  popfd

                  pushfd
                  pop eax

                  push ecx
                  popfd

                  cmp eax, ecx
                  je .no_cpuid
                  ret

.no_cpuid:        mov al, '2'
                  jmp panic

check_long_mode:  mov eax, 0x80000000
                  cpuid
                  cmp eax, 0x80000001
                  jb .no_long_mode
                  mov eax, 0x80000001
                  cpuid
                  test edx, 1 << 29
                  jz .no_long_mode
                  ret
.no_long_mode:    mov al, '3'
                  jmp panic

panic:            mov dword [0xb8000], 0x4f524f45 ; 'ER'
                  mov dword [0xb8004], 0x4f3a4f52 ; 'R:'
                  mov dword [0xb8008], 0x4f204f20 ; ' '
                  mov byte  [0xb800a], al
                  jmp halt

global gdt_flush
extern gp
gdt_flush:        lgdt [gp]
                  mov ax, 0x10
                  mov ds, ax
                  mov es, ax
                  mov fs, ax
                  mov gs, ax
                  mov ss, ax
                  jmp 0x08:.flush2
.flush2:          ret

global halt
halt:             cli
.hang:            hlt
                  jmp .hang


section .bss
stack_bottom:
  resb 8192 ; 8kb
stack_top:
