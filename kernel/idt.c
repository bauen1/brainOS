#include "idt.h"
#include "system.h"

#include <stddef.h>
#include <stdint.h>

static idt_entry_t idt_entries[256];

static void idt_set_gate(uint8_t i, uint32_t offset, uint16_t selector, uint8_t flags) {
  idt_entries[i].offset_low = offset & 0xFFFF;
  idt_entries[i].selector = selector;
  idt_entries[i].zero = 0;
  idt_entries[i].type_attr = flags & 0xFF;
  idt_entries[i].offset_high = (offset >> 16) & 0xFFFF;
}

extern void idt_flush(uint32_t);

static struct {
  uint16_t limit;
  uint32_t base;
} __attribute__((packed)) idt_p;

void idt_install() {
  memset((void*)&idt_entries, 0x0, sizeof(struct idt_entry) * 256);

  // initialize the PIC
  outportb(PIC1_COMMAND, 0x11);
  outportb(PIC2_COMMAND, 0x11);

  outportb(PIC1_DATA, 0x20); // IRQ0-7 maps to ISR32-39
  outportb(PIC2_DATA, 0x28); // IRQ8-15 maps to ISR40-47

  outportb(PIC1_DATA, 0x04);
  outportb(PIC2_DATA, 0x02);

  outportb(PIC1_DATA, 0x01);
  outportb(PIC2_DATA, 0x01);

  outportb(PIC1_DATA, 0x00);
  outportb(PIC2_DATA, 0x00);
  //

  idt_set_gate( 0, (uint32_t)isr0 , 0x08, 0x8E);
  idt_set_gate( 1, (uint32_t)isr1 , 0x08, 0x8E);
  idt_set_gate( 2, (uint32_t)isr2 , 0x08, 0x8E);
  idt_set_gate( 3, (uint32_t)isr3 , 0x08, 0x8E);
  idt_set_gate( 4, (uint32_t)isr4 , 0x08, 0x8E);
  idt_set_gate( 5, (uint32_t)isr5 , 0x08, 0x8E);
  idt_set_gate( 6, (uint32_t)isr6 , 0x08, 0x8E);
  idt_set_gate( 7, (uint32_t)isr7 , 0x08, 0x8E);
  idt_set_gate( 8, (uint32_t)isr8 , 0x08, 0x8E);
  idt_set_gate( 9, (uint32_t)isr9 , 0x08, 0x8E);
  idt_set_gate(10, (uint32_t)isr10, 0x08, 0x8E);
  idt_set_gate(11, (uint32_t)isr11, 0x08, 0x8E);
  idt_set_gate(12, (uint32_t)isr12, 0x08, 0x8E);
  idt_set_gate(13, (uint32_t)isr13, 0x08, 0x8E);
  idt_set_gate(14, (uint32_t)isr14, 0x08, 0x8E);
  idt_set_gate(15, (uint32_t)isr15, 0x08, 0x8E);
  idt_set_gate(16, (uint32_t)isr16, 0x08, 0x8E);
  idt_set_gate(17, (uint32_t)isr17, 0x08, 0x8E);
  idt_set_gate(18, (uint32_t)isr18, 0x08, 0x8E);
  idt_set_gate(19, (uint32_t)isr19, 0x08, 0x8E);
  idt_set_gate(20, (uint32_t)isr20, 0x08, 0x8E);
  idt_set_gate(21, (uint32_t)isr21, 0x08, 0x8E);
  idt_set_gate(22, (uint32_t)isr22, 0x08, 0x8E);
  idt_set_gate(23, (uint32_t)isr23, 0x08, 0x8E);
  idt_set_gate(24, (uint32_t)isr24, 0x08, 0x8E);
  idt_set_gate(25, (uint32_t)isr25, 0x08, 0x8E);
  idt_set_gate(26, (uint32_t)isr26, 0x08, 0x8E);
  idt_set_gate(27, (uint32_t)isr27, 0x08, 0x8E);
  idt_set_gate(28, (uint32_t)isr28, 0x08, 0x8E);
  idt_set_gate(29, (uint32_t)isr29, 0x08, 0x8E);
  idt_set_gate(30, (uint32_t)isr30, 0x08, 0x8E);
  idt_set_gate(31, (uint32_t)isr31, 0x08, 0x8E);

  idt_set_gate(32, (uint32_t)irq0 , 0x08, 0x8E);
  idt_set_gate(33, (uint32_t)irq1 , 0x08, 0x8E);
  idt_set_gate(34, (uint32_t)irq2 , 0x08, 0x8E);
  idt_set_gate(35, (uint32_t)irq3 , 0x08, 0x8E);
  idt_set_gate(36, (uint32_t)irq4 , 0x08, 0x8E);
  idt_set_gate(37, (uint32_t)irq5 , 0x08, 0x8E);
  idt_set_gate(38, (uint32_t)irq6 , 0x08, 0x8E);
  idt_set_gate(39, (uint32_t)irq7 , 0x08, 0x8E);
  idt_set_gate(40, (uint32_t)irq8 , 0x08, 0x8E);
  idt_set_gate(41, (uint32_t)irq9 , 0x08, 0x8E);
  idt_set_gate(43, (uint32_t)irq10, 0x08, 0x8E);
  idt_set_gate(44, (uint32_t)irq11, 0x08, 0x8E);
  idt_set_gate(45, (uint32_t)irq12, 0x08, 0x8E);
  idt_set_gate(46, (uint32_t)irq13, 0x08, 0x8E);
  idt_set_gate(47, (uint32_t)irq14, 0x08, 0x8E);
  idt_set_gate(48, (uint32_t)irq15, 0x08, 0x8E);

  //idt_set_gate(0x80, (uint32_t)isr0, 0x08, 0x8E);

  idt_p.limit = sizeof(struct idt_entry) * 256 - 1;
  idt_p.base = (uint32_t)&idt_entries;

  //puthex("(uint32_t)&idt_p:      ", (uint32_t)&idt_p);
  //puthex("(uint32_t)idt_p.limit: ", (uint32_t)idt_p.limit);
  //puthex("(uint32_t)idt_p.base:  ", (uint32_t)idt_p.base);


  idt_flush((uint32_t)&idt_p);
  //putc(10/0);
}

const char* exception[] = {
  "Division by Zero",
  "Debug Exception",
  "NMI",
  "Breakpoint",
  "into detected overflow",
  "Out of Bounds",
  "Invalid Opcode",
  "No coprocessor",
  "Double Fault",
  "coprocessor segment overrun",
  "Bad TSS",
  "Segment not present",
  "Stack fault",
  "General Protection Fault",
  "Page Fault",
  "Unknown interrupt exception",
  "coprocessor fault",
  "alignment check exception",
  "machine check exception",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
};

isr_t isr_handlers[257];

static void call_isr_handler(struct registers registers) {
  if (isr_handlers[registers.isr_num] != 0) {
    isr_handlers[registers.isr_num](registers);
  } else {
    if (isr_handlers[256] != 0) {
      isr_handlers[256](registers);
    }
  }
}

void set_isr_handler(uint8_t i, isr_t handler) {
  isr_handlers[i] = handler;
}

void set_default_isr_handler(isr_t handler) {
  isr_handlers[256] = handler;
}

void isr_handler(struct registers registers) __attribute__((optimize("0"))); // Disable optimization because gcc likes to mock around with the argument stack otherwise
void isr_handler(struct registers registers) {
  /*puts(exception[registers.isr_num]);
  putc('\n');
  puthex("ds:       ", registers.ds);
  puthex("edi:      ", registers.edi);
  puthex("esi:      ", registers.esi);
  puthex("ebp:      ", registers.ebp);
  puthex("esp:      ", registers.esp);
  puthex("ebx:      ", registers.ebx);
  puthex("edx:      ", registers.edx);
  puthex("ecx:      ", registers.ecx);
  puthex("eax:      ", registers.eax);
  puthex("isr_num:  ", registers.isr_num);
  puthex("err_code: ", registers.err_code);
  puthex("eip:      ", registers.eip);
  puthex("cs:       ", registers.cs);
  puthex("eflags:   ", registers.eflags);
  puthex("useresp:  ", registers.useresp);
  puthex("ss:       ", registers.ss);

  __asm__ __volatile__ ("cli");
  for(;;){
    __asm__ __volatile__ ("hlt");
  }*/
  call_isr_handler(registers);
}

// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=27234
void irq_handler(volatile struct registers registers) __attribute__((optimize("-O0"))); // Disable optimization because gcc likes to mock around with the argument stack otherwise
void irq_handler(volatile struct registers registers) {
  if (registers.isr_num >= 40) { // in this case irq num
    outportb(PIC2_COMMAND, PIC_EOI);
  }

  outportb(PIC1_COMMAND, PIC_EOI);

  call_isr_handler(registers);
}
