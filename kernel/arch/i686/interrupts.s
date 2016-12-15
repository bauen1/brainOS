extern isr_handler

isr_common_stub:
            pusha
            mov ax, ds
            push eax
            mov ax, 0x10
            mov ds, ax
            mov es, ax
            mov fs, ax
            mov gs, ax

            call isr_handler

            pop ebx
            mov ds, bx
            mov es, bx
            mov fs, bx
            mov gs, bx

            popa
            add esp, 8
            sti
            iret

global isr0
isr0:       cli
            push byte 0
            push byte 0
            jmp isr_common_stub
global isr1
isr1:       jmp common_stub
global isr2
isr2:       jmp common_stub
global isr3
isr3:       jmp common_stub
global isr4
isr4:       jmp common_stub
global isr5
isr5:       jmp common_stub
global isr6
isr6:       jmp common_stub
global isr7
isr7:       jmp common_stub
global isr8
isr8:       jmp common_stub
global isr9
isr9:       jmp common_stub
global isr10
isr10:       jmp common_stub
global isr11
isr11:       jmp common_stub
global isr12
isr12:       jmp common_stub
global isr13
isr13:       jmp common_stub

common_stub:  mov eax, 0x4f4e4f4f
              mov dword [0xb8000], eax
              hlt
              jmp common_stub

global isr14
isr14:
global isr15
isr15:
global isr16
isr16:
global isr17
isr17:
global isr18
isr18:
global isr19
isr19:
global isr20
isr20:
global isr21
isr21:
global isr22
isr22:
global isr23
isr23:
global isr24
isr24:
global isr25
isr25:
global isr26
isr26:
global isr27
isr27:
global isr28
isr28:
global isr29
isr29:
global isr30
isr30:
global isr31
isr31:        jmp common_stub
