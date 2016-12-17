#if !defined(__cplusplus)
#include <stdbool.h>
#endif

#include <stddef.h>
#include <stdint.h>

#include "multiboot.h"
#include "system.h"
#include "vga.h"
#include "tty.h"
#include "idt.h"
#include "gdt.h"
#include "keyboard.h"

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

void kpanic (struct registers * registers) {
  // TODO: Make this more epic, blue background etc everyone loves a good old BSOD
  puts("KERNEL PANIC:\n");
  puts((char *)exception[registers->isr_num]);
  putc('\n');
  puthex("ds:       ", registers->ds);
  puthex("edi:      ", registers->edi);
  puthex("esi:      ", registers->esi);
  puthex("ebp:      ", registers->ebp);
  puthex("esp:      ", registers->esp);
  puthex("ebx:      ", registers->ebx);
  puthex("edx:      ", registers->edx);
  puthex("ecx:      ", registers->ecx);
  puthex("eax:      ", registers->eax);
  puthex("isr_num:  ", registers->isr_num);
  puthex("err_code: ", registers->err_code);
  puthex("eip:      ", registers->eip);
  puthex("cs:       ", registers->cs);
  puthex("eflags:   ", registers->eflags);
  puthex("useresp:  ", registers->useresp);
  puthex("ss:       ", registers->ss);

  __asm__ __volatile__ ("cli");
  for(;;){
    __asm__ __volatile__ ("hlt");
  }
}

int kmain (multiboot_t * mboot, uint32_t stack_size, uintptr_t esp) {
  tty_init();
  tty_set_attribute(get_attribute(VGA_COLOR_WHITE, VGA_COLOR_CYAN));
  puts("+------------------------------------------------------------------------------+\n");
  puts("| brainOS v0.1 MIT Licence 2016 bauen1                                         |\n");
  puts("+------------------------------------------------------------------------------+\n");
  tty_set_attribute(get_attribute(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
  putc('\n');
  puts("boot information:\n");
  puthex("stack size:         ", stack_size);
  puthex("esp:                ", esp);

  putc('\n');
  puthex("mboot->flags:       ", mboot->flags);
  putc('\n');

  char buf[64];
  memset((void *)buf, 0, 20);
  itoa(mboot->mem_upper - mboot->mem_lower, (char *)&buf, 10);
  puts("calculated memory (from mboot->mem_upper - mboot->mem_lower):    ");
  puts(&buf[0]);
  puts("kb\n");

  gdt_init();
  idt_install();

  keyboard_install();
  //time_init();
  //pci_install();

  for (int i = 0; i < 32; i++) {
    set_isr_handler(i, kpanic);
  }
  putc(10/0);

  __asm__ __volatile__("sti");  // enable interrupts

  for(;;){
    __asm__ __volatile__ ("hlt");
  }

  return 1;
}
