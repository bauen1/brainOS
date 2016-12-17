; boot.s - entry point
MB_HEADER_MAGIC equ 0x1BADB002
MB_EAX_MAGIC equ 0x2BADB002
MB_PAGE_ALIGN	equ 1<<0 ; align on 4kb boundary
MB_MEMORY_INFO	equ 1<<1 ; request mem_info struct
MB_HEADER_FLAGS equ MB_PAGE_ALIGN | MB_MEMORY_INFO
MB_CHECKSUM equ -(MB_HEADER_MAGIC + MB_HEADER_FLAGS)

STACK_SIZE equ 16384 ; 16kb (gotta think big)

bits 32

section ._multiboot_header

extern code, bss, end

align 4
_multiboot_header:
                dd MB_HEADER_MAGIC                ;
                dd MB_HEADER_FLAGS                ;
                dd MB_CHECKSUM                    ;

                dd _multiboot_header              ;
                dd code                           ;
                dd bss                            ;
                dd end                            ;
                dd _boot                          ;

section .text
global _boot:function _boot.end-_boot
extern kmain
_boot:
                  cli                             ; first order: disable interrupts
                  cmp eax, MB_EAX_MAGIC
                  jne .no_multiboot

                  mov esp, stack                  ; setup stack

                  push esp                        ; save it for the kernel because lazy
                  push STACK_SIZE
                  push ebx                        ; multiboot header

                  cmp eax, MB_EAX_MAGIC           ; magic is in the air

                  call kmain                      ; int kmain (multiboot_t *multiboot_info, uint32_t stack_size, uintptr_t esp);

.halt:            cli                             ; if we ever return just go to sleep
                  hlt
                  jmp .halt

.no_multiboot:    mov dword [0xb8000], 0x4f4e4f4f ; NO (trying to do the least possible but still letting the user know = i'm lazy)
                  jmp .halt
.end:

%include "kernel/arch/i686/gdt-flush.s"
%include "kernel/arch/i686/idt-flush.s"
%include "kernel/arch/i686/interrupts.s"

section .bss
align 16
  resb STACK_SIZE
stack:
