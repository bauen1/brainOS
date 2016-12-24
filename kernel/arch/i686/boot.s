; boot.s - entry point
MB_HEADER_MAGIC equ 0x1BADB002
MB_EAX_MAGIC equ 0x2BADB002
MB_PAGE_ALIGN	equ 1<<0                      ; align on 4kb boundary
MB_MEMORY_INFO	equ 1<<1                    ; request mem_info struct
MB_HEADER_FLAGS equ MB_PAGE_ALIGN | MB_MEMORY_INFO
MB_CHECKSUM equ -(MB_HEADER_MAGIC + MB_HEADER_FLAGS)

STACK_SIZE equ 32768                        ; 32kb (gotta think big)

bits 32

extern code, bss, end, kmain

section ._multiboot_header

align 4
_multiboot_header:
                dd MB_HEADER_MAGIC          ;
                dd MB_HEADER_FLAGS          ;
                dd MB_CHECKSUM              ;

                dd _multiboot_header        ;
                dd code                     ;
                dd bss                      ;
                dd end                      ;
                dd _boot                    ;

section .text
global _boot:function _boot.end-_boot
_boot:
                  ; 9 tabs (2 spaces)       ; and 13 more to here
                  cli                       ; first order: disable interrupts
                  cmp eax, MB_EAX_MAGIC
                  jne .no_multiboot

                  mov esp, stack            ; setup stack

                  push stack                ; pass stack start
                  push STACK_SIZE           ; and size to the kernel
                  push ebx                  ; and don't forget a pointer to the multiboot header

                  cmp eax, MB_EAX_MAGIC     ; magic is in the air

                  call kmain                ; int kmain (multiboot_t *multiboot_info, uint32_t stack_size, uintptr_t esp);

.halt:            cli                       ; if we ever return just go to sleep
                  hlt
                  jmp .halt

.no_multiboot:    ; NO (trying to do the least possible but still letting the user know = i'm lazy)
                  mov dword[0xb8000], 0x4f4e4f4f
                  jmp .halt
.end:

%include "kernel/arch/i686/gdt-flush.s"
%include "kernel/arch/i686/idt-flush.s"
%include "kernel/arch/i686/interrupts.s"
%include "kernel/arch/i686/paging.s"

section .bss
align 16
  resb STACK_SIZE
stack:
