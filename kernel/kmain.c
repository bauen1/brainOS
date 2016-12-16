#if !defined(__cplusplus)
#include <stdbool.h>
#endif

#include <stddef.h>
#include <stdint.h>

#include "multiboot.h"
#include "system.h"
#include "tty.h"
#include "idt.h"
#include "gdt.h"

__attribute__((OS_main));
int kmain (multiboot_t *multiboot_info, uint32_t stack_size, uintptr_t esp) {
  tty_init();
  puts("brainOS v0.1\n");
  puts("boot information\n");
  puthex("stack size:      ", stack_size);
  puthex("esp:             ", esp);

  puts("multiboot_info flags:");
  char buffer[1024];
  itoa(multiboot_info->flags, &buffer[0], 2);
  puts(&buffer[0]);
  putc('\n');

  gdt_init(esp);

  puts("pre idt_install();\n");
  idt_install(esp);
  puts("post idt_install();\n");

  __asm__ __volatile__("sti");  // enable interrupts

  for(;;){
    __asm__ __volatile__ ("hlt");
  }

  puts("Nothing has blown up yet \\o/\n");

  return 1;
}