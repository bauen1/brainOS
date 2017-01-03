#ifndef IDT_H
#define IDT_H

#include <stddef.h>
#include <stdint.h>

typedef struct idt_entry {
  uint16_t offset_low;
  uint16_t selector;
  uint8_t zero;
  uint8_t type_attr;
  uint16_t offset_high;
} __attribute__((packed)) idt_entry_t;

typedef struct registers { // This is what the stack looks like to the C interrupt handler
  uint32_t gs;
  uint32_t fs;
  uint32_t es;
  uint32_t ds;

  uint32_t edi;
  uint32_t esi;
  uint32_t ebp;
  uint32_t esp;
  uint32_t ebx;
  uint32_t edx;
  uint32_t ecx;
  uint32_t eax;

  uint32_t isr_num;
  uint32_t err_code;

  uint32_t eip;
  uint32_t cs;
  uint32_t eflags;
  uint32_t useresp;
  uint32_t ss;
} __attribute__((packed)) registers_t;

void idt_install();

typedef void (* isr_t)(struct registers *);

void set_isr_handler(uint8_t i, isr_t handler);
#define set_irq_handler(i, h) (set_isr_handler((i)+32, (h)))

#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1

#define PIC_EOI 0x20

#define IRQ0 0x20
#define IRQ1 0x21
#define IRQ2 0x22
#define IRQ3 0x23
#define IRQ4 0x24
#define IRQ5 0x25
#define IRQ6 0x26
#define IRQ7 0x27
#define IRQ8 0x28
#define IRQ9 0x29
#define IRQ10 0x2a
#define IRQ11 0x2b
#define IRQ12 0x2c
#define IRQ13 0x2d
#define IRQ14 0x2e
#define IRQ15 0x2f

// exceptions
extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

// IRQs
extern void isr32();
extern void isr33();
extern void isr34();
extern void isr35();
extern void isr36();
extern void isr37();
extern void isr38();
extern void isr39();
extern void isr40();
extern void isr41();
extern void isr42();
extern void isr43();
extern void isr44();
extern void isr45();
extern void isr46();
extern void isr47();

// syscall
extern void isr128();

#endif
