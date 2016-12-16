extern isr_handler
extern irq_handler

isr_common_stub:
              pusha

              mov ax, ds
              push eax            ; save the ds

              mov ax, 0x10        ; kernel data segment
              mov ds, ax
              mov es, ax
              mov fs, ax
              mov gs, ax

              call isr_handler

              pop ebx             ; original ds
              mov ds, bx
              mov es, bx
              mov fs, bx
              mov gs, bx

              popa
              add esp, 8          ; removes the error code & isr num
              sti
              iret

irq_common_stub:
              pushad              ; push eax, ecx, edx, ebx, original esp, ebp, esi, edi

              mov ax, ds
              push eax            ; save the ds

              mov ax, 0x10        ; kernel data segment
              mov ds, ax
              mov es, ax
              mov fs, ax
              mov gs, ax

              call irq_handler

              pop ebx
              mov ds, bx
              mov es, bx
              mov fs, bx
              mov gs, bx

              popad               ; pops eax, ecx, edx, ebx, original esp, ebp, esi, edi
              add esp, 8          ; removes the error code & isr num
              sti
              iretd

global isr0
isr0:         cli
              push byte 0
              push byte 0
              jmp isr_common_stub
global isr1
isr1:         cli
              push byte 0
              push byte 1
              jmp isr_common_stub
global isr2
isr2:         cli
              push byte 0
              push byte 2
              jmp isr_common_stub
global isr3
isr3:         cli
              push byte 0
              push byte 3
              jmp isr_common_stub
global isr4
isr4:         cli
              push byte 0
              push byte 4
              jmp isr_common_stub
global isr5
isr5:         cli
              push byte 0
              push byte 5
              jmp isr_common_stub
global isr6
isr6:         cli
              push byte 0
              push byte 6
              jmp isr_common_stub
global isr7
isr7:         cli
              push byte 0
              push byte 7
              jmp isr_common_stub
global isr8
isr8:         cli
              ;push byte 0
              push byte 8
              jmp isr_common_stub
global isr9
isr9:         cli
              push byte 0
              push byte 9
              jmp isr_common_stub
global isr10
isr10:        cli
              ;push byte 0
              push byte 10
              jmp isr_common_stub
global isr11
isr11:        cli
              ;push byte 0
              push byte 11
              jmp isr_common_stub
global isr12
isr12:        cli
              ;push byte 0
              push byte 12
              jmp isr_common_stub
global isr13
isr13:        cli
              ;push byte 0
              push byte 13
              jmp isr_common_stub
global isr14
isr14:        cli
              ;push byte 0
              push byte 14
              jmp isr_common_stub
global isr15
isr15:        cli
              push byte 0
              push byte 15
              jmp isr_common_stub
global isr16
isr16:        cli
              push byte 0
              push byte 16
              jmp isr_common_stub
global isr17
isr17:        cli
              push byte 0
              push byte 17
              jmp isr_common_stub
global isr18
isr18:        cli
              push byte 0
              push byte 18
              jmp isr_common_stub
global isr19
isr19:        cli
              push byte 0
              push byte 19
              jmp isr_common_stub
global isr20
isr20:        cli
              push byte 0
              push byte 20
              jmp isr_common_stub
global isr21
isr21:        cli
              push byte 0
              push byte 21
              jmp isr_common_stub
global isr22
isr22:        cli
              push byte 0
              push byte 22
              jmp isr_common_stub
global isr23
isr23:        cli
              push byte 0
              push byte 23
              jmp isr_common_stub
global isr24
isr24:        cli
              push byte 0
              push byte 24
              jmp isr_common_stub
global isr25
isr25:        cli
              push byte 0
              push byte 25
              jmp isr_common_stub
global isr26
isr26:        cli
              push byte 0
              push byte 26
              jmp isr_common_stub
global isr27
isr27:        cli
              push byte 0
              push byte 27
              jmp isr_common_stub
global isr28
isr28:        cli
              push byte 0
              push byte 28
              jmp isr_common_stub
global isr29
isr29:        cli
              push byte 0
              push byte 29
              jmp isr_common_stub
global isr30
isr30:        cli
              push byte 0
              push byte 30
              jmp isr_common_stub
global isr31
isr31:        cli
              push byte 0
              push byte 31
              jmp isr_common_stub

global irq0
irq0:         cli
              push byte 0
              push byte 32
              jmp irq_common_stub
global irq1
irq1:         cli
              push byte 0
              push byte 33
              jmp irq_common_stub
global irq2
irq2:         cli
              push byte 0
              push byte 34
              jmp irq_common_stub
global irq3
irq3:         cli
              push byte 0
              push byte 35
              jmp irq_common_stub
global irq4
irq4:         cli
              push byte 0
              push byte 36
              jmp irq_common_stub
global irq5
irq5:         cli
              push byte 0
              push byte 37
              jmp irq_common_stub
global irq6
irq6:         cli
              push byte 0
              push byte 38
              jmp irq_common_stub
global irq7
irq7:         cli
              push byte 0
              push byte 39
              jmp irq_common_stub
global irq8
irq8:         cli
              push byte 0
              push byte 40
              jmp irq_common_stub
global irq9
irq9:         cli
              push byte 0
              push byte 41
              jmp irq_common_stub
global irq10
irq10:        cli
              push byte 0
              push byte 42
              jmp irq_common_stub
global irq11
irq11:        cli
              push byte 0
              push byte 43
              jmp irq_common_stub
global irq12
irq12:        cli
              push byte 0
              push byte 44
              jmp irq_common_stub
global irq13
irq13:        cli
              push byte 0
              push byte 45
              jmp irq_common_stub
global irq14
irq14:        cli
              push byte 0
              push byte 46
              jmp irq_common_stub
global irq15
irq15:        jmp $
              cli
              push byte 0
              push byte 47
              jmp irq_common_stub
